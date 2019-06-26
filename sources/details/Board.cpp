#include "Board.h"

namespace tc
{

Board::Board(i32 Width, i32 Height)
	: Width(Width)
	, Height(Height)
{
	i32 cellCount = Width * Height;
	MergedBlocks.resize(cellCount);
	Clear();
}

bool Board::Blit(const Span& span, i32 xOrigin, i32 yOrigin, Cell Value, BlockLayer TestLayer, BlockLayer BlitLayer)
{
	FourPixels fpx(span);

	std::array<i32, 4> xs;
	std::array<i32, 4> ys;

	for(i32 i = 0; i < 4; ++i)
	{
		xs[i] = xOrigin + fpx.x[i];
		ys[i] = yOrigin - fpx.y[i];
	}

	// Test part
	if (TestLayer != BlockLayer::None)
	{
		std::array<Cell*, 4> testCells;
		for(i32 i = 0; i < 4; ++i)
		{
			testCells[i] = AtInternal(xs[i], ys[i], TestLayer);
			if (testCells[i] == &VirtualOffBoard)
				return false;
			if (testCells[i]->state && testCells[i] != &VirtualNorth)
				return false;
		}
	}

	// Blit part
	if (BlitLayer != BlockLayer::None)
	{
		for(i32 i = 0; i < 4; ++i)
		{
			*AtInternal(xs[i], ys[i], BlitLayer) = Value;
		}
	}

	return true;
}

void Board::Clear()
{
	Fill(Cell{});
	Consolidate();
}

void Board::Fill(Cell Value)
{
	std::fill(std::begin(MergedBlocks), std::end(MergedBlocks), Value);
}

void Board::ResetToConsolidated()
{
	// reset to last consolidated state
	MergedBlocks = StaticBlocks;
}

void Board::Consolidate()
{
	// Set current state as the new reference
	StaticBlocks = MergedBlocks;
}

Cell* Board::AtInternal(i32 x, i32 y, BlockLayer Target)
{
	if (x < 0 || x >= Width || y < 0)
		return &VirtualOffBoard;
	if (y >= Height)
		return &VirtualNorth;
	Grid& grid = Target==BlockLayer::Static ? StaticBlocks : MergedBlocks;
	return &grid[LinearCoord(x, y)];
}

} // ns tc
