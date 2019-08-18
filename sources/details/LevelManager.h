#pragma once

#include "types.h"


// An implementation of Chapter 6 & 7 of the Design Guideline (TDG)
namespace tc
{

enum LevelUpPolicy
{
	// 10 lines to lvl-up
	Fixed,

	// (5 * level) lines to level-up (eg. 1->2 after 5 lines)
	// Also, The bonus system speed up the lvl-up process
	VariableWithBonus,
};

// Note on implementation:
//   Internally, levels are handled 0 based (eg. in 0..14)
class LevelManager
{
public:
	LevelManager(LevelUpPolicy Mode=Fixed, u32 StartLevel=0);

	void Reset();

	// Declare an additional score of 1,2,3 or 4 lines
	void RegisterClearedLines(u32 LineCount);

	u32 GetCurrentLevel() const { return CurrentLevel; }
	u32 GetCurrentFallSpeed() const;
	u32 GetClearedLineCount() const { return ClearedLineCount; }

private:
	void SetCurrentLevel(u32 Level);

	LevelUpPolicy Mode;
	u32 StartLevel; // 0 based level (eg. in 0..14)
	u32 CurrentLevel;
	u32 ClearedLineCount;
// 	u32 GiftedLineCount; // cool actions are rewarded with additionnal line count;
	u32 Score;
	bool Capped;
};

} // ns tc
