#include "SRS.h"

#include "pieces.h"
#include "types.h"

namespace tc
{

// SuperRotationSystem: https://tetris.fandom.com/wiki/SRS
// Note: weird double {{ }} syntax is required for std::arrays initialization...
static const SRSTable SRSOffsets {{
	// J, L, S, T, Z Tetromino Wall Kick Data
	{{{ 0, 0}, {-1, 0}, {-1,-1}, { 0, 2}, {-1, 2}}}, // 3 -> 0
	{{{ 0, 0}, {-1, 0}, {-1, 1}, { 0,-2}, {-1,-2}}}, // 0 -> 1
	{{{ 0, 0}, { 1, 0}, { 1,-1}, { 0, 2}, { 1, 2}}}, // 1 -> 2
	{{{ 0, 0}, { 1, 0}, { 1, 1}, { 0,-2}, { 1,-2}}}, // 2 -> 3
	{{{ 0, 0}, { 1, 0}, { 1,-1}, { 0, 2}, { 1, 2}}}, // 1 -> 0
	{{{ 0, 0}, {-1, 0}, {-1, 1}, { 0,-2}, {-1,-2}}}, // 2 -> 1
	{{{ 0, 0}, {-1, 0}, {-1,-1}, { 0, 2}, {-1, 2}}}, // 3 -> 2
	{{{ 0, 0}, { 1, 0}, { 1, 1}, { 0,-2}, { 1,-2}}}, // 0 -> 3

	// I Tetromino Wall Kick Data
	{{{ 0, 0}, { 1, 0}, {-2, 0}, { 1,-2}, {-2, 1}}}, // 3 -> 0
	{{{ 0, 0}, {-2, 0}, { 1, 0}, {-2,-1}, { 1, 2}}}, // 0 -> 1
	{{{ 0, 0}, {-1, 0}, { 2, 0}, {-1, 2}, { 2,-1}}}, // 1 -> 2
	{{{ 0, 0}, { 2, 0}, {-1, 0}, { 2, 1}, {-1,-2}}}, // 2 -> 3
	{{{ 0, 0}, { 2, 0}, {-1, 0}, { 2, 1}, {-1,-2}}}, // 1 -> 0
	{{{ 0, 0}, { 1, 0}, {-2, 0}, { 1,-2}, {-2, 1}}}, // 2 -> 1
	{{{ 0, 0}, {-2, 0}, { 1, 0}, {-2,-1}, { 1, 2}}}, // 3 -> 2
	{{{ 0, 0}, {-1, 0}, { 2, 0}, {-1, 2}, { 2,-1}}}  // 0 -> 3
}};

const SRSOffsetLine& GetSRSOffsets(EPiece Nature, EOrient NewOrient, bool IsLeftRotation)
{
	bool bIsI = Nature == EPiece::Piece_I;
	const SRSOffsetLine& line = SRSOffsets[bIsI*8 + IsLeftRotation*4 + i32(NewOrient)];
	return line;
}


} // ns tc
