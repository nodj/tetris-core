// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#include "tc/details/pieces.h"
#include "tc/details/types.h"

#include <assert.h>
#include <bitset>

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
	{   // S
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

} // ns tc
