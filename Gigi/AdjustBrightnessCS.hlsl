// Unnamed technique, shader AdjustBrightness
/*$(ShaderResources)*/

#include "Shared.hlsli"

/*$(_compute:csmain)*/(uint3 DTid : SV_DispatchThreadID)
{
	uint2 px = DTid.xy;
	float3 color = SRGBToLinear(Color[px].rgb);
	color *= /*$(Variable:BrightnessMultiplier)*/;
	Color[px] = float4(LinearToSRGB(color.rgb), 1.0f);
}

/*
Shader Resources:
	Texture Color (as UAV)
*/
