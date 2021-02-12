
#pragma once

#include "tc/details/Board.h"
#include "tc/details/types.h"
#include "tc/modes/GameMode.h"

#include <cstdint>
#include <memory>

namespace tc
{

// https://tetris.fandom.com/wiki/Tetris_Guideline
class TetrisHost
{
public:
	TetrisHost();

	// setup a new game mode
	std::shared_ptr<class StandardGameMode> SetupStandardMode(i32 Width=10, i32 Height=22);
	std::shared_ptr<GameMode> GetCurrentMode() { return game; }

private:
	std::shared_ptr<GameMode> game;
};

} // ns tc
