
#pragma once

#include "stdint.h"
#include "details/Board.h"
#include "details/types.h"
#include "modes/GameMode.h"

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
