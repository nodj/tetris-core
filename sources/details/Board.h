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

	const Cell& At(i32 x, i32 y) const { return *const_cast<Board*>(this)->AtInternal(x, y); }
	Cell& At(i32 x, i32 y) { return *AtInternal(x, y); }

	void Clear();
	void Fill(Cell Value);

	void Consolidate();

	void ResetToConsolidated();

private:
	Cell* AtInternal(i32 x, i32 y);

	constexpr i32 LinearCoord(i32 x, i32 y) const { return x + y * Width; }

	i32 Width;
	i32 Height;
    Grid StaticBlocks;
    Grid MergedBlocks;

	// fake cells, used to avoid tons of ifs
	Cell VirtualOffBoard;
	Cell VirtualNorth;
};

} // ns tc
