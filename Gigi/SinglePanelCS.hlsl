// Unnamed technique, shader SinglePanel
/*$(ShaderResources)*/

#include "Shared.hlsli"

/*$(_compute:csmain)*/(uint3 DTid : SV_DispatchThreadID)
{
	Output[DTid.xy] = float4(LinearToSRGB(Input[DTid.xy].rgb), 1.0f);
}

/*
Shader Resources:
	Texture Input (as SRV)
	Texture Output (as UAV)
*/
