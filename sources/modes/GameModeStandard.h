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
	End, // death anim
	_count,
};


class WaitStateNode : public IStateNode
{
public:
	WaitStateNode(class StandardGameMode* Mode) : Mode(Mode) {}
	virtual u32 Id() const override { return SubState::Wait; }
	virtual void Enter() override;
	virtual i32 GetLogicTickRate() override;
	virtual bool Tick(i32 LogicTick) override;
	virtual bool TickFadeOut(i32 LogicTick) override;

private:
	class StandardGameMode* Mode;

	i32 CurrentFlashLine;
	i32 CleanupColumn;
};


class PlayStateNode : public IStateNode
{
public:
	PlayStateNode(class StandardGameMode* Mode);
	virtual u32 Id() const override { return SubState::Play; }
	virtual void Enter() override;
	virtual i32 GetLogicTickRate() override;
	virtual bool Tick(i32 LogicTick) override;

private:
	class StandardGameMode* Mode = nullptr;
	u32 GravityTickBudget;

	u32 AutoRepeatDelay;
	u32 AutoRepeatSpeed;

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
};


class StandardGameMode : public GameMode
{
public:
	StandardGameMode(i32 Width=10, i32 Height=22);

	virtual void RegisterInput(EGameplayInput Input) override;

	virtual void InternalTick(i32 ms) override;

	const Board& GetBoard() const { return board; }

private:
	Board board;

	InputTracker Inputs;

	StateTracker Tracker;

	WaitStateNode WaitState;
	friend WaitStateNode;

	PlayStateNode PlayState;
	friend PlayStateNode;
};

} // ns tc
