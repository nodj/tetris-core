#pragma once

#include "types.h"

#include <array>
#include <assert.h>
#include <vector>
#include "pieces.h"


/* Coordinates start from the bottom left cell, at (0,0).
 * -> Highest addressable cell: (W-1, H-1)
 * */
class Board
{
public:
	struct Cell
	{
		EPiece nature:3; // what piece type it is from
		bool phantom:1; // modifier to handle phantom state
	};

    using Grid = std::vector<Cell>;

	Board(i32 Width, i32 Height);

	Cell& GetCell(i32 x, i32 y)
	{
		assert(x >= 0 && x < Width);
		assert(y >= 0 && y < Height);
		return MergedBlocks[LinearCoord(x, y)];
	}

private:
	constexpr i32 LinearCoord(i32 x, i32 y) { return x + y * Width; }

	i32 Width;
	i32 Height;
    Grid StaticBlocks;
    Grid DynamicBlocks;
    Grid MergedBlocks;
};
