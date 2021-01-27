#include "LevelManager.h"
#include "types.h"

#include <array>

namespace tc
{

LevelManager::LevelManager(LevelUpPolicy Mode, u32 StartLevel) : Mode(Mode)
	, StartLevel(StartLevel)
	, MaxedOut(false)
{
	Reset();
}

void LevelManager::Reset()
{
	SetCurrentLevel(StartLevel);
	Stats = BasicGameStats();
}

void LevelManager::RegisterScore(u32 LineCount, bool bIsTSpin, bool bIsMiniTSpin)
{
	if (LineCount == 0 && !bIsMiniTSpin && !bIsTSpin)
		return;
	Stats.Lines += LineCount;

	// update level (TDG.6)
	if (!MaxedOut)
	{
		switch (Mode)
		{
			case LevelUpPolicy::VariableWithBonus:
				while (!MaxedOut && Stats.Lines / 5 >= (Stats.Level+1) * (Stats.Level+2) / 2)
					SetCurrentLevel(Stats.Level + 1);
				break;

			case LevelUpPolicy::Fixed:
				SetCurrentLevel(Stats.Lines / 10);
				break;
		}
	}

	// Score (TDG.8)
	i32 ThisScore = 0;
	if (!bIsTSpin)
	{
		static std::array<i32, 5> m = {0, 1, 3, 5, 8};
		ThisScore = m[Clamp<u32>(LineCount, 0, (u32)m.size())];
		ThisScore += bIsMiniTSpin; // mini-TSpin adds 100xlevel
	}
	else
	{
		static std::array<i32, 4> m = {4, 8, 12, 16};
		ThisScore = m[Clamp<u32>(LineCount, 0, (u32)m.size())];
	}
	Stats.Score += ThisScore * 100 * (Stats.Level + 1);
}

void LevelManager::RegisterDrop(u32 DroppedCellCount, bool bHard)
{
	Stats.Score += DroppedCellCount * (bHard ? 2 : 1);
}

void LevelManager::SetCurrentLevel(u32 Level)
{
	static std::array<u32, 15> Delays = {1000, 793, 618, 473, 355, 262, 190, 135, 94, 64, 43, 28, 18, 11, 7};

	u32 MaxLevel = (u32)Delays.size();
	Stats.Level = Clamp<u32>(Level, 0, MaxLevel);
	MaxedOut = Level == 14;

	// Update fall speed (in ms per cell) TDG.7
	FallSpeedDelay_ms = Delays[Stats.Level];
}

} // ns tc
