#pragma once

#include "details/Board.h"
#include "details/InputTracker.h"
#include "details/StateTracker.h"

#include "modes/NodeIds.h"


namespace tc
{


class WaitStateNode : public IStateNode
{
public:
	WaitStateNode(Board& ModeBoard, InputTracker& ModeInputs);
	virtual StateNodeId Id() const override { return NodeIds::Wait; }
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


} // ns tc
