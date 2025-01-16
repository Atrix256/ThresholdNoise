#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <direct.h>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

using uint = unsigned int;

template <typename T>
T Clamp(T value, T themin, T themax)
{
	if (value <= themin)
		return themin;
	else if (value >= themax)
		return themax;
	else
		return value;
}

// Portions of this code were based on https://bisqwit.iki.fi/story/howto/dither/jy/
uint Bayer(uint x, uint y, uint XBits, uint YBits)
{
	if (YBits == 0 || (XBits < YBits && XBits != 0))
	{
		uint temp = XBits;
		XBits = YBits;
		YBits = temp;

		temp = x;
		x = y;
		y = temp;
	}

	uint value = 0;
	uint xmask = XBits;
	uint ymask = YBits;
	uint xc = x ^ ((y << XBits) >> YBits);
	uint yc = y;

	uint offset = 0;
	for (uint bit = 0; bit < XBits + YBits; )
	{
		ymask--;
		value |= ((yc >> ymask) & 1) << bit;
		bit++;

		for (offset += XBits; offset >= YBits; offset -= YBits)
		{
			xmask--;
			value |= ((xc >> xmask) & 1) << bit;
			bit++;
		}
	}

	return value;
}

int main(int argc, char** argv)
{
	static const uint c_matrixBitsX = 4;
	static const uint c_matrixBitsY = 4;

	static const uint c_matrixWidth = 1 << c_matrixBitsX;
	static const uint c_matrixHeight = 1 << c_matrixBitsY;

	static const uint c_matrixEntries = c_matrixWidth * c_matrixHeight;

	std::vector<unsigned char> pixels(c_matrixEntries);
	unsigned char* pixel = pixels.data();

	for (uint iy = 0; iy < c_matrixWidth; ++iy)
	{
		printf("[ ");
		for (uint ix = 0; ix < c_matrixHeight; ++ix)
		{
			uint bayerU = Bayer(ix, iy, c_matrixBitsX, c_matrixBitsY);
			float bayerF = float(bayerU) / float(c_matrixEntries);
			printf("%0.2f ", bayerF);

			pixel[0] = (unsigned char)Clamp(bayerF * 256.0f, 0.0f, 255.0f);
			pixel++;
		}
		printf("]\n");
	}

	_mkdir("out");

	char fileName[1024];
	sprintf(fileName, "out/Bayer_%u_%u.png", c_matrixWidth, c_matrixHeight);
	stbi_write_png(fileName, c_matrixWidth, c_matrixHeight, 1, pixels.data(), 0);

	return 0;
}
