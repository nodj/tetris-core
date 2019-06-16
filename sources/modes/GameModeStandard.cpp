#include "GameModeStandard.h"

namespace tc
{

// this is the sandbox with no meaning
template<> bool StandardGameMode::Tick<StandardGameMode::State::Wait>()
{
	auto& Ctx = WaitStateContext;
	i32 LogicTick = ConsumeAvailableLogicTick(100);
	if (LogicTick <= 0)
	{
		return false;
	}

	Ctx.CurrentFlashLine += LogicTick;
	Ctx.CurrentFlashLine %= board.GetHeight();

	if (currentState.IsFirstFrame())
	{
		board.Clear();
		Ctx.CurrentFlashLine = 0;
	}

	board.StartFrame();

	i32 w = board.GetWidth();
	for (i32 x=0; x < w; ++x)
	{
		board.At(x, Ctx.CurrentFlashLine).state = true;
	}

	return true;
}

template<> bool StandardGameMode::Tick<StandardGameMode::State::Play>()
{
	return false;
}

template<> bool StandardGameMode::Tick<StandardGameMode::State::End>()
{
	return false;
}

StandardGameMode::StandardGameMode(i32 Width, i32 Height)
	: board(Width, Height)
	, currentState(State::Setup)
	, AccumulatedSubTicks(0)
	, SubTickPerFrame(500)
{
	currentState.Allow(Setup, Wait);
	currentState.Allow(Wait, Play);
	currentState.Allow(Play, End);
	currentState.Allow(End, Setup);

	currentState.Set(Wait);
}

void StandardGameMode::RegisterInput(EGameplayInput /*Input*/)
{

}

void StandardGameMode::InternalTick(i32 ms)
{
	AccumulatedSubTicks += ms;

	bool FrameConsumed = false;
	do
	{
		switch (currentState.Get())
		{
			case Wait: FrameConsumed = Tick<State::Wait>(); break;
			case Play: FrameConsumed = Tick<State::Play>(); break;
			case End: FrameConsumed = Tick<State::End>(); break;
			default: assert(false);
		}

		if (FrameConsumed)
		{
			currentState.EndFrame();
		}
	}
	while(FrameConsumed);
}

bool StandardGameMode::ConsumeSubTicks(int RequiredTicks)
{

	if (RequiredTicks <= AccumulatedSubTicks)
	{
		AccumulatedSubTicks -= RequiredTicks;
		return true;
	}
	return false;
}

i32 StandardGameMode::ConsumeAvailableLogicTick(int SubtickPerLogicTick)
{
	i32 LogicTickCount = AccumulatedSubTicks / SubtickPerLogicTick;
	AccumulatedSubTicks -= LogicTickCount * SubtickPerLogicTick;
	return LogicTickCount;
}

} // ns tc
