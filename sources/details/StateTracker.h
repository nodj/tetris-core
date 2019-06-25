#pragma once

#include "types.h"
#include "ILogicTickableSystem.h"
#include "LogicTickConverter.h"

#include <map>
#include <vector>


namespace tc
{

struct IStateNode
{
	IStateNode()
		: bCanFadeIn(0)
		, bCanTick(0)
		, bCanFadeOut(0)
	{}

	// return the identifier of this state
	virtual u32 Id() const = 0;

	// called once on state init
	virtual void Enter() {}

	// called pre Tick* calls
	virtual i32 GetLogicTickRate() = 0;

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
	// return an outcome value usable to orient to the next state
	// ex: a state could have a Win or Lose outcome value, with a map of following state
	virtual u8 Exit() { return 0; }

	u8 bCanFadeIn : 1;
	u8 bCanTick : 1;
	u8 bCanFadeOut : 1;
};

class StateTracker
{
	using AbsoluteOutcomeCode = u64;

public:
	using StateNodeId = u32;
	using OutcomeId = u8;

	// return true when insertion succeed
	bool SetNextStateNode(IStateNode* FromStateNode, IStateNode* ToStateNode, OutcomeId WhenOutcome=0);
	bool SetStartState(IStateNode* StateNode);

	// Timing
	// return true if tickable
	bool Tick(i32 ms);
	bool IsTickable() { return CurrentStateNode!= nullptr; }

	LogicTickConverter& GetTicker()
	{ return TickConverter; }

	void AddTickableSystem(ILogicTickableSystem* Tickable);

private:
	AbsoluteOutcomeCode MakeCode(const IStateNode& From, OutcomeId WhenOutcome)
	{ return u64(From.Id()) << 8 | WhenOutcome; }

	IStateNode* Advance(IStateNode* FromStateNode, OutcomeId OnOutcome);

private:
	using ESubState = u8;
	static const ESubState ESS_Enter   = 1 << 0;
	static const ESubState ESS_FadeIn  = 1 << 1;
	static const ESubState ESS_Tick    = 1 << 2;
	static const ESubState ESS_FadeOut = 1 << 3;
	static const ESubState ESS_Exit    = 1 << 4;
	static const ESubState ESS_TickableMask = ESS_FadeIn | ESS_Tick | ESS_FadeOut;

	// machine description
	std::map<AbsoluteOutcomeCode, IStateNode*> NextStateNode;
	std::map<StateNodeId, IStateNode*> StateNodes;

	IStateNode* CurrentStateNode = nullptr;
	ESubState subState = ESS_Enter;

	LogicTickConverter TickConverter;
	std::vector<ILogicTickableSystem*> TickableSystems;
};

} // ns tc