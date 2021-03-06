// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#pragma once

#include "tc/details/pieces.h"
#include "tc/details/types.h"

#include <array>
#include <vector>

namespace tc
{

struct Cell
{
	Cell(): state(false), nature(EPiece::Piece_None), phantom(false), locked(false) {}

	bool state:1; // on-off
	EPiece nature:3; // what piece type it is from
	bool phantom:1; // modifier to handle phantom state
	bool locked:1; // if the cell is locked or movable
	Cell AsPhantom() { Cell Phantom; Phantom.nature = nature; Phantom.phantom = true; return Phantom; }
};

/**
 * Coordinates start from the bottom left cell, at (0,0).
 * -> Highest addressable cell: (W-1, H-1)
 */
class Board
{
public:
    using Grid = std::vector<Cell>;

public:
	// Describe a block layer
	enum BlockLayer {None, Static, Merged};

	Board(i32 Width, i32 Height);

	i32 GetWidth() const { return Width; }
	i32 GetHeight() const { return Height; }

	const Cell& At(i32 x, i32 y) const { return *const_cast<Board*>(this)->AtInternal(x, y, BlockLayer::Merged); }
	Cell& At(i32 x, i32 y) { return *AtInternal(x, y, BlockLayer::Merged); }

	bool IsLineComplete(i32 y) const;

	bool Blit(const Span& span, i32 xOrigin, i32 yOrigin, Cell Value, BlockLayer TestLayer, BlockLayer BlitLayer);

	void Clear();
	void FillLine(i32 y, Cell Value);
	void Fill(Cell Value);
	void DeleteLines(std::vector<i32> CompletedLines);

	// Set current state as the new reference
	void Consolidate();

	// reset to last consolidated state
	void ResetToConsolidated();

private:
	constexpr i32 LinearOffset(i32 x, i32 y) const { return x + y * Width; }
	Cell* AtInternal(i32 x, i32 y, BlockLayer Target);

	i32 Width;
	i32 Height;
    Grid StaticBlocks;
    Grid MergedBlocks;

	// Virtual cells, used to avoid tons of ifs
	Cell VirtualOffBoard;
	Cell VirtualNorth;
};

} // ns tc
