#pragma once

#include "tc/modes/GameMode.h"

#include "tc/details/Board.h"
#include "tc/details/InputTracker.h"
#include "tc/details/LevelManager.h"
#include "tc/details/StateTracker.h"

#include <memory>

namespace tc
{

class StandardGameMode : public GameMode
{
public:
	StandardGameMode(i32 Width=10, i32 Height=22);

	virtual void RegisterInput(EGameplayInput Input) override;

	const Board& GetBoard() const { return SharedBoard; }

	// Check if currently in game, with meaningful stats.
	bool IsInPlayMode() const;

	// API for stats during play mode. UB if IsInPlayMode returns false
	const BasicGameStats& GetBasicStats() const;

protected:
	virtual void InternalTick(i32 ms) override;

private:
	Board SharedBoard;
	InputTracker Inputs;

	StateTracker Tracker;

	std::unique_ptr<class PlayStateNode> PlayState;
	std::unique_ptr<class WaitStateNode> WaitState;
	std::unique_ptr<class PauseStateNode> PauseState;
};

} // ns tc
