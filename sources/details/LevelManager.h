#pragma once

#include "types.h"


// An implementation of Chapter 6 & 7 of the Design Guideline (TDG)
namespace tc
{

// Note on implementation:
//   Internally, levels are handled 0 based (eg. in 0..14)
class LevelManager
{
public:
	enum LevelUpMode
	{
		// 10 lines to lvl-up
		Fixed,

		// (5 * level) lines to level-up (eg. 1->2 after 5 lines)
		// Also, The bonus system speed up the lvl-up process
		VariableWithBonus,
	};

	LevelManager(LevelUpMode Mode=Fixed, u32 StartLevel=0)
		: Mode(Mode)
		, StartLevel(StartLevel)
		, CurrentLevel(0)
		, ClearedLineCount(0)
	{
		Reset();
	}

	void Reset()
	{
		SetCurrentLevel(StartLevel);
		ClearedLineCount = 0;
	}

	// Declare an additional score of 1,2,3 or 4 lines
	void RegisterClearedLines(u32 LineCount)
	{
		ClearedLineCount += LineCount;
		switch (Mode)
		{
			case VariableWithBonus: // niy
			case Fixed:
				SetCurrentLevel(ClearedLineCount/10);
				break;
		}
	}

	u32 GetCurrentLevel() { return CurrentLevel; }
	u32 GetCurrentFallSpeed();

private:
	void SetCurrentLevel(u32 Level) { CurrentLevel = Clamp<u32>(Level, 0, 14); }

	LevelUpMode Mode;
	u32 StartLevel; // 0 based level (eg. in 0..14)
	u32 CurrentLevel;
	u32 ClearedLineCount;
};

} // ns tc
