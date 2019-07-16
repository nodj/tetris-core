#pragma once

#include "types.h"
#include <array>

namespace tc
{

enum EPiece : u8
{
	Piece_I = 0,
	Piece_L = 1,
	Piece_J = 2,
	Piece_S = 3,
	Piece_Z = 4,
	Piece_O = 5,
	Piece_T = 6,
	Piece_None = 7,
	Piece_Count = 7,
};

enum EOrient
{
	Orient_N,
	Orient_E,
	Orient_S,
	Orient_W,
};

struct Span
{
	Span(u16 pixels=0);
	u16 pixels;
};

const Span& GetSpan(EPiece p, EOrient o);

struct FourPixels
{
	FourPixels(const Span& span);
	std::array<u8, 4> x;
	std::array<u8, 4> y;
};

} // ns tc