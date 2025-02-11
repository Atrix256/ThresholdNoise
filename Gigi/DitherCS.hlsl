// Unnamed technique, shader Threshold
/*$(ShaderResources)*/

#include "Shared.hlsli"
#include "LDSShuffler.hlsli"

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

// AKA Subtractive Dithering
float ConvertFromNBits(float value, int bits, float ditherRnd)
{
	int multiplier = (int(1) << (bits)) - 1;
	return (value * multiplier - ditherRnd) / multiplier;
}

float3 ConvertFromNBits(float3 value, int bits, float3 ditherRnd)
{
	return float3(
		ConvertFromNBits(value.r, bits, ditherRnd.r),
		ConvertFromNBits(value.g, bits, ditherRnd.g),
		ConvertFromNBits(value.b, bits, ditherRnd.b)
	);
}

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

float GetRng(uint3 px, int noiseType, bool extendNoise, float color, inout uint wangStatePixel, inout uint wangStateGlobal, int index)
{
	float2 indexOffsetF = R2LDS(index);

	switch (noiseType)
	{
		case NoiseTypes::White: return wang_hash_float01(wangStatePixel);
		case NoiseTypes::White_Triangular:
		{
			return ReshapeUniformToTriangle(wang_hash_float01(wangStatePixel));
		}
		case NoiseTypes::White_Triangular_Plus:
		{
			return ReshapeUniformToTriangleNoEdges(color, 1u << uint(/*$(Variable:BitsPerColorChannel)*/), wang_hash_float01(wangStatePixel));
		}
		case NoiseTypes::White4:
	 	{
			float value = wang_hash_float01(wangStatePixel);
			value = clamp(floor(value * 4.0f), 0.0f, 3.0f) / 4.0f;
			return value;
		}
		case NoiseTypes::White4_Plus_Half:
	 	{
			float value = wang_hash_float01(wangStatePixel);
			value = (clamp(floor(value * 4.0f), 0.0f, 3.0f) + 0.5f) / 4.0f;
			return value;
		}
		case NoiseTypes::Blue2D_Offset:
		{
			float2 frameOffsetF = float2(wang_hash_float01(wangStateGlobal), wang_hash_float01(wangStateGlobal));
			return ReadNoiseTexture(/*$(Image2D:Textures/FAST_Blue2D/blue2d_0.png:R8_Unorm:float:false)*/, px.xy, indexOffsetF + frameOffsetF);
		}
		case NoiseTypes::Blue2D_Flipbook: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue2D/blue2d_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise);
		case NoiseTypes::Blue2D_Golden_Ratio:
		{
			float value = ReadNoiseTexture(/*$(Image2D:Textures/FAST_Blue2D/blue2d_0.png:R8_Unorm:float:false)*/, px.xy, indexOffsetF);
			int frameIndex = px.z % 64;
			value = frac(value + frameIndex * c_goldenRatioConjugate);
			return value;
		}
		case NoiseTypes::STBN_10: return ReadNoiseTexture(/*$(Image2DArray:Textures/STBN_10/stbn_scalar_10_2Dx1Dx1D_128x128x32x1_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise);
		case NoiseTypes::STBN_19: return ReadNoiseTexture(/*$(Image2DArray:Textures/STBN_19/stbn_scalar_19_2Dx1Dx1D_128x128x32x1_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise);
		case NoiseTypes::FAST_Blue_Exp_Separate: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue_Exp/real_uniform_gauss1_0_exp0101_separate05_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise);
		case NoiseTypes::FAST_Blue_Exp_Product: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue_Exp/real_uniform_gauss1_0_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise);
		case NoiseTypes::FAST_Triangle_Blue_Exp_Separate: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Tent_Blue_Exp/real_tent_gauss1_0_exp0101_separate05_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise) * 2.0f - 0.5f;
		case NoiseTypes::FAST_Triangle_Blue_Exp_Product: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Tent_Blue_Exp/real_tent_gauss1_0_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise) * 2.0f - 0.5f;
		case NoiseTypes::FAST_Binomial3x3_Exp: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Binomial3x3_Exp/real_uniform_binomial3x3_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise);
		case NoiseTypes::FAST_Box3x3_Exp: return ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Box3x3_Exp/real_uniform_box3x3_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise);
		case NoiseTypes::FAST_Blue_Exp_Separate_Triangle_Plus:
		{
			float rng = ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue_Exp/real_uniform_gauss1_0_exp0101_separate05_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise);
			return ReshapeUniformToTriangleNoEdges(color, 1u << uint(/*$(Variable:BitsPerColorChannel)*/), rng);
		}
		case NoiseTypes::FAST_Blue_Exp_Product_Triangle_Plus:
		{
			float rng = ReadNoiseTexture(/*$(Image2DArray:Textures/FAST_Blue_Exp/real_uniform_gauss1_0_exp0101_product_%i.png:R8_Unorm:float:false)*/, px, indexOffsetF, extendNoise);
			return ReshapeUniformToTriangleNoEdges(color, 1u << uint(/*$(Variable:BitsPerColorChannel)*/), rng);
		}
		case NoiseTypes::Blue_Tellusim_128_128_64:
		{
			// This texture is an 8x8 grid of tiles that are each 128x128.
			// The time axis goes left to right, then top to bottom.
			uint z = px.z % 64;
			uint2 xy = (px.xy + uint2(indexOffsetF*128.0f)) % uint2(128, 128);

			uint2 tilexy = uint2 (z % 8, z / 8);

			uint2 readpx = (tilexy * 128) + xy;

			return ReadNoiseTexture(/*$(Image2D:Textures/tellusim/128x128_l64_s16_resave.png:R8_Unorm:float:false)*/, readpx, float2(0.0f, 0.0f));
		}
		case NoiseTypes::Blue_Stable_Fiddusion:
		{
			// The texture has 16 tiles vertically, each 64x64
			uint z = px.z % 16;
			uint2 xy = (px.xy + uint2(indexOffsetF*128.0f)) % uint2(64, 64);
			xy.y += z * 64;
			return ReadNoiseTexture(/*$(Image2D:Textures/Stable_Fiddusion/64x64x16 - S Rolloff - T Cosine - 0.0353.png:R8_Unorm:float:false)*/, xy, float2(0.0f, 0.0f));
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
			uint2 frameOffset = uint2(wang_hash_uint(wangStateGlobal), wang_hash_uint(wangStateGlobal));

			int bitsX = /*$(Variable:BitsPerColorChannel)*/ / 2;
			int bitsY = /*$(Variable:BitsPerColorChannel)*/ - bitsX;

			return Bayer(px.x + frameOffset.x, px.y + frameOffset.y, bitsX, bitsY);
		}
		case NoiseTypes::Bayer_Plus_Half:
		{
			uint2 frameOffset = uint2(wang_hash_uint(wangStateGlobal), wang_hash_uint(wangStateGlobal));

			int bitsX = /*$(Variable:BitsPerColorChannel)*/ / 2;
			int bitsY = /*$(Variable:BitsPerColorChannel)*/ - bitsX;

			return Bayer(px.x + frameOffset.x, px.y + frameOffset.y, bitsX, bitsY, 0.5f);
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

float3 GetRng3(uint3 px, int noiseType, bool extendNoise, float3 color, uint wangStatePixel, inout uint wangStateGlobal)
{
	float3 ret;
	ret.x = GetRng(px, noiseType, extendNoise, color.r, wangStatePixel, wangStateGlobal, 0);
	ret.y = GetRng(px, noiseType, extendNoise, color.g, wangStatePixel, wangStateGlobal, 1);
	ret.z = GetRng(px, noiseType, extendNoise, color.b, wangStatePixel, wangStateGlobal, 2);
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
	bool extendNoise = false;
	bool subtractiveDither = false;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
		{
			noiseType = /*$(Variable:NoiseType1)*/;
			animate = /*$(Variable:Animate1)*/;
			extendNoise = /*$(Variable:ExtendNoise1)*/;
			subtractiveDither = /*$(Variable:SubtractiveDither1)*/;
		}
		else
		{
			noiseType = /*$(Variable:NoiseType3)*/;
			animate = /*$(Variable:Animate3)*/;
			extendNoise = /*$(Variable:ExtendNoise3)*/;
			subtractiveDither = /*$(Variable:SubtractiveDither3)*/;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			noiseType = /*$(Variable:NoiseType2)*/;
			animate = /*$(Variable:Animate2)*/;
			extendNoise = /*$(Variable:ExtendNoise2)*/;
			subtractiveDither = /*$(Variable:SubtractiveDither2)*/;
		}
		else
		{
			noiseType = /*$(Variable:NoiseType4)*/;
			animate = /*$(Variable:Animate4)*/;
			extendNoise = /*$(Variable:ExtendNoise4)*/;
			subtractiveDither = /*$(Variable:SubtractiveDither4)*/;
		}
	}

	if (!animate)
		px.z = 0;

	// Initialize wang hash
	uint wangStatePixel = wang_hash_init(px);
	uint wangStateGlobal = wang_hash_init(uint3(1337, 435, px.z));

	// Dither
	float3 rng = GetRng3(px, noiseType, extendNoise, color.rgb, wangStatePixel, wangStateGlobal);
	color.rgb = ConvertToNBits(color.rgb, /*$(Variable:BitsPerColorChannel)*/, rng);

	// Subtractive dither, if we should
	if(subtractiveDither)
		color.rgb = ConvertFromNBits(color.rgb, /*$(Variable:BitsPerColorChannel)*/, rng);

	// Write out result
	color.rgb = LinearToSRGB(color.rgb);
	Output[px.xy] = color;
}

/*
Shader Resources:
	Texture Input (as SRV)
	Texture Output (as UAV)
*/
