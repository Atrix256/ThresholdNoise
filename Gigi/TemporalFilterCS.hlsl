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
	bool neighborhoodClamp = false;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha1)*/;
			neighborhoodClamp = /*$(Variable:NeighborhoodClamp1)*/;
		}
		else
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha3)*/;
			neighborhoodClamp = /*$(Variable:NeighborhoodClamp3)*/;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha2)*/;
			neighborhoodClamp = /*$(Variable:NeighborhoodClamp2)*/;
		}
		else
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha4)*/;
			neighborhoodClamp = /*$(Variable:NeighborhoodClamp4)*/;
		}
	}

	// Get the previous and next colors
	float4 temporalAccum = TemporalAccum[px];
	float3 lastValue = temporalAccum.rgb;
	float3 nextValue = Input[px].rgb;

	// Reset temporal buffer when the temporal filter parameters change, or when the temporal accumulation buffer doesn't have data in it yet (alpha is 0)
	if (temporalAccum.a < 1.0f)
		temporalAlpha = 1.0f;
	temporalAlpha = clamp(temporalAlpha, 0.0f, 1.0f);

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
	TemporalAccum[px] = float4(value, 1.0f);
	Output[px] = float4(LinearToSRGB(value), 1.0f);
}

/*
Shader Resources:
	Texture Input (as SRV)
	Texture Output (as UAV)
	Texture TemporalAccum (as UAV)
*/
