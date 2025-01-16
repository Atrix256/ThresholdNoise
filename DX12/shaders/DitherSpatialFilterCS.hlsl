// Unnamed technique, shader DitherSpatialFilter


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

struct Struct__DitherSpatialFilterCB
{
    int SpatialFilter1;
    int SpatialFilter2;
    int SpatialFilter3;
    int SpatialFilter4;
    float SpatialFilterParam1;
    float SpatialFilterParam2;
    float SpatialFilterParam3;
    float SpatialFilterParam4;
};

Texture2D<float4> Input : register(t0);
RWTexture2D<float4> Output : register(u0);
ConstantBuffer<Struct__DitherSpatialFilterCB> _DitherSpatialFilterCB : register(b0);

#line 2


#include "Shared.hlsli"

[numthreads(8, 8, 1)]
#line 6
void csmain(uint3 DTid : SV_DispatchThreadID)
{
	// Calculate the size of each quadrant
	uint2 dims;
	Input.GetDimensions(dims.x, dims.y);
	dims /= 2;

	uint2 px = DTid.xy;

	// Get the parameters for whatever quadrant we are in
	int spatialFilterType = 0;
	float spatialFilterParam = 0.0f;
	if (px.x <= dims.x)
	{
		if (px.y <= dims.y)
		{
			spatialFilterType = _DitherSpatialFilterCB.SpatialFilter1;
			spatialFilterParam = _DitherSpatialFilterCB.SpatialFilterParam1;
		}
		else
		{
			spatialFilterType = _DitherSpatialFilterCB.SpatialFilter3;
			spatialFilterParam = _DitherSpatialFilterCB.SpatialFilterParam3;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			spatialFilterType = _DitherSpatialFilterCB.SpatialFilter2;
			spatialFilterParam = _DitherSpatialFilterCB.SpatialFilterParam2;
		}
		else
		{
			spatialFilterType = _DitherSpatialFilterCB.SpatialFilter4;
			spatialFilterParam = _DitherSpatialFilterCB.SpatialFilterParam4;
		}
	}

	uint2 pxRelative = px % dims;
	uint2 pxOffset = px - pxRelative;

	switch(spatialFilterType)
	{
		case SpatialFilters::None:
		{
			Output[px] = LinearToSRGB(Input[px]);
			break;
		}
		case SpatialFilters::Box:
		{
			int radius = (int)spatialFilterParam;
			float4 sum = 0.0f;
			float weight = 0.0f;
			for (int iy = -radius; iy <= radius; ++iy)
			{
				for (int ix = -radius; ix <= radius; ++ix)
				{
					int2 pxReadRel = int2(pxRelative) + int2(ix, iy);
					int2 pxReadRelClamped = clamp(pxReadRel, int2(0,0), int2(dims) - 1);
					if (pxReadRel.x == pxReadRelClamped.x && pxReadRel.y == pxReadRelClamped.y)
					{
						sum += Input[pxOffset + pxReadRel];
						weight += 1.0f;
					}
				}
			}

			Output[px] = LinearToSRGB(sum / weight);
			break;
		}
		case SpatialFilters::Gauss:
		{
			static const float c_support = 0.995f;

			float sigma = spatialFilterParam;
			float4 sum = 0.0f;
			float weight = 0.0f;

			// calculate radius of our blur based on sigma and support percentage
			const int radius = int(ceil(sqrt(-2.0 * sigma * sigma * log(1.0 - c_support))));

			for (int iy = -radius; iy <= radius; ++iy)
			{
				float kernelY = IntegrateGaussian(iy, sigma);
				for (int ix = -radius; ix <= radius; ++ix)
				{

					int2 pxReadRel = int2(pxRelative) + int2(ix, iy);
					int2 pxReadRelClamped = clamp(pxReadRel, int2(0,0), int2(dims) - 1);
					if (pxReadRel.x == pxReadRelClamped.x && pxReadRel.y == pxReadRelClamped.y)
					{
						float kernelX = IntegrateGaussian(ix, sigma);
						float kernel = kernelX * kernelY;

						sum += Input[pxOffset + pxReadRel] * kernel;
						weight += kernel;
					}
				}
			}

			Output[px] = LinearToSRGB(sum / weight);
			break;
		}
	}
}

/*
Shader Resources:
	Texture Input (as SRV)
	Texture Output (as UAV)
*/
