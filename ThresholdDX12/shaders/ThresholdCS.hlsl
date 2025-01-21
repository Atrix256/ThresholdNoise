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
    static const int FAST_Triangle_Blue_Exp_Separate = 8;
    static const int FAST_Triangle_Blue_Exp_Product = 9;
    static const int FAST_Binomial3x3_Exp = 10;
    static const int FAST_Box3x3_Exp = 11;
    static const int Blue_Tellusim_128_128_64 = 12;
    static const int Blue_Stable_Fiddusion = 13;
    static const int R2 = 14;
    static const int IGN = 15;
    static const int Bayer4 = 16;
    static const int Bayer16 = 17;
    static const int Bayer64 = 18;
    static const int Bayer256 = 19;
    static const int Round = 20;
    static const int Floor = 21;
};

struct SpatialFilters
{
    static const int None = 0;
    static const int Box = 1;
    static const int Gauss = 2;
};

struct TemporalFilters
{
    static const int None = 0;
    static const int Ema = 1;
    static const int EMA_plus_Clamp = 2;
    static const int Monte_Carlo = 3;
};

struct Struct__ThresholdCB
{
    uint Animate1;
    uint Animate2;
    uint Animate3;
    uint Animate4;
    uint ExtendNoise1;
    uint ExtendNoise2;
    uint ExtendNoise3;
    uint ExtendNoise4;
    int FrameIndex;
    int NoiseType1;
    int NoiseType2;
    int NoiseType3;
    int NoiseType4;
    float Threshold;
    float2 _padding0;
};

SamplerState linearWrapSampler : register(s0);
Texture2D<float4> Input : register(t0);
RWTexture2D<float4> Output : register(u0);
Texture2D<float4> ThresholdMap : register(t1);
Texture2D<float> _loadedTexture_0 : register(t2);
Texture2DArray<float> _loadedTexture_1 : register(t3);
Texture2DArray<float> _loadedTexture_2 : register(t4);
Texture2DArray<float> _loadedTexture_3 : register(t5);
Texture2DArray<float> _loadedTexture_4 : register(t6);
Texture2DArray<float> _loadedTexture_5 : register(t7);
Texture2DArray<float> _loadedTexture_6 : register(t8);
Texture2DArray<float> _loadedTexture_7 : register(t9);
Texture2DArray<float> _loadedTexture_8 : register(t10);
Texture2DArray<float> _loadedTexture_9 : register(t11);
Texture2D<float> _loadedTexture_10 : register(t12);
Texture2D<float> _loadedTexture_11 : register(t13);
ConstantBuffer<Struct__ThresholdCB> _ThresholdCB : register(b0);

#line 2


#include "Shared.hlsli"
#include "LDSShuffler.hlsli"

float ReadNoiseTexture(Texture2DArray<float> tex, uint3 px, float2 offsetF, bool extendNoise)
{
	uint3 noiseDims;
	tex.GetDimensions(noiseDims.x, noiseDims.y, noiseDims.z);
	int3 readPos;
	readPos.xy = (px.xy + int2(offsetF * float2(noiseDims.xy)));
	readPos.z = px.z;

	if (extendNoise)
	{
		int cycleCount = px.z / noiseDims.z;
		if (noiseDims.x == 128)
		{
			uint shuffleIndex = LDSShuffle1D_GetValueAtIndex(cycleCount, 16384, 10127, 435);
			readPos.xy += Convert1DTo2D_Hilbert(shuffleIndex, 16384);
		}
		else if (noiseDims.x == 64)
		{
			uint shuffleIndex = LDSShuffle1D_GetValueAtIndex(cycleCount, 4096, 2531, 435);
			readPos.xy += Convert1DTo2D_Hilbert(shuffleIndex, 4096);
		}
	}

	return tex[readPos % noiseDims];
}

float ReadNoiseTexture(Texture2D<float> tex, uint2 px, float2 offsetF)
{
	uint2 noiseDims;
	tex.GetDimensions(noiseDims.x, noiseDims.y);
	int2 readPos = (px + int2(offsetF * float2(noiseDims.xy)));
	return tex[readPos % noiseDims];
}

