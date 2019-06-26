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

Cell* Board::AtInternal(i32 x, i32 y)
{
	if (x < 0 || x >= Width || y < 0)
		return &VirtualOffBoard;
	if (y >= Height)
		return &VirtualNorth;
	return &MergedBlocks[LinearCoord(x, y)];
}

} // ns tc
