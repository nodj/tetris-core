#pragma once
#include "GameMode.h"

#include "details/Board.h"
#include "details/InputTracker.h"
#include "details/LevelManager.h"
#include "details/PieceGenerator.h"
#include "details/pieces.h"
#include "details/StateTracker.h"

#include <algorithm>
#include <map>
#include <numeric>
#include <random>


namespace tc
{

enum SubState
{
	Setup, // init
	Wait, // menu loop
	Play, // std loop
	Pause, // blank screen
	End, // death anim
	_count,
};


class WaitStateNode : public IStateNode
{
public:
	WaitStateNode(Board& ModeBoard, InputTracker& ModeInputs);
	virtual StateNodeId Id() const override { return SubState::Wait; }
	virtual void Enter(IStateNode* PreviousNode) override;
	virtual i32 GetLogicTickRate() override;
	virtual bool Tick(i32 LogicTick) override;
	virtual bool TickFadeOut(i32 LogicTick) override;

private:
	Board& MainBoard;
	InputTracker& Inputs;

	i32 CurrentFlashLine;
	i32 CleanupColumn;
};


class PlayStateNode : public IStateNode
{
public:
	PlayStateNode(Board& ModeBoard, InputTracker& ModeInputs);
	virtual StateNodeId Id() const override { return SubState::Play; }
	virtual void Enter(IStateNode* PreviousNode) override;
	virtual i32 GetLogicTickRate() override;
	virtual bool Tick(i32 LogicTick) override;


	virtual OutcomeId Exit() override;

public:
	const LevelManager& GetLevelManager() const { return Level; }

private:
	Board& MainBoard;
	InputTracker& Inputs;

	u32 GravityTickBudget = 0;

	u32 AutoRepeatDelay = 170; // TDG.5.2: Approx 0.3s
	u32 AutoRepeatSpeed = 50;

	struct HorizontalInputTracker
	{
		i32 LastDirection = 0;
		u32 MoveTickBudget = 0;
		u32 RepeatCount = 0;
	};
	HorizontalInputTracker hzit;

	EPiece MovingBlockNature = Piece_None;
	EOrient MovingBlockOrient = Orient_N;
	i32 MovingBlockX = 0;
	i32 MovingBlockY = 0;

	std::vector<i32> CompletedLines;
	u32 CompleteLineAnimBudget = 0;
	PieceGenerator<> RPG;
	LevelManager Level;

	Board BackupState;
	OutcomeId CurrentOutcomeId;
};

class PauseStateNode : public IStateNode
{
public:
	PauseStateNode(Board& ModeBoard, InputTracker& ModeInputs);
	virtual StateNodeId Id() const override { return SubState::Pause; }

	virtual void Enter(IStateNode* PreviousNode) override;
	virtual i32 GetLogicTickRate() override;
	virtual bool Tick(i32 LogicTick) override;

private:
	Board& MainBoard;
	InputTracker& Inputs;
	i32 MsgOffsetX;
};

class StandardGameMode : public GameMode
{
public:
	static constexpr OutcomeId WhenEnded = 0;
	static constexpr OutcomeId WhenPaused = 1;

	StandardGameMode(i32 Width=10, i32 Height=22);

	virtual void RegisterInput(EGameplayInput Input) override;

	virtual void InternalTick(i32 ms) override;

	const Board& GetBoard() const { return SharedBoard; }

public:
	// Check if currently in game, with meaningful stats.
	bool IsInPlayMode() const;

	// API for stats during play mode. UB if IsInPlayMode returns false
	const BasicGameStats& GetBasicStats() const { return PlayState.GetLevelManager().GetCurrentStats(); }

private:
	Board SharedBoard;
	InputTracker Inputs;

	StateTracker Tracker;

	WaitStateNode WaitState;
	PlayStateNode PlayState;
	PauseStateNode PauseState;
};

} // ns tc
