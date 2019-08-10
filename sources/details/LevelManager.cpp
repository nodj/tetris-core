#include "LevelManager.h"
#include "types.h"

#include <array>

namespace tc
{

LevelManager::LevelManager(LevelUpPolicy Mode/*=Fixed*/, u32 StartLevel/*=0*/) : Mode(Mode)
	, StartLevel(StartLevel)
	, CurrentLevel(0)
	, ClearedLineCount(0)
// 	, GiftedLineCount(0)
	, Score(0)
	, Capped(false)
{
	Reset();
}

void LevelManager::Reset()
{
	SetCurrentLevel(StartLevel);
	ClearedLineCount = 0;
	Score = 0;
}

void LevelManager::RegisterClearedLines(u32 LineCount)
{
	ClearedLineCount += LineCount;

	// compute the new level
	if (!Capped)
	{
		switch (Mode)
		{
			case VariableWithBonus:
				while (!Capped && ClearedLineCount / 5 >= (CurrentLevel+1) * (CurrentLevel+2) / 2)
					SetCurrentLevel(CurrentLevel + 1);
				break;

			case Fixed:
				SetCurrentLevel(ClearedLineCount / 10);
				break;
		}
	}
}

u32 LevelManager::GetCurrentFallSpeed()
{
	// in ms per cell, TDG.7
	static std::array<u32, 15> DefaultFallSpeed = {1000, 793, 618, 473, 355, 262, 190, 135, 94, 64, 43, 28, 18, 11, 7};
	return DefaultFallSpeed[ Clamp<u32>(CurrentLevel, 0, 14)];
}

void LevelManager::SetCurrentLevel(u32 Level)
{
	CurrentLevel = Clamp<u32>(Level, 0, 14);
	Capped = Level == 14;
}

} // ns tc
