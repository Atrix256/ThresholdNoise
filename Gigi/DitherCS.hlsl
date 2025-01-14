// Unnamed technique, shader Threshold
/*$(ShaderResources)*/

#include "Shared.hlsli"

float ConvertToNBits(float value, int bits, float ditherRnd)
{
	int multiplier = (int(1) << (bits)) - 1;
	return floor(value * multiplier + ditherRnd) / multiplier;
}

float3 ConvertToNBits(float3 value, int bits, float3 ditherRnd)
{
	return float3(
		ConvertToNBits(value.r, bits, ditherRnd.r),
		ConvertToNBits(value.g, bits, ditherRnd.g),
		ConvertToNBits(value.b, bits, ditherRnd.b)
	);
}

float ReadNoiseTexture(Texture2DArray<float> tex, uint3 px, float2 offsetF)
{
	// TODO: use offsetF. multiply by dims.
	uint3 noiseDims;
	tex.GetDimensions(noiseDims.x, noiseDims.y, noiseDims.z);
	int3 readPos;
	readPos.xy = (px.xy + int2(offsetF * float2(noiseDims.xy)));
	readPos.z = px.z;
	return tex[readPos % noiseDims];
}

float GetRng(uint3 px, int noiseType, inout uint wangState, int index)
{
	float2 offsetF = R2LDS(index);

	switch (noiseType)
	{
		case NoiseTypes::White: return float3(wang_hash_float01(wangState), wang_hash_float01(wangState), wang_hash_float01(wangState));
		case NoiseTypes::Blue2D: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue2D/blue2d_%i.png:R8_Unorm:float:false)*/, px, offsetF);
		case NoiseTypes::STBN_10: return ReadNoiseTexture(/*$(Image2DArray:Textures/STBN_10/stbn_scalar_10_2Dx1Dx1D_128x128x32x1_%i.png:R8_Unorm:float:false)*/, px, offsetF);
		case NoiseTypes::STBN_19: return ReadNoiseTexture(/*$(Image2DArray:Textures/STBN_19/stbn_scalar_19_2Dx1Dx1D_128x128x32x1_%i.png:R8_Unorm:float:false)*/, px, offsetF);
		case NoiseTypes::FAST_Blue_Exp: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue_Exp/real_uniform_gauss1_0_exp0101_separate05_%i.png:R8_Unorm:float:false)*/, px, offsetF);
		case NoiseTypes::FAST_Binomial3x3_Exp: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Binomial3x3_Exp/real_uniform_binomial3x3_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, offsetF);
		case NoiseTypes::FAST_Box3x3_Exp: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Box3x3_Exp/real_uniform_box3x3_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, offsetF);
		case NoiseTypes::R2:
		{
			 // TODO: how to handle px.z? maybe a white noise offset.
			 // If so, note in the blog notes that we don't have a way to animate R2
			return R2LDG(px.xy + int2(offsetF * 512.0f));
		}
		case NoiseTypes::IGN:
		{
			return IGNLDG(int3(px.xy + int2(offsetF * 512.0f), px.z));
		}
		case NoiseTypes::Bayer:
		{
			 // TODO: how to handle px.z? maybe a white noise offset.
			 // If so, note in the blog notes that we don't have a way to animate R2
			return Bayer(px.x, px.y, 4);
		}
		case NoiseTypes::Round:
		{
			return 0.5f;
		}
		case NoiseTypes::Floor:
		{
			return 0.0f;
		}
	}
	return 0.0f;
}

float3 GetRng3(uint3 px, int noiseType, inout uint wangState)
{
	float3 ret;
	ret.x = GetRng(px, noiseType, wangState, 0);
	ret.y = GetRng(px, noiseType, wangState, 1);
	ret.z = GetRng(px, noiseType, wangState, 2);
	return ret;
}

/*$(_compute:csmain)*/(uint3 DTid : SV_DispatchThreadID)
{
	// Get the color of the pixel
	uint2 dims;
	Input.GetDimensions(dims.x, dims.y);
	uint3 px = uint3(DTid.xy, /*$(Variable:FrameIndex)*/);
	float4 color = Input[px.xy % dims];

	// Get the parameters for whatever quadrant we are in
	int noiseType = 0;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
			noiseType = /*$(Variable:NoiseType1)*/;
		else
			noiseType = /*$(Variable:NoiseType3)*/;
	}
	else
	{
		if (px.y <= dims.y)
			noiseType = /*$(Variable:NoiseType2)*/;
		else
			noiseType = /*$(Variable:NoiseType4)*/;
	}

	// Initialize wang hash
	uint wangState = wang_hash_init(px);

	// Dither
	float3 rng = GetRng3(px, noiseType, wangState);
	color.rgb = ConvertToNBits(color.rgb, /*$(Variable:BitsPerColorChannel)*/, rng);

	// Write out result
	color.rgb = LinearToSRGB(color.rgb);
	Output[px.xy] = color;
}

/*
Shader Resources:
	Texture Input (as SRV)
	Texture Output (as UAV)
*/

/*

TODO:
* noise types for dithering
  * also some of that competitive blue noise?
  * https://tellusim.com/improved-blue-noise/
  * https://acko.net/blog/stable-fiddusion/
 ? also round and floor options?

* Finish bayer
* Need to dive deeper into Bayer. Can you use it like you are, or do you need different logic? also verify the function works correctly. Can include that in the blog post.
 * could also try with a bayer texture.
 
* for temporal could also have EMA with neighborhood clamp.

* code generate C++ dx12 too.

* look for TODOs in this file

* checkbox to do per channel dithering?

* golden ratio animated blue noise?

BLOG NOTES:
* Threshold test as a second blog post!
* compare round and floor vs something nicely dithered (could maybe even be white noise?)
* show difference between STBN 1.0 and 1.9
* show difference between blue2d and a temporal blue noise. if you gauss blur, it's less compelling than if you don't!

* Link to STBN and FAST repos.
 * also the competitive blue noise



*/