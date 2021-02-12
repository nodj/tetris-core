// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#include "tc/TetrisHost.h"

#include "tc/modes/GameModeStandard.h"


namespace tc
{

TetrisHost::TetrisHost()
	: game(std::make_unique<NullGameMode>())
{
}

std::shared_ptr<StandardGameMode> TetrisHost::SetupStandardMode(i32 Width, i32 Height)
{
	auto tmp = std::make_shared<StandardGameMode>(Width, Height);
	game = tmp;
	return tmp;
}

} // ns tc
