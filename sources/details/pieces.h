#pragma once

#include "types.h"
#include <array>

namespace tc
{

enum EPiece
{
	Piece_None,
	Piece_I,
	Piece_L,
	Piece_J,
	Piece_N,
	Piece_Z,
	Piece_O,
	Piece_T,
	Piece_Count = Piece_T
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

} // ns tc