// Unnamed technique, shader AdjustBrightness
/*$(ShaderResources)*/

#include "Shared.hlsli"

/*$(_compute:csmain)*/(uint3 DTid : SV_DispatchThreadID)
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
			brightnessMultiplier = (/*$(Variable:Brighten1)*/) ? /*$(Variable:BrightnessMultiplier)*/ : 1.0f;
		else
			brightnessMultiplier = (/*$(Variable:Brighten3)*/) ? /*$(Variable:BrightnessMultiplier)*/ : 1.0f;
	}
	else
	{
		if (px.y <= dims.y)
			brightnessMultiplier = (/*$(Variable:Brighten2)*/) ? /*$(Variable:BrightnessMultiplier)*/ : 1.0f;
		else
			brightnessMultiplier = (/*$(Variable:Brighten4)*/) ? /*$(Variable:BrightnessMultiplier)*/ : 1.0f;
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
