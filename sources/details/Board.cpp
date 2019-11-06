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

bool Board::IsLineComplete(i32 y) const
{
	if (y < 0 || y >= Height)
		return false;

	return std::all_of(
		StaticBlocks.begin() + LinearOffset(0, y),
		StaticBlocks.begin() + LinearOffset(0, y + 1),
		[](const Cell& c){ return c.state; }
	);
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

void Board::FillLine(i32 y, Cell Value)
{
	if (y >= 0 && y < Height)
		std::fill(std::begin(MergedBlocks) + y * Width, std::begin(MergedBlocks) + (y+1) * Width, Value);
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
	return &grid[LinearOffset(x, y)];
}

void Board::DeleteLines(std::vector<i32> CompletedLines)
{
	// clean indices
	auto BadIt = std::partition(std::begin(CompletedLines), std::end(CompletedLines), [=](i32 y){ return y >= 0 && y < Height; });
	CompletedLines.erase(BadIt, std::end(CompletedLines));
	std::sort(CompletedLines.begin(), CompletedLines.end(), std::greater<int>()); // reverse sort

	// rotate full lines to top
	for (i32 y : CompletedLines)
	{
		std::rotate(
			std::begin(StaticBlocks)+LinearOffset(0, y),
			std::begin(StaticBlocks)+LinearOffset(0, y+1),
			std::end(StaticBlocks)
		);
	}

	// erase top
	Cell NullCell;
	std::fill(
		std::begin(StaticBlocks)+LinearOffset(0, Height - i32(CompletedLines.size())),
		std::end(StaticBlocks),
		NullCell
	);

	ResetToConsolidated();
}


} // ns tc
