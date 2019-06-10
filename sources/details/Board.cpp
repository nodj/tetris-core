#include "Board.h"

Board::Board(i32 Width, i32 Height) 
	: Width(Width)
	, Height(Height)
{
	Cell nullCell{ 0 };
	i32 cellCount = Width * Height;
	StaticBlocks.resize(cellCount, nullCell);
	DynamicBlocks.resize(cellCount, nullCell);
	MergedBlocks.resize(cellCount, nullCell);
}
