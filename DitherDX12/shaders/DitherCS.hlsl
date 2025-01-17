// Unnamed technique, shader Threshold


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
    static const int FAST_Binomial3x3_Exp = 8;
    static const int FAST_Box3x3_Exp = 9;
    static const int Blue_Tellusim_128_128_64 = 10;
    static const int Blue_Stable_Fiddusion = 11;
    static const int R2 = 12;
    static const int IGN = 13;
    static const int Bayer = 14;
    static const int Round = 15;
    static const int Floor = 16;
};

struct SpatialFilters
{
    static const int None = 0;
    static const int Box = 1;
    static const int Gauss = 2;
};

struct Struct__DitherCB
{
    uint Animate1;
    uint Animate2;
    uint Animate3;
    uint Animate4;
    int BitsPerColorChannel;
    int FrameIndex;
    int NoiseType1;
    int NoiseType2;
    int NoiseType3;
    int NoiseType4;
    float2 _padding0;
};

SamplerState linearWrapSampler : register(s0);
Texture2D<float4> Input : register(t0);
RWTexture2D<float4> Output : register(u0);
Texture2D<float> _loadedTexture_0 : register(t1);
Texture2DArray<float> _loadedTexture_1 : register(t2);
Texture2DArray<float> _loadedTexture_2 : register(t3);
Texture2DArray<float> _loadedTexture_3 : register(t4);
Texture2DArray<float> _loadedTexture_4 : register(t5);
Texture2DArray<float> _loadedTexture_5 : register(t6);
Texture2DArray<float> _loadedTexture_6 : register(t7);
Texture2DArray<float> _loadedTexture_7 : register(t8);
Texture2D<float> _loadedTexture_8 : register(t9);
Texture2D<float> _loadedTexture_9 : register(t10);
ConstantBuffer<Struct__DitherCB> _DitherCB : register(b0);

#line 2


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
		case NoiseTypes::Blue2D_Offset:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));
			return ReadNoiseTexture(_loadedTexture_0, px.xy, indexOffsetF + frameOffsetF);
		}
		case NoiseTypes::Blue2D_Flipbook: return ReadNoiseTexture(_loadedTexture_1, px, indexOffsetF);
		case NoiseTypes::Blue2D_Golden_Ratio:
		{
			float value = ReadNoiseTexture(_loadedTexture_0, px.xy, indexOffsetF);
			int frameIndex = px.z % 64;
			value = frac(value + frameIndex * c_goldenRatioConjugate);
			return value;
		}
		case NoiseTypes::STBN_10: return ReadNoiseTexture(_loadedTexture_2, px, indexOffsetF);
		case NoiseTypes::STBN_19: return ReadNoiseTexture(_loadedTexture_3, px, indexOffsetF);
		case NoiseTypes::FAST_Blue_Exp_Separate: return ReadNoiseTexture(_loadedTexture_4, px, indexOffsetF);
		case NoiseTypes::FAST_Blue_Exp_Product: return ReadNoiseTexture(_loadedTexture_5, px, indexOffsetF);
		case NoiseTypes::FAST_Binomial3x3_Exp: return ReadNoiseTexture(_loadedTexture_6, px, indexOffsetF);
		case NoiseTypes::FAST_Box3x3_Exp: return ReadNoiseTexture(_loadedTexture_7, px, indexOffsetF);
		case NoiseTypes::Blue_Tellusim_128_128_64:
		{
			// This texture is an 8x8 grid of tiles that are each 128x128.
			// The time axis goes left to right, then top to bottom.
			uint z = px.z % 64;
			uint2 xy = (px.xy + uint2(indexOffsetF*128.0f)) % uint2(128, 128);

			uint2 tilexy = uint2 (z % 8, z / 8);

			uint2 readpx = (tilexy * 128) + xy;

			return ReadNoiseTexture(_loadedTexture_8, readpx, float2(0.0f, 0.0f));
		}
		case NoiseTypes::Blue_Stable_Fiddusion:
		{
			// The texture has 16 tiles vertically, each 64x64
			uint z = px.z % 16;
			uint2 xy = (px.xy + uint2(indexOffsetF*128.0f)) % uint2(64, 64);
			xy.y += z * 64;
			return ReadNoiseTexture(_loadedTexture_9, xy, float2(0.0f, 0.0f));
		}
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

			int bitsX = _DitherCB.BitsPerColorChannel / 2;
			int bitsY = _DitherCB.BitsPerColorChannel - bitsX;

			return Bayer(px.x + int(frameOffsetF.x * 4.0f), px.y + int(frameOffsetF.y * 4.0f), bitsX, bitsY);
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

[numthreads(8, 8, 1)]
#line 127
void csmain(uint3 DTid : SV_DispatchThreadID)
{
	// Get the color of the pixel
	uint2 dims;
	Input.GetDimensions(dims.x, dims.y);
	uint3 px = uint3(DTid.xy, _DitherCB.FrameIndex);
	float4 color = Input[px.xy % dims];

	// Get the parameters for whatever quadrant we are in
	int noiseType = 0;
	bool animate = true;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
		{
			noiseType = _DitherCB.NoiseType1;
			animate = _DitherCB.Animate1;
		}
		else
		{
			noiseType = _DitherCB.NoiseType3;
			animate = _DitherCB.Animate3;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			noiseType = _DitherCB.NoiseType2;
			animate = _DitherCB.Animate2;
		}
		else
		{
			noiseType = _DitherCB.NoiseType4;
			animate = _DitherCB.Animate4;
		}
	}

	if (!animate)
		px.z = 0;

	// Initialize wang hash
	uint wangStatePixel = wang_hash_init(px);
	uint wangStateGlobal = wang_hash_init(uint3(1337, 435, px.z));

	// Dither
	float3 rng = GetRng3(px, noiseType, wangStatePixel, wangStateGlobal);
	color.rgb = ConvertToNBits(color.rgb, _DitherCB.BitsPerColorChannel, rng);

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

* maybe move this into a "Dither" subfolder, and make a new one for "Threshold"

* code generate C++ dx12 too.

Theshold TODO:
* have a slider that is a threshold level
* also optional to multiply it by a threshold map

BLOG NOTES:

* Title "Analyzing Animated Dithering Techniques"?

title image: Evolution of dithering
* Round -> white -> bayer -> blue -> STBN (filtered space / time) -> FAST product (filtered space / time)
* show 2 bits per color channel (64 colors), but show what happens when it drops to 1 bit (8 colors). temporal blue noise still looks pretty great.

* Threshold test as a second blog post!  Maybe investigate it before writing post?

* talk about how we offset the texture to get the 3 values for each noise type

* R2 and Bayer don't have a natural way to animate them over time, so each frame has a different white noise offset (others don't either)
* describe each noise type

* do everything with flags_256 to make 512x512 images for the post?

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
 * explain how we divide the number of color bits by 2 to get the x bits for bayer, and the remainder are y bits.

* show separate vs compiled FAST noise results.

* talk about how bayer does pixel swapping to optimize, and so does FAST noise. What is the next step to optimize for?

*/