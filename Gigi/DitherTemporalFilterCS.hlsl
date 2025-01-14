// Unnamed technique, shader DitherTemporalFilter
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
	float temporalAlphaLastFrame = 1.0f;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha1)*/;
			temporalAlphaLastFrame = /*$(Variable:TemporalFilterAlphaLastFrame1)*/;
		}
		else
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha3)*/;
			temporalAlphaLastFrame = /*$(Variable:TemporalFilterAlphaLastFrame3)*/;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha2)*/;
			temporalAlphaLastFrame = /*$(Variable:TemporalFilterAlphaLastFrame2)*/;
		}
		else
		{
			temporalAlpha = /*$(Variable:TemporalFilterAlpha4)*/;
			temporalAlphaLastFrame = /*$(Variable:TemporalFilterAlphaLastFrame4)*/;
		}
	}

	// Reset temporal buffer when the temporal filter parameters change
	if (temporalAlpha != temporalAlphaLastFrame)
		temporalAlpha = 1.0f;

	temporalAlpha = clamp(temporalAlpha, 0.0f, 1.0f);

	float3 lastValue = TemporalAccum[px].rgb;
	float3 nextValue = Input[px].rgb;

	float3 value = lerp(lastValue, nextValue, temporalAlpha);

	TemporalAccum[px] = float4(value, 1.0f);

	Output[px] = float4(LinearToSRGB(value), 1.0f);
}

/*
Shader Resources:
	Texture Input (as SRV)
	Texture Output (as UAV)
	Texture TemporalAccum (as UAV)
*/
