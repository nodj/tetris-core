#include "pieces.h"
#include "types.h"
#include <bitset>
#include <assert.h>

namespace tc
{

Span::Span(u16 pixels/*=0*/)
	: pixels(pixels)
{
	assert(std::bitset<16>(pixels).count() == 4);
}

const static std::array< std::array<Span, 4>, 7> allSpans = { {
	{   // I
		Span{0b0000'1111'0000'0000},
		Span{0b0010'0010'0010'0010},
		Span{0b0000'0000'1111'0000},
		Span{0b0100'0100'0100'0100},
	},
	{   // L
		Span{0b0010'1110'0000'0000},
		Span{0b0100'0100'0110'0000},
		Span{0b0000'1110'1000'0000},
		Span{0b1100'0100'0100'0000},
	},
	{   // J
		Span{0b1000'1110'0000'0000},
		Span{0b0110'0100'0100'0000},
		Span{0b0000'1110'0010'0000},
		Span{0b0100'0100'1100'0000},
	},
	{   // N
		Span{0b0110'1100'0000'0000},
		Span{0b0100'0110'0010'0000},
		Span{0b0000'0110'1100'0000},
		Span{0b1000'1100'0100'0000},
	},
	{   // Z
		Span{0b1100'0110'0000'0000},
		Span{0b0010'0110'0100'0000},
		Span{0b0000'1100'0110'0000},
		Span{0b0100'1100'1000'0000},
	},
	{   // O
		Span{0b0110'0110'0000'0000},
		Span{0b0110'0110'0000'0000},
		Span{0b0110'0110'0000'0000},
		Span{0b0110'0110'0000'0000},
	},
	{   // T
		Span{0b0100'1110'0000'0000},
		Span{0b0100'0110'0100'0000},
		Span{0b0000'1110'0100'0000},
		Span{0b0100'1100'0100'0000},
	},
}};

const Span& GetSpan(EPiece p, EOrient o)
{
	return allSpans[p][o];
}

FourPixels::FourPixels(const Span& span)
{
	i32 pxIndex = 3;
	for (i32 i = 0; i < 16; ++i)
	{
		if (span.pixels & (1<<i))
		{
			x[pxIndex] = u8((15-i)%4);
			y[pxIndex] = u8((15-i)/4);
			pxIndex--;
		}
	}
}

} // ns tc