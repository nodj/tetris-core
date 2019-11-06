#pragma once

#include "GameMode.h"

#include "details/Board.h"
#include "details/InputTracker.h"
#include "details/LevelManager.h"
#include "details/StateTracker.h"

#include <memory>

namespace tc
{

class StandardGameMode : public GameMode
{
public:
	StandardGameMode(i32 Width=10, i32 Height=22);

	virtual void RegisterInput(EGameplayInput Input) override;

	virtual void InternalTick(i32 ms) override;

	const Board& GetBoard() const { return SharedBoard; }

public:
	// Check if currently in game, with meaningful stats.
	bool IsInPlayMode() const;

	// API for stats during play mode. UB if IsInPlayMode returns false
	const BasicGameStats& GetBasicStats() const;

private:
	Board SharedBoard;
	InputTracker Inputs;

	StateTracker Tracker;

	std::unique_ptr<class PlayStateNode> PlayState;
	std::unique_ptr<class WaitStateNode> WaitState;
	std::unique_ptr<class PauseStateNode> PauseState;
};

} // ns tc
