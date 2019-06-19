#pragma once
#include "GameMode.h"
#include "../details/Board.h"
#include <bitset>
#include "../details/InputTracker.h"


namespace tc
{

struct IState
{
	IState()
		: bCanFadeIn(0)
		, bCanTick(0)
		, bCanFadeOut(0)
	{}

	// return the identifier of this state
	virtual i32 Id() = 0;

	// called pre Tick* calls
	virtual i32 GetLogicTickRate() = 0;

	// called once on state init
	virtual void Enter() {}

	// loop before standard loop
	// return true to repeat this phase
	virtual bool TickFadeIn(i32 /*LogicTick*/) { return false; }

	// standard execution loop
	// return true to repeat this phase
	virtual bool Tick(i32 /*LogicTick*/) { return false; }

	// loop after standard loop
	// return true to repeat this phase
	virtual bool TickFadeOut(i32 /*LogicTick*/) { return false; }

	// called once on state exit
	virtual void Exit() {}

	u8 bCanFadeIn : 1;
	u8 bCanTick : 1;
	u8 bCanFadeOut : 1;
};



enum SubState
{
	Setup, // init
	Wait, // menu loop
	Play, // std loop
	End, // death anim
	_count,
};





class WaitState : public IState
{
public:
	WaitState(class StandardGameMode* Mode) : Mode(Mode) {}
	virtual i32 Id() override { return SubState::Wait; }
	virtual void Enter() override;
	virtual i32 GetLogicTickRate() override;
	virtual bool TickFadeIn(i32 LogicTick) override;
	virtual bool Tick(i32 LogicTick) override;
	virtual bool TickFadeOut(i32 LogicTick) override;
private:
	i32 CurrentFlashLine;
	class StandardGameMode* Mode;
};


class StandardGameMode : public GameMode
{
public:
	StandardGameMode(i32 Width=10, i32 Height=22);

	virtual void RegisterInput(EGameplayInput Input) override;
	virtual void InternalTick(i32 ms) override;

	const Board& GetBoard() const { return board; }

private:

// 	bool ConsumeSubTicks(int RequiredTicks);

	i32 ConsumeAvailableLogicTick(int SubtickPerLogicTick);


// 	StateTracker<State, State::_count> currentState;

// 	template<State Current> bool Tick();
	Board board;
	i32 AccumulatedSubTicks;
// 	i32 SubTickPerFrame;

	WaitState waitState;
	u8 subState;

	InputTracker Inputs;

	friend WaitState;
};

} // ns tc
