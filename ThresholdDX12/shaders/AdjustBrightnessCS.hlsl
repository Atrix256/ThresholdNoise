// Unnamed technique, shader AdjustBrightness


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

struct Struct__AdjustBrightnessCB
{
    uint Brighten1;
    uint Brighten2;
    uint Brighten3;
    uint Brighten4;
    float BrightnessMultiplier;
    float3 _padding0;
};

RWTexture2D<float4> Color : register(u0);
ConstantBuffer<Struct__AdjustBrightnessCB> _AdjustBrightnessCB : register(b0);

#line 2


#include "Shared.hlsli"

[numthreads(8, 8, 1)]
#line 6
void csmain(uint3 DTid : SV_DispatchThreadID)
{
	// Calculate the size of each quadrant
	uint2 dims;
	Color.GetDimensions(dims.x, dims.y);
	dims /= 2;

	uint2 px = DTid.xy;

	// Get brightness multiplier for quadrant
	float brightnessMultiplier = 1.0f;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
			brightnessMultiplier = (_AdjustBrightnessCB.Brighten1) ? _AdjustBrightnessCB.BrightnessMultiplier : 1.0f;
		else
			brightnessMultiplier = (_AdjustBrightnessCB.Brighten3) ? _AdjustBrightnessCB.BrightnessMultiplier : 1.0f;
	}
	else
	{
		if (px.y <= dims.y)
			brightnessMultiplier = (_AdjustBrightnessCB.Brighten2) ? _AdjustBrightnessCB.BrightnessMultiplier : 1.0f;
		else
			brightnessMultiplier = (_AdjustBrightnessCB.Brighten4) ? _AdjustBrightnessCB.BrightnessMultiplier : 1.0f;
	}

	// Apply brightness multiplier
	float3 color = SRGBToLinear(Color[px].rgb);
	color *= brightnessMultiplier;
	Color[px] = float4(LinearToSRGB(color.rgb), 1.0f);
}

/*
Shader Resources:
	Texture Color (as UAV)
*/
