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
	uint3 noiseDims;
	tex.GetDimensions(noiseDims.x, noiseDims.y, noiseDims.z);
	int3 readPos;
	readPos.xy = (px.xy + int2(offsetF * float2(noiseDims.xy)));
	readPos.z = px.z;
	return tex[readPos % noiseDims];
}

float ReadNoiseTexture(Texture2D<float> tex, uint2 px, float2 offsetF)
{
	uint2 noiseDims;
	tex.GetDimensions(noiseDims.x, noiseDims.y);
	int2 readPos = (px + int2(offsetF * float2(noiseDims.xy)));
	return tex[readPos % noiseDims];
}

float GetRng(uint3 px, int noiseType, inout uint wangStatePixel, inout uint wangStateGlobal, int index)
{
	float2 indexOffsetF = R2LDS(index);

	switch (noiseType)
	{
		case NoiseTypes::White: return wang_hash_float01(wangStatePixel);
		case NoiseTypes::Blue2D: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue2D/blue2d_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF);
		case NoiseTypes::Blue2D_plus_Golden_Ratio:
		{
			float value = ReadNoiseTexture(/*$(Image2D:Textures/FAST_Blue2D/blue2d_0.png:R8_Unorm:float:false)*/, px.xy, indexOffsetF);
			int frameIndex = px.z % 64;
			value = frac(value + frameIndex * c_goldenRatioConjugate);
			return value;
		}
		case NoiseTypes::STBN_10: return ReadNoiseTexture(/*$(Image2DArray:Textures/STBN_10/stbn_scalar_10_2Dx1Dx1D_128x128x32x1_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF);
		case NoiseTypes::STBN_19: return ReadNoiseTexture(/*$(Image2DArray:Textures/STBN_19/stbn_scalar_19_2Dx1Dx1D_128x128x32x1_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF);
		case NoiseTypes::FAST_Blue_Exp: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue_Exp/real_uniform_gauss1_0_exp0101_separate05_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF);
		case NoiseTypes::FAST_Binomial3x3_Exp: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Binomial3x3_Exp/real_uniform_binomial3x3_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF);
		case NoiseTypes::FAST_Box3x3_Exp: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Box3x3_Exp/real_uniform_box3x3_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF);
		case NoiseTypes::R2:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));
			return R2LDG(px.xy + int2((indexOffsetF + frameOffsetF) * 512.0f));
		}
		case NoiseTypes::IGN:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));

			return IGNLDG(int2(px.xy + int2((indexOffsetF + frameOffsetF) * 512.0f)));
		}
		case NoiseTypes::Bayer:
		{
			 float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));
			return Bayer(px.x + int(frameOffsetF.x * 4.0f), px.y + int(frameOffsetF.y * 4.0f), 4);
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

float3 GetRng3(uint3 px, int noiseType, inout uint wangStatePixel, inout uint wangStateGlobal)
{
	float3 ret;
	ret.x = GetRng(px, noiseType, wangStatePixel, wangStateGlobal, 0);
	ret.y = GetRng(px, noiseType, wangStatePixel, wangStateGlobal, 1);
	ret.z = GetRng(px, noiseType, wangStatePixel, wangStateGlobal, 2);
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
	bool animate = true;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
		{
			noiseType = /*$(Variable:NoiseType1)*/;
			animate = /*$(Variable:Animate1)*/;
		}
		else
		{
			noiseType = /*$(Variable:NoiseType3)*/;
			animate = /*$(Variable:Animate3)*/;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			noiseType = /*$(Variable:NoiseType2)*/;
			animate = /*$(Variable:Animate2)*/;
		}
		else
		{
			noiseType = /*$(Variable:NoiseType4)*/;
			animate = /*$(Variable:Animate4)*/;
		}
	}

	if (!animate)
		px.z = 0;

	// Initialize wang hash
	uint wangStatePixel = wang_hash_init(px);
	uint wangStateGlobal = wang_hash_init(uint3(1337, 435, px.z));

	// Dither
	float3 rng = GetRng3(px, noiseType, wangStatePixel, wangStateGlobal);
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
  * https://tellusim.com/improved-blue-noise/
  * https://acko.net/blog/stable-fiddusion/
 

* Finish bayer
* Need to dive deeper into Bayer. Can you use it like you are, or do you need different logic? also verify the function works correctly. Can include that in the blog post.
 * could also try with a bayer texture.
* do rectangular bayer - https://bisqwit.iki.fi/story/howto/dither/jy/.
 * for N bits, you can make that a rectangle. Make it as square as possible, then make the bayer matrix of that size.
 
* code generate C++ dx12 too.

* add blue 2d with offsets. so people can see it's the same as flipbook.


BLOG NOTES:

* Title "Analyzing Animated Dithering Techniques"?

* Threshold test as a second blog post!  Maybe investigate it before writing post?

* talk about how we offset the texture to get the 3 values

* R2 and Bayer don't have a natural way to animate them over time, so each frame has a different white noise offset

* do everything with flags_256 to make 512x512 images

* compare round and floor vs something nicely dithered (could maybe even be white noise?)
* show difference between STBN 1.0 and 1.9
* show difference between blue2d and a temporal blue noise. if you gauss blur, it's less compelling than if you don't!
* show what neighborhood clamping does to results (in different noise types)

* feature box noise too

* compare blue to binomial?

* Link to STBN and FAST repos.
 * also the competitive blue noise
  * https://tellusim.com/improved-blue-noise/
  * https://acko.net/blog/stable-fiddusion/

* note that doing a random offset each frame is ~ the same as doing a flip book of 2d blue noise.
* show animating blue noise with golden ratio. flickering in both taa and non taa. can link to that other blog post on animating blue noise.

! Fast BLUE Exp is the best blue noise.

A progression of quality. 1 bit per color channel. 3 bits per pixel.
round, white, blue, TAA blue

* link to blog post about LDS shuffler for offsets

* link to ordered dithering wikipedia :P
* and this https://bisqwit.iki.fi/story/howto/dither/jy/
 * Bayer is not fully Bayer, but it's close. I adapted to shader code.
* 16x16 (4 bits x 4 bits) is the largest bayer matrix you need, it's for for 8 bit color and has 256 different values

*/