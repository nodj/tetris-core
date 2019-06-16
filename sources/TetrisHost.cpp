#include "TetrisHost.h"

#include "modes/GameModeStandard.h"


namespace tc
{

TetrisHost::TetrisHost()
	: game(std::make_unique<NullGameMode>())
{
}

StandardGameMode& TetrisHost::SetupStandardMode(i32 Width, i32 Height)
{
	game = std::make_unique<StandardGameMode>(Width, Height);
	return *(StandardGameMode*)game.get();
}

} // ns tc
