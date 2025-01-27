// Unnamed technique, shader Threshold
/*$(ShaderResources)*/

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
			return ReadNoiseTexture(/*$(Image2D:Textures/FAST_Blue2D/blue2d_0.png:R8_Unorm:float:false)*/, px.xy, frameOffsetF);
		}
		case NoiseTypes::Blue2D_Flipbook: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue2D/blue2d_%i.png:R8_Unorm:float:false)*/, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::Blue2D_Golden_Ratio:
		{
			float value = ReadNoiseTexture(/*$(Image2D:Textures/FAST_Blue2D/blue2d_0.png:R8_Unorm:float:false)*/, px.xy, float2(0.0f, 0.0f));
			int frameIndex = px.z % 64;
			value = frac(value + frameIndex * c_goldenRatioConjugate);
			return value;
		}
		case NoiseTypes::STBN_10: return ReadNoiseTexture(/*$(Image2DArray:Textures/STBN_10/stbn_scalar_10_2Dx1Dx1D_128x128x32x1_%i.png:R8_Unorm:float:false)*/, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::STBN_19: return ReadNoiseTexture(/*$(Image2DArray:Textures/STBN_19/stbn_scalar_19_2Dx1Dx1D_128x128x32x1_%i.png:R8_Unorm:float:false)*/, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::FAST_Blue_Exp_Separate: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue_Exp/real_uniform_gauss1_0_exp0101_separate05_%i.png:R8_Unorm:float:false)*/, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::FAST_Blue_Exp_Product: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue_Exp/real_uniform_gauss1_0_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::FAST_Triangle_Blue_Exp_Separate: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Tent_Blue_Exp/real_tent_gauss1_0_exp0101_separate05_%i.png:R8_Unorm:float:false)*/, px, float2(0.0f, 0.0f), extendNoise) * 2.0f - 0.5f;
		case NoiseTypes::FAST_Triangle_Blue_Exp_Product: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Tent_Blue_Exp/real_tent_gauss1_0_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, float2(0.0f, 0.0f), extendNoise) * 2.0f - 0.5f;
		case NoiseTypes::FAST_Binomial3x3_Exp: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Binomial3x3_Exp/real_uniform_binomial3x3_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::FAST_Box3x3_Exp: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Box3x3_Exp/real_uniform_box3x3_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, float2(0.0f, 0.0f), extendNoise);
		case NoiseTypes::Blue_Tellusim_128_128_64:
		{
			// This texture is an 8x8 grid of tiles that are each 128x128.
			// The time axis goes left to right, then top to bottom.
			uint z = px.z % 64;
			uint2 xy = (px.xy) % uint2(128, 128);

			uint2 tilexy = uint2 (z % 8, z / 8);

			uint2 readpx = (tilexy * 128) + xy;

			return ReadNoiseTexture(/*$(Image2D:Textures/tellusim/128x128_l64_s16_resave.png:R8_Unorm:float:false)*/, readpx, float2(0.0f, 0.0f));
		}
		case NoiseTypes::Blue_Stable_Fiddusion:
		{
			// The texture has 16 tiles vertically, each 64x64
			uint z = px.z % 16;
			uint2 xy = (px.xy) % uint2(64, 64);
			xy.y += z * 64;
			return ReadNoiseTexture(/*$(Image2D:Textures/Stable_Fiddusion/64x64x16 - S Rolloff - T Cosine - 0.0353.png:R8_Unorm:float:false)*/, xy, float2(0.0f, 0.0f));
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
	bool extendNoise = false;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
		{
			noiseType = /*$(Variable:NoiseType1)*/;
			animate = /*$(Variable:Animate1)*/;
			extendNoise = /*$(Variable:ExtendNoise1)*/;
		}
		else
		{
			noiseType = /*$(Variable:NoiseType3)*/;
			animate = /*$(Variable:Animate3)*/;
			extendNoise = /*$(Variable:ExtendNoise3)*/;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			noiseType = /*$(Variable:NoiseType2)*/;
			animate = /*$(Variable:Animate2)*/;
			extendNoise = /*$(Variable:ExtendNoise2)*/;
		}
		else
		{
			noiseType = /*$(Variable:NoiseType4)*/;
			animate = /*$(Variable:Animate4)*/;
			extendNoise = /*$(Variable:ExtendNoise4)*/;
		}
	}

	if (!animate)
		px.z = 0;

	// Initialize wang hash
	uint wangStatePixel = wang_hash_init(px);
	uint wangStateGlobal = wang_hash_init(uint3(1337, 435, px.z));

	// threshold
	float rng = GetRng(px, noiseType, extendNoise, wangStatePixel, wangStateGlobal);

	float threshold = /*$(Variable:Threshold)*/;

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
* make sure the C++ code builds ok from a different directory.
* When done with post, add the gigi techniques to the browser and mention it in the post!
* check in gguser files with labels of 4x4 solid color textures, and make the labels not render if the label size is <= 4x4
 * and 512x512 images
* register 2 gigi techniques - one for dithering, one for stippling.

Threshold blog post notes:
* auto brightness to make up for pixels being black.
* dithering vs thresholding: dithering can reduce bit count of the colors. thresholding can eliminate having to do logic for certain colors.
* a nice demo: threshold down to 0.018. AKA 2% of the pixels rendered.  0.1 temporal filter. gauss 4.0 spatial filter. FAST looks pretty darn decent! looks so crazy filtered vs unfiltered. sort of less impressive when you look at the dots without auto brightness. still impressive though.
 * should do it that way. show dots without auto brightness and show filtered with auto brightness.
* also mention the C++ code to make bayer matrix images.
* adjusting brightness is like importance sampling (is it?), we are multiplying the color since fewer pixels remain.

* link to this fractal temporally stable dithering thing: https://www.youtube.com/watch?v=HPqGaIMVuLs
 * would be nice if that was blue noise. and spatiotemporal blue noise.

* show abdalla's work in reconstruction in latest paper. our thresholded blue noise is not as good as his, but his isn't real time.

Dither BLOG NOTES:

* Title "Analyzing Animated Dithering Techniques"?

title image: Evolution of dithering
* Round -> white -> bayer -> blue -> STBN (filtered space / time) -> FAST product (filtered space / time)
* show 2 bits per color channel (64 colors), but show what happens when it drops to 1 bit (8 colors). temporal blue noise still looks pretty great.

* important to mention that we work in linear space, not sRGB. so need to convert from sRGB to linear, do the work, then convert from linear back to sRGB before writing it out.

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

* link to beyond white noise in rendering video.

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