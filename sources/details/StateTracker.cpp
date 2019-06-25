#include "StateTracker.h"

#include "ILogicTickableSystem.h"


namespace tc
{

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
	if (Tickable)
		TickableSystems.push_back(Tickable);
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