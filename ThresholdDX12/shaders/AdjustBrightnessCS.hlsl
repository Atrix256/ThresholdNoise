// Unnamed technique, shader AdjustBrightness


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
    static const int Bayer4 = 14;
    static const int Bayer16 = 15;
    static const int Bayer64 = 16;
    static const int Bayer256 = 17;
    static const int Round = 18;
    static const int Floor = 19;
};

struct SpatialFilters
{
    static const int None = 0;
    static const int Box = 1;
    static const int Gauss = 2;
};

struct Struct__AdjustBrightnessCB
{
    float BrightnessMultiplier;
    float3 _padding0;
};

RWTexture2D<float4> Color : register(u0);
ConstantBuffer<Struct__AdjustBrightnessCB> _AdjustBrightnessCB : register(b0);

#line 2


#include "Shared.hlsli"

[numthreads(8, 8, 1)]
#line 6
void csmain(uint3 DTid : SV_DispatchThreadID)
{
	uint2 px = DTid.xy;
	float3 color = SRGBToLinear(Color[px].rgb);
	color *= _AdjustBrightnessCB.BrightnessMultiplier;
	Color[px] = float4(LinearToSRGB(color.rgb), 1.0f);
}

/*
Shader Resources:
	Texture Color (as UAV)
*/
