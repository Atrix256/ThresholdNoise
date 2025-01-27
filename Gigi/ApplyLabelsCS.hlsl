// Unnamed technique, shader ApplyLabels
/*$(ShaderResources)*/

#include "Shared.hlsli"

void ApplyLabel(inout float3 color, uint2 px, uint2 quadrantDims, Texture2D<float4> label)
{
	uint2 labelDims;
	label.GetDimensions(labelDims.x, labelDims.y);

	if (labelDims.x <= 4 && labelDims.y <= 4)
		return;

	uint2 pxrel = px % quadrantDims;

	if (pxrel.x >= (labelDims.x + 5) || pxrel.y >= (labelDims.y + 5))
		return;

	color = lerp(color, float3(0.0f, 0.0f, 0.0f), 0.75f);

	if (pxrel.x >= labelDims.x || pxrel.y >= labelDims.y)
		return;

	color = lerp(color, float3(1.0f, 1.0f, 1.0f), label[pxrel].a);
}

/*$(_compute:csmain)*/(uint3 DTid : SV_DispatchThreadID)
{
	// Calculate the size of each quadrant
	uint2 dims;
	Color.GetDimensions(dims.x, dims.y);
	dims /= 2;

	uint2 px = DTid.xy;

	// Apply the label based on what quadrant we are in
	float3 color = SRGBToLinear(Color[px].rgb);
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
			ApplyLabel(color, px, dims, Label1_UL);
		else
			ApplyLabel(color, px, dims, Label3_BL);
	}
	else
	{
		if (px.y <= dims.y)
			ApplyLabel(color, px, dims, Label2_UR);
		else
			ApplyLabel(color, px, dims, Label4_BR);
	}

	// Write out result
	color = LinearToSRGB(color);
	Color[px.xy] = float4(color, 1.0f);
}

/*
Shader Resources:
	Texture Color (as UAV)
	Texture Label1_UL (as SRV)
	Texture Label2_UR (as SRV)
	Texture Label3_BL (as SRV)
	Texture Label4_BR (as SRV)
*/
