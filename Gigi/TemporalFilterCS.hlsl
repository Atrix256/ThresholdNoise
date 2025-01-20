// Unnamed technique, shader TemporalFilter
/*$(ShaderResources)*/

#include "Shared.hlsli"

/*$(_compute:csmain)*/(uint3 DTid : SV_DispatchThreadID)
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
			temporalAlpha = /*$(Variable:TemporalFilterAlpha1)*/;
			temporalFilter = /*$(Variable:TemporalFilter1)*/;
		}
		else
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha3)*/;
			temporalFilter = /*$(Variable:TemporalFilter3)*/;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha2)*/;
			temporalFilter = /*$(Variable:TemporalFilter2)*/;
		}
		else
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha4)*/;
			temporalFilter = /*$(Variable:TemporalFilter4)*/;
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
		if (/*$(Variable:Reset Accumulation)*/)
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
