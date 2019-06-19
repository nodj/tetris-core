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

void Board::Clear()
{
	std::fill(std::begin(MergedBlocks), std::end(MergedBlocks), Cell{});
	Consolidate();
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

} // ns tc
