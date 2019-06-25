#pragma once
#include "GameMode.h"

#include "../details/Board.h"
#include "../details/InputTracker.h"
#include "../details/StateTracker.h"

#include <map>


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
	PlayStateNode(class StandardGameMode* Mode) : Mode(Mode) {}
	virtual u32 Id() const override { return SubState::Play; }
	virtual void Enter() override;
	virtual i32 GetLogicTickRate() override;
	virtual bool Tick(i32 LogicTick) override;

private:
	class StandardGameMode* Mode;
	u32 LogicTickSum;
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
