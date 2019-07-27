#include "LevelManager.h"
#include "types.h"

#include <array>

namespace tc
{

u32 LevelManager::GetCurrentFallSpeed()
{
	// in ms per cell, TDG.7
	static std::array<u32, 15> DefaultFallSpeed = {1000, 793, 618, 473, 355, 262, 190, 135, 94, 64, 43, 28, 18, 11, 7};
	return DefaultFallSpeed[CurrentLevel];
}

} // ns tc
