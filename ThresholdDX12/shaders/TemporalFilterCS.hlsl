// Unnamed technique, shader TemporalFilter


struct NoiseTypes
{
    static const int White = 0;
    static const int Blue2D_Offset = 1;
    static const int Blue2D_Flipbook = 2;
    static const int Blue2D_Golden_Ratio = 3;
    static const int STBN_10 = 4;
    static const int STBN_19 = 5;
    static const int FAST_Blue_Exp_Separate = 6;
    static const int FAST_Blue_Exp_Product = 7;
    static const int FAST_Triangle_Blue_Exp_Separate = 8;
    static const int FAST_Triangle_Blue_Exp_Product = 9;
    static const int FAST_Binomial3x3_Exp = 10;
    static const int FAST_Box3x3_Exp = 11;
    static const int Blue_Tellusim_128_128_64 = 12;
    static const int Blue_Stable_Fiddusion = 13;
    static const int R2 = 14;
    static const int IGN = 15;
    static const int Bayer4 = 16;
    static const int Bayer16 = 17;
    static const int Bayer64 = 18;
    static const int Bayer256 = 19;
    static const int Round = 20;
    static const int Floor = 21;
};

struct SpatialFilters
{
    static const int None = 0;
    static const int Box = 1;
    static const int Gauss = 2;
};

struct TemporalFilters
{
    static const int None = 0;
    static const int Ema = 1;
    static const int EMA_plus_Clamp = 2;
    static const int Monte_Carlo = 3;
};

struct Struct__TemporalFilterCB
{
    uint Reset_Accumulation;
    int TemporalFilter1;
    int TemporalFilter2;
    int TemporalFilter3;
    int TemporalFilter4;
    float TemporalFilterAlpha1;
    float TemporalFilterAlpha2;
    float TemporalFilterAlpha3;
    float TemporalFilterAlpha4;
    float3 _padding0;
};

Texture2D<float4> Input : register(t0);
RWTexture2D<float4> Output : register(u0);
RWTexture2D<float4> TemporalAccum : register(u1);
ConstantBuffer<Struct__TemporalFilterCB> _TemporalFilterCB : register(b0);

#line 2


#include "Shared.hlsli"

[numthreads(8, 8, 1)]
#line 6
void csmain(uint3 DTid : SV_DispatchThreadID)
{
	// Calculate the size of each quadrant
	uint2 dims;
	Input.GetDimensions(dims.x, dims.y);
	dims /= 2;

	uint2 px = DTid.xy;

	// Get the parameters for whatever quadrant we are in
	float temporalAlpha = 1.0f;
	int temporalFilter = TemporalFilters::None;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
		{
			temporalAlpha = _TemporalFilterCB.TemporalFilterAlpha1;
			temporalFilter = _TemporalFilterCB.TemporalFilter1;
		}
		else
		{
			temporalAlpha = _TemporalFilterCB.TemporalFilterAlpha3;
			temporalFilter = _TemporalFilterCB.TemporalFilter3;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			temporalAlpha = _TemporalFilterCB.TemporalFilterAlpha2;
			temporalFilter = _TemporalFilterCB.TemporalFilter2;
		}
		else
		{
			temporalAlpha = _TemporalFilterCB.TemporalFilterAlpha4;
			temporalFilter = _TemporalFilterCB.TemporalFilter4;
		}
	}
	bool neighborhoodClamp = (temporalFilter == TemporalFilters::EMA_plus_Clamp);

	// Get the previous and next colors
	float4 temporalAccum = TemporalAccum[px];
	float3 lastValue = temporalAccum.rgb;
	float3 nextValue = Input[px].rgb;

	// Reset temporal buffer when the temporal filter parameters change, or when the temporal accumulation buffer doesn't have data in it yet (alpha is 0)
	// Also reset every frame if the filter is "none"
	// Also reset if the user clicks "Reset Accumulation"
	if (temporalAccum.a < 1.0f || temporalFilter == TemporalFilters::None)
		temporalAlpha = 1.0f;
	temporalAlpha = clamp(temporalAlpha, 0.0f, 1.0f);

	// Logic for monte carlo
	float alphaOut = 1.0f;
	if (temporalFilter == TemporalFilters::Monte_Carlo)
	{
		if (_TemporalFilterCB.Reset_Accumulation)
			temporalAccum.a = 1.0f;

		float sampleCount = max(temporalAccum.a, 1.0f);
		alphaOut = sampleCount + 1.0f;
		temporalAlpha = 1.0f / sampleCount;
	}

	// Do a neighborhood clamp if we should, to simulate that anti ghosting feature of TAA
	if (neighborhoodClamp && temporalAlpha < 1.0f)
	{
		float3 themin = nextValue;
		float3 themax = nextValue;

		for (int iy = -2; iy <= 2; ++iy)
		{
			for (int ix = -2; ix <= 2; ++ix)
			{
				float3 neighborhoodSample = Input[int2(px) + int2(ix, iy)].rgb;
				themin = min(themin, neighborhoodSample);
				themax = max(themax, neighborhoodSample);
			}
		}

		lastValue = clamp(lastValue, themin, themax);
	}

	// Lerp
	float3 value = lerp(lastValue, nextValue, temporalAlpha);

	// Write result to output, and accumulation buffer
	TemporalAccum[px] = float4(value, alphaOut);
	Output[px] = float4(LinearToSRGB(value), 1.0f);
}

/*
Shader Resources:
	Texture Input (as SRV)
	Texture Output (as UAV)
	Texture TemporalAccum (as UAV)
*/
