#include "GameModeStandard.h"

namespace tc
{

StandardGameMode::StandardGameMode(i32 Width, i32 Height)
	: board(Width, Height)
	, AccumulatedSubTicks(0)
// 	, currentState(State::Setup)
	, waitState(this)
	, subState(1)
{
// 	currentState.Allow(Setup, Wait);
// 	currentState.Allow(Wait, Play);
// 	currentState.Allow(Play, End);
// 	currentState.Allow(End, Setup);
//
// 	currentState.Set(Wait);
}

void StandardGameMode::RegisterInput(EGameplayInput Input)
{
	Inputs.RegisterInput(Input);
}

void StandardGameMode::InternalTick(i32 ms)
{
	AccumulatedSubTicks += ms;

	// >>>>>>>>> state tracker job

	enum ESubState
	{
		ESS_Enter   = 1 << 0,
		ESS_FadeIn  = 1 << 1,
		ESS_Tick    = 1 << 2,
		ESS_FadeOut = 1 << 3,
		ESS_Exit    = 1 << 4,
		ESS_TickableMask = ESS_FadeIn | ESS_Tick | ESS_FadeOut
	};

	auto& s = waitState;
	u8 capa = ESS_Enter | s.bCanFadeIn*ESS_FadeIn | s.bCanTick*ESS_Tick | s.bCanFadeOut*ESS_FadeOut | ESS_Exit;
	while ((subState & capa) == 0)
		subState <<= 1;

	i32 LogicTick = 0;
	if (subState & ESS_TickableMask)
	{
		i32 TickRate = s.GetLogicTickRate();
		LogicTick = ConsumeAvailableLogicTick(TickRate);
		if (LogicTick == 0)
		{
			return; // can't use this frame (not enough time elapsed)
		}
	}

	switch(subState)
	{
		case ESS_Enter:
			s.Enter();
			subState <<= 1;
			break;
		case ESS_FadeIn:
			if (!s.TickFadeIn(LogicTick))
				subState <<= 1;
			break;
		case ESS_Tick:
			if (!s.Tick(LogicTick))
				subState <<= 1;
			break;
		case ESS_FadeOut:
			if (!s.TickFadeOut(LogicTick))
				subState <<= 1;
			break;
		case ESS_Exit:
			s.Exit();
			subState = ESS_Enter;
			break;
	}

	// <<<<<<<<< state tracker job
}

i32 StandardGameMode::ConsumeAvailableLogicTick(int SubtickPerLogicTick)
{
	i32 LogicTickCount = AccumulatedSubTicks / SubtickPerLogicTick;
	AccumulatedSubTicks -= LogicTickCount * SubtickPerLogicTick;
	Inputs.ConsumeLogicalTicks(LogicTickCount);
	return LogicTickCount;
}

void WaitState::Enter()
{
	// config capabilities
	bCanFadeIn = true;
	bCanTick = true;
	bCanFadeOut = true;

	// init internal state
	CurrentFlashLine = 0;
	Mode->board.Clear();
}

i32 WaitState::GetLogicTickRate()
{
	return 100; // #tetris_todo no magic numbers
}

bool WaitState::TickFadeIn(i32 LogicTick)
{
	Board& board = Mode->board;

	// advance logic
	CurrentFlashLine += LogicTick;
	CurrentFlashLine %= board.GetHeight();

	// draw on board
	board.ResetToConsolidated();

	i32 w = board.GetWidth();
	for (i32 x=0; x < w; ++x)
	{
		board.At(x, CurrentFlashLine).state = true;
	}

	// exit condition check
	if (Mode->Inputs.IsAnyActionDown())
	{
// 		Mode->currentState.Set()
	}
	return true; // looooooooooooooooop
}

bool WaitState::Tick(i32 LogicTick)
{
	return false;
}

bool WaitState::TickFadeOut(i32 LogicTick)
{
	return false;
}

} // ns tc