float GetRng(uint3 px, int noiseType, bool extendNoise, inout uint wangStatePixel, inout uint wangStateGlobal)
{
	switch (noiseType)
	{
		case NoiseTypes::White: return wang_hash_float01(wangStatePixel);
		case NoiseTypes::Blue2D_Offset:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));
			return ReadNoiseTexture(_loadedTexture_0, px.xy, frameOffsetF);
		}
		case NoiseTypes::Blue2D_Flipbook: return ReadNoiseTexture(_loadedTexture_1, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::Blue2D_Golden_Ratio:
		{
			float value = ReadNoiseTexture(_loadedTexture_0, px.xy, float2(0.0f, 0.0f));
			int frameIndex = px.z % 64;
			value = frac(value + frameIndex * c_goldenRatioConjugate);
			return value;
		}
		case NoiseTypes::STBN_10: return ReadNoiseTexture(_loadedTexture_2, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::STBN_19: return ReadNoiseTexture(_loadedTexture_3, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::FAST_Blue_Exp_Separate: return ReadNoiseTexture(_loadedTexture_4, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::FAST_Blue_Exp_Product: return ReadNoiseTexture(_loadedTexture_5, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::FAST_Triangle_Blue_Exp_Separate: return ReadNoiseTexture(_loadedTexture_6, px, float2(0.0f, 0.0f), extendNoise) * 2.0f - 0.5f;
		case NoiseTypes::FAST_Triangle_Blue_Exp_Product: return ReadNoiseTexture(_loadedTexture_7, px, float2(0.0f, 0.0f), extendNoise) * 2.0f - 0.5f;
		case NoiseTypes::FAST_Binomial3x3_Exp: return ReadNoiseTexture(_loadedTexture_8, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::FAST_Box3x3_Exp: return ReadNoiseTexture(_loadedTexture_9, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::Blue_Tellusim_128_128_64:
		{
			// This texture is an 8x8 grid of tiles that are each 128x128.
			// The time axis goes left to right, then top to bottom.
			uint z = px.z % 64;
			uint2 xy = (px.xy) % uint2(128, 128);

			uint2 tilexy = uint2 (z % 8, z / 8);

			uint2 readpx = (tilexy * 128) + xy;

			return ReadNoiseTexture(_loadedTexture_10, readpx, float2(0.0f, 0.0f));
		}
		case NoiseTypes::Blue_Stable_Fiddusion:
		{
			// The texture has 16 tiles vertically, each 64x64
			uint z = px.z % 16;
			uint2 xy = (px.xy) % uint2(64, 64);
			xy.y += z * 64;
			return ReadNoiseTexture(_loadedTexture_11, xy, float2(0.0f, 0.0f));
		}
		case NoiseTypes::R2:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));
			return R2LDG(px.xy + int2((frameOffsetF) * 512.0f));
		}
		case NoiseTypes::IGN:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));

			return IGNLDG(int2(px.xy + int2((frameOffsetF) * 512.0f)));
		}
		case NoiseTypes::Bayer4:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));
			return Bayer(px.x + int(frameOffsetF.x * 2.0f), px.y + int(frameOffsetF.y * 2.0f), 1, 1);
		}
		case NoiseTypes::Bayer16:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));
			return Bayer(px.x + int(frameOffsetF.x * 4.0f), px.y + int(frameOffsetF.y * 4.0f), 2, 2);
		}
		case NoiseTypes::Bayer64:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));
			return Bayer(px.x + int(frameOffsetF.x * 8.0f), px.y + int(frameOffsetF.y * 8.0f), 3, 3);
		}
		case NoiseTypes::Bayer256:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));
			return Bayer(px.x + int(frameOffsetF.x * 16.0f), px.y + int(frameOffsetF.y * 16.0f), 4, 4);
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

[numthreads(8, 8, 1)]
#line 131
void csmain(uint3 DTid : SV_DispatchThreadID)
{
	// Get the color of the pixel
	uint2 dims;
	Input.GetDimensions(dims.x, dims.y);
	uint3 px = uint3(DTid.xy, _ThresholdCB.FrameIndex);
	float4 color = Input[px.xy % dims];

	// Get the parameters for whatever quadrant we are in
	int noiseType = 0;
	bool animate = true;
	bool extendNoise = false;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
		{
			noiseType = _ThresholdCB.NoiseType1;
			animate = _ThresholdCB.Animate1;
			extendNoise = _ThresholdCB.ExtendNoise1;
		}
		else
		{
			noiseType = _ThresholdCB.NoiseType3;
			animate = _ThresholdCB.Animate3;
			extendNoise = _ThresholdCB.ExtendNoise3;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			noiseType = _ThresholdCB.NoiseType2;
			animate = _ThresholdCB.Animate2;
			extendNoise = _ThresholdCB.ExtendNoise2;
		}
		else
		{
			noiseType = _ThresholdCB.NoiseType4;
			animate = _ThresholdCB.Animate4;
			extendNoise = _ThresholdCB.ExtendNoise4;
		}
	}

	if (!animate)
		px.z = 0;

	// Initialize wang hash
	uint wangStatePixel = wang_hash_init(px);
	uint wangStateGlobal = wang_hash_init(uint3(1337, 435, px.z));

	// threshold
	float rng = GetRng(px, noiseType, extendNoise, wangStatePixel, wangStateGlobal);

	float threshold = _ThresholdCB.Threshold;

	float2 uv = (float2(px.xy % dims) + 0.5f) / float2(dims);
	threshold *= ThresholdMap.SampleLevel(linearWrapSampler, uv, 0).r;

	if (rng >= threshold && threshold < 1.0f)
		color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// Write out result
	color.rgb = LinearToSRGB(color.rgb);
	Output[px.xy] = color;
}

