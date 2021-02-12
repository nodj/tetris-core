// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#include "tc/details/StateTracker.h"

namespace tc
{

bool StateTracker::SetNextStateNode(IStateNode* FromStateNode, IStateNode* ToStateNode, OutcomeId WhenOutcome/*=0*/)
{
	if (FromStateNode == nullptr)
		return false;

	OutcomeEdgeCode FromOutcome = MakeCode(*FromStateNode, WhenOutcome);

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
			return false; // skip this frame (not enough time elapsed)
		}
	}

	switch(subState)
	{
		case ESS_Enter:
			s.Enter(PreviousStateNode);
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
			OutcomeId OutcomeValue = s.Exit();
			PreviousStateNode = CurrentStateNode;
			CurrentStateNode = Advance(PreviousStateNode, OutcomeValue);
			subState = ESS_Enter;
			break;
	}

	return bTickableFrame;
}

IStateNode* StateTracker::Advance(IStateNode* FromStateNode, OutcomeId OnOutcome)
{
	if (FromStateNode==nullptr)
		return nullptr;

	OutcomeEdgeCode FromOutcome = MakeCode(*FromStateNode, OnOutcome);
	auto it = NextStateNode.find(FromOutcome);

	return it != NextStateNode.end() ? it->second : nullptr;
}

} // ns tc
