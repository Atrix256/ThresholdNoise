
uint wang_hash_init(uint3 seed)
{
	return uint(seed.x * uint(1973) + seed.y * uint(9277) + seed.z * uint(26699)) | uint(1);
}

uint wang_hash_uint(inout uint seed)
{
	seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> 4);
	seed *= uint(0x27d4eb2d);
	seed = seed ^ (seed >> 15);
	return seed;
}

float wang_hash_float01(inout uint state)
{
	return float(wang_hash_uint(state) & 0x00FFFFFF) / float(0x01000000);
}

// R2 is from http://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences/
// R2 Low discrepancy sequence
float2 R2LDS(int index)
{
	static const float g  = 1.32471795724474602596f;
	static const float a1 = 1 / g;
	static const float a2 = 1 / (g * g);
	return float2(frac(float(index) * a1), frac(float(index) * a2));
}

// R2 Low discrepancy grid
float R2LDG(uint2 pos)
{
	static const float g  = 1.32471795724474602596f;
	static const float a1 = 1 / g;
	static const float a2 = 1 / (g * g);
	return frac(float(pos.x) * a1 + float(pos.y) * a2);
}

// From http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
float IGNLDG(int2 pixelPos)
{
	return (52.9829189f * ((0.06711056f * float(pixelPos.x) + 0.00583715f * float(pixelPos.y)) % 1)) % 1;
}

float3 LinearToSRGB(float3 linearCol)
{
	float3 sRGBLo = linearCol * 12.92;
	float3 sRGBHi = (pow(abs(linearCol), float3(1.0 / 2.4, 1.0 / 2.4, 1.0 / 2.4)) * 1.055) - 0.055;
	float3 sRGB;
	sRGB.r = linearCol.r <= 0.0031308 ? sRGBLo.r : sRGBHi.r;
	sRGB.g = linearCol.g <= 0.0031308 ? sRGBLo.g : sRGBHi.g;
	sRGB.b = linearCol.b <= 0.0031308 ? sRGBLo.b : sRGBHi.b;
	return sRGB;
}

float3 SRGBToLinear(in float3 sRGBCol)
{
	float3 linearRGBLo = sRGBCol / 12.92;
	float3 linearRGBHi = pow((sRGBCol + 0.055) / 1.055, float3(2.4, 2.4, 2.4));
	float3 linearRGB;
	linearRGB.r = sRGBCol.r <= 0.04045 ? linearRGBLo.r : linearRGBHi.r;
	linearRGB.g = sRGBCol.g <= 0.04045 ? linearRGBLo.g : linearRGBHi.g;
	linearRGB.b = sRGBCol.b <= 0.04045 ? linearRGBLo.b : linearRGBHi.b;
	return linearRGB;
}

float4 LinearToSRGB(float4 linearCol)
{
    float4 ret;
    ret.rgb = LinearToSRGB(linearCol.rgb);
    ret.a = linearCol.a;
    return ret;
}

float4 SRGBToLinear(float4 sRGBCol)
{
    float4 ret;
    ret.rgb = SRGBToLinear(sRGBCol.rgb);
    ret.a = sRGBCol.a;
    return ret;
}

// Gauss blur calculations adapted from http://demofox.org/gauss.html

float erf(float x)
{
  // save the sign of x
  float sign = (x >= 0) ? 1 : -1;
  x = abs(x);

  // constants
  static const float a1 =  0.254829592;
  static const float a2 = -0.284496736;
  static const float a3 =  1.421413741;
  static const float a4 = -1.453152027;
  static const float a5 =  1.061405429;
  static const float p  =  0.3275911;

  // A&S formula 7.1.26
  float t = 1.0/(1.0 + p*x);
  float y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);
  return sign * y; // erf(-x) = -erf(x);
}

float IntegrateGaussian(float x, float sigma)
{
  float p1 = erf((x-0.5)/sigma*sqrt(0.5));
  float p2 = erf((x+0.5)/sigma*sqrt(0.5));
  return (p2-p1)/2.0;
}

uint InterleaveBits(uint A, uint B)
{
    uint ret = 0;
    for (uint i = 0; i < 16; ++i)
    {
        ret |= (A % 2);
        ret = ret << 1;
        A = A >> 1;

        ret |= (B % 2);
        ret = ret << 1;
        B = B >> 1;
    }
    return ret;
}

// Bayer code adapted from https://en.wikipedia.org/wiki/Ordered_dithering#Threshold_map
// returns M_{ij} for an NxN Bayer Matrix
float Bayer(uint i, uint j, uint N)
{
    i = i % N;
    j = j % N;
    return float(reversebits(InterleaveBits(i ^ j, i))) / float(N*N);
}

static const float c_goldenRatioConjugate = 0.61803399f;