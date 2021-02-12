// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#pragma once

#include "tc/details/types.h"


// An implementation of Chapter 6, 7 & 8 of the Design Guideline (TDG)
namespace tc
{

enum class LevelUpPolicy
{
	// 10 lines to lvl-up
	Fixed,

	// (5 * level) lines to level-up (eg. 1->2 after 5 lines)
	// Also, The bonus system speed up the lvl-up process
	VariableWithBonus,
};

struct BasicGameStats
{
	u32 Level = 0; // 0-based Level
	u32 Lines = 0; // cleared line count
	u32 Score = 0; // points
};

// Note on implementation:
//   Internally, levels are handled 0 based (eg. in 0..14)
class LevelManager
{

public:
	LevelManager(LevelUpPolicy Mode=LevelUpPolicy::Fixed, u32 StartLevel=0);

	void Reset();

	// Declare an additional score of 1,2,3 or 4 lines
	void RegisterScore(u32 LineCount, bool bIsTSpin, bool bIsMiniTSpin);

	// Declare soft or hard drop for additional points
	void RegisterDrop(u32 DroppedCellCount, bool bHard);

	u32 GetCurrentFallSpeed() const { return FallSpeedDelay_ms; }

	const BasicGameStats& GetCurrentStats() const { return Stats; }

private:
	void SetCurrentLevel(u32 Level);

	LevelUpPolicy Mode;
	u32 StartLevel; // 0 based level (eg. in 0..14)
	BasicGameStats Stats;
	bool MaxedOut;
	u32 FallSpeedDelay_ms = 0;
};

} // ns tc
