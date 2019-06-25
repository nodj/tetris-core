#include "GameModeStandard.h"

namespace tc
{

StandardGameMode::StandardGameMode(i32 Width, i32 Height)
	: board(Width, Height)
	, WaitState(this)
	, PlayState(this)
{
	Tracker.AddTickableSystem(&Inputs);

	Tracker.SetNextStateNode(&WaitState, &PlayState);
	Tracker.SetNextStateNode(&PlayState, &WaitState);
	Tracker.SetStartState(&WaitState);
}

void StandardGameMode::RegisterInput(EGameplayInput Input)
{
	Inputs.RegisterInput(Input);
}

void StandardGameMode::InternalTick(i32 ms)
{
	bool bShouldTickAgain = Tracker.Tick(ms);

	if (!bShouldTickAgain)
		SetOver();
}

void WaitStateNode::Enter()
{
	// config capabilities
	bCanFadeIn = false;
	bCanTick = true;
	bCanFadeOut = true;

	// init internal state
	CurrentFlashLine = 0;
	CleanupColumn = 0;
	Mode->board.Clear();
}

i32 WaitStateNode::GetLogicTickRate()
{
	return 100; // #tetris_todo no magic numbers
}

bool WaitStateNode::Tick(i32 LogicTick)
{
	Board& board = Mode->board;

	// advance logic
	CurrentFlashLine += LogicTick;
	CurrentFlashLine %= board.GetHeight();

	// draw on board
	board.ResetToConsolidated();

	i32 w = board.GetWidth();
	for (i32 x = 0; x < w; ++x)
	{
		board.At(x, CurrentFlashLine).state = true;
	}

	// exit condition check
	if (Mode->Inputs.IsAnyActionDown())
	{
		return false; // goto fadeout anim loop
	}
	return true;
}

bool WaitStateNode::TickFadeOut(i32 LogicTick)
{
	Board& board = Mode->board;

	// advance logic
	i32 TargetColCount = CleanupColumn + LogicTick;
	for (; CleanupColumn < TargetColCount; ++CleanupColumn)
	{
		if (CleanupColumn > board.GetWidth())
		{
			return false;
		}

		i32 h = board.GetHeight();
		for (i32 y = 0; y < h; ++y)
		{
			board.At(CleanupColumn, y).state = false;
		}
	}
	return true;
}

void PlayStateNode::Enter()
{
	bCanFadeIn = false;
	bCanTick = true;
	bCanFadeOut = false;
	LogicTickSum = 0;

	Board& board = Mode->board;
	board.Clear();
}

i32 PlayStateNode::GetLogicTickRate()
{
	return 100;
}

bool PlayStateNode::Tick(i32 LogicTick)
{
	LogicTickSum += LogicTick;

	Board& board = Mode->board;

	Cell FillCell;
	FillCell.state = (LogicTickSum /10)%2 == 0;

	board.Fill(FillCell);

	return !Mode->Inputs.IsAnyActionDown();
}

bool StateTracker::SetNextStateNode(IStateNode* FromStateNode, IStateNode* ToStateNode, OutcomeId WhenOutcome/*=0*/)
{
	if (FromStateNode == nullptr)
		return false;

	AbsoluteOutcomeCode FromOutcome = MakeCode(*FromStateNode, WhenOutcome);

	// emplace returns a pair (iterator:dest, bool:inserted)
	auto res = NextStateNode.emplace(FromOutcome, ToStateNode);
	return res.second;
}

bool StateTracker::SetStartState(IStateNode* StateNode)
{
	CurrentStateNode = StateNode;
	return IsTickable();
}

bool StateTracker::Tick(i32 ms)
{
	TickConverter.AddTickBuffer(ms);

	if (CurrentStateNode == nullptr)
	{
		return false;
	}

	auto& s = *CurrentStateNode;
	ESubState capa = ESubState(ESS_Enter | s.bCanFadeIn*ESS_FadeIn | s.bCanTick*ESS_Tick | s.bCanFadeOut*ESS_FadeOut | ESS_Exit);
	while ((subState & capa) == 0)
		subState <<= 1;

	i32 LogicTickCount = 0;
	bool bTickableFrame = subState & ESS_TickableMask;
	if (bTickableFrame)
	{
		i32 TickRate = s.GetLogicTickRate();
		LogicTickCount = TickConverter.ConsumeAvailableLogicTick(TickRate);

		if (LogicTickCount == 0)
		{
			return true; // skip this frame (not enough time elapsed)
		}

		for (ILogicTickableSystem* TickableSystem : TickableSystems)
		{
			TickableSystem->LogicalTick(LogicTickCount);
		}
	}

	switch(subState)
	{
		case ESS_Enter:
			s.Enter();
			subState <<= 1;
			break;
		case ESS_FadeIn:
			if (!s.TickFadeIn(LogicTickCount))
				subState <<= 1;
			break;
		case ESS_Tick:
			if (!s.Tick(LogicTickCount))
				subState <<= 1;
			break;
		case ESS_FadeOut:
			if (!s.TickFadeOut(LogicTickCount))
				subState <<= 1;
			break;
		case ESS_Exit:
			// advance in the state chain
			u8 OutcomeValue = s.Exit();
			CurrentStateNode = Advance(CurrentStateNode, OutcomeValue);
			subState = ESS_Enter;
			break;
	}

	if (bTickableFrame)
	{
		for (ILogicTickableSystem* TickableSystem : TickableSystems)
		{
			TickableSystem->EndFrame(LogicTickCount);
		}
	}

	return IsTickable();
}

void StateTracker::AddTickableSystem(ILogicTickableSystem* Tickable)
{
	if (Tickable) TickableSystems.push_back(Tickable);
}

IStateNode* StateTracker::Advance(IStateNode* FromStateNode, OutcomeId OnOutcome)
{
	if (FromStateNode==nullptr)
		return nullptr;

	AbsoluteOutcomeCode FromOutcome = MakeCode(*FromStateNode, OnOutcome);
	auto it = NextStateNode.find(FromOutcome);

	return it != NextStateNode.end() ? it->second : nullptr;
}

} // ns tc
