#pragma once

#include "types.h"

#include <array>
#include <assert.h>
#include <vector>
#include "pieces.h"

namespace tc
{

struct Cell
{
	Cell(): state(false), nature(EPiece::Piece_None), phantom(false) {}

	bool state:1; // on-off
	EPiece nature:3; // what piece type it is from
	bool phantom:1; // modifier to handle phantom state
};

/*
 * Coordinates start from the bottom left cell, at (0,0).
 * -> Highest addressable cell: (W-1, H-1)
 */
class Board
{
public:

    using Grid = std::vector<Cell>;

public:
	Board(i32 Width, i32 Height);

	i32 GetWidth() const { return Width; }
	i32 GetHeight() const { return Height; }

	const Cell& At(i32 x, i32 y) const { return MergedBlocks[LinearCoordChecked(x, y)]; }
	Cell& At(i32 x, i32 y) { return MergedBlocks[LinearCoordChecked(x, y)]; }

	void Clear();
	void Consolidate();

	void ResetToConsolidated();
	void EndFrame();

private:
	constexpr i32 LinearCoord(i32 x, i32 y) const { return x + y * Width; }
	i32 LinearCoordChecked(i32 x, i32 y) const {
		assert(x >= 0 && x < Width);
		assert(y >= 0 && y < Height);
		return LinearCoord(x, y);
	}

	i32 Width;
	i32 Height;
    Grid StaticBlocks;
    Grid MergedBlocks;
};

} // ns tc