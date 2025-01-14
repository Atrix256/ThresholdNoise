// Unnamed technique, shader DitherSpatialFilter
/*$(ShaderResources)*/

#include "Shared.hlsli"

/*$(_compute:csmain)*/(uint3 DTid : SV_DispatchThreadID)
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
			spatialFilterType = /*$(Variable:SpatialFilter1)*/;
			spatialFilterParam = /*$(Variable:SpatialFilterParam1)*/;
		}
		else
		{
			spatialFilterType = /*$(Variable:SpatialFilter3)*/;
			spatialFilterParam = /*$(Variable:SpatialFilterParam3)*/;
		}
	}
	else
	{
		if (px.y <= dims.y)
		{
			spatialFilterType = /*$(Variable:SpatialFilter2)*/;
			spatialFilterParam = /*$(Variable:SpatialFilterParam2)*/;
		}
		else
		{
			spatialFilterType = /*$(Variable:SpatialFilter4)*/;
			spatialFilterParam = /*$(Variable:SpatialFilterParam4)*/;
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