/*
Shader Resources:
	Texture Input (as SRV)
	Texture Output (as UAV)
	Texture ThresholdMap (as SRV)
*/


/*
TODO:

* make it so you can have a label for each square, and it centers it at the top. use it to label your diagrams. might need 2 labels per square. noise type and filtering? or maybe that is 2 different labels?
* Make sure c++ dx12 generated code is up to date for both
* check in proj and solution files, but with relative paths instead of absolute.





Threshold blog post notes:
* auto brightness to make up for pixels being black.
* dithering vs thresholding: dithering can reduce bit count of the colors. thresholding can eliminate having to do logic for certain colors.
* a nice demo: threshold down to 0.018. AKA 2% of the pixels rendered.  0.1 temporal filter. gauss 4.0 spatial filter. FAST looks pretty darn decent! looks so crazy filtered vs unfiltered. sort of less impressive when you look at the dots without auto brightness. still impressive though.
 * should do it that way. show dots without auto brightness and show filtered with auto brightness.
* also mention the C++ code to make bayer matrix images.
* adjusting brightness is like importance sampling (is it?), we are multiplying the color since fewer pixels remain.

Dither BLOG NOTES:

* Title "Analyzing Animated Dithering Techniques"?

title image: Evolution of dithering
* Round -> white -> bayer -> blue -> STBN (filtered space / time) -> FAST product (filtered space / time)
* show 2 bits per color channel (64 colors), but show what happens when it drops to 1 bit (8 colors). temporal blue noise still looks pretty great.

* triangular blue noise that is uniform on the edges.
 * i was thinking about using triangular blue noise, and mapping to uniform on the edges. Problem: tent not invertible!
 * so, i took uniform blue noise and remapped it to triangle away from the edges. Works decently. you can see the difference in the sky at 3 bits per color!
 * there may be a better way to do things, because the triangular remapping does diminish blue noise quality.

* subtractive dithering: show how there is no obvious banding. both with white noise and blue noise. does darken at low bit depths though (and triangular brightens). compare to triangular which also doesn't have banding?

* dithering link to integration?

* show extended vs not under monte carlo

* Threshold test as a second blog post!  Maybe investigate it before writing post?

* maybe mention that the random offset noise could be seen as part of the "noise extension" logic. It is just that Z is 1.

* talk about how we offset the texture to get the 3 values for each noise type

* bayer doesn't converge when randomly offset, weird!
 * why not?
 * i think it might be due to for instance 8x8 being 1/64th values, while blue noise has 1/256
 * yes, the quantization. Use quantized white noise to show it!

* R2 and Bayer don't have a natural way to animate them over time, so each frame has a different white noise offset (others don't either)
* describe each noise type

* do everything with flags_256 to make 512x512 images for the post?

* compare round and floor vs something nicely dithered (could maybe even be white noise?)
* show difference between STBN 1.0 and 1.9
* show difference between blue2d and a temporal blue noise. if you gauss blur, it's less compelling than if you don't!
* show what neighborhood clamping does to results (in different noise types)
 * white gets a way bigger impact to it than blue.__XB_AddI64 totally visible in temporally accumulated result.

* bayer doesn't converge. it's biased! kind of surprising.

* feature box noise too

* compare blue to binomial?

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

* mention a future post showing how to make a tiny gbuffer

* put link to post, link to gigi, and description in readme

? how to do "proper dithering" with blue noise? that is uniform on the edges, but triangular in the middle.
 * maybe we can convert the edges to uniform?

Link to:
* inside rendering: https://loopit.dk/rendering_inside.pdf
 * and more on shipping for ios: https://loopit.dk/inside_shipping_on_ios.pdf#page=99
* bart's post: https://bartwronski.com/2016/10/30/dithering-part-three-real-world-2d-quantization-dithering/
* srgb or not: http://www.thetenthplanet.de/archives/5367
* the thesis: https://dspacemainprd01.lib.uwaterloo.ca/server/api/core/bitstreams/022b1b01-5e4d-441a-bc53-ea13c65d1dd7/content
* https://tellusim.com/improved-blue-noise/
* https://acko.net/blog/stable-fiddusion/
* beyond white noise video
* STBN and FAST repos

* triangular dithering stuff:
 * https://www.shadertoy.com/view/4ssXRX
  * you added comments here: https://www.shadertoy.com/view/4t2SDh
 * this where it's uniform at the edes: https://computergraphics.stackexchange.com/a/5952/10515
  * "proper dithering" https://www.shadertoy.com/view/Wts3zH
 
*/