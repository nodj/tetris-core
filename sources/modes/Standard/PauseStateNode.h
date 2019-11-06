#pragma once

#include "details/Board.h"
#include "details/InputTracker.h"
#include "details/StateTracker.h"

#include "modes/NodeIds.h"


namespace tc
{


class PauseStateNode : public IStateNode
{
public:
	PauseStateNode(Board& ModeBoard, InputTracker& ModeInputs);
	virtual StateNodeId Id() const override { return NodeIds::Pause; }

	virtual void Enter(IStateNode* PreviousNode) override;
	virtual i32 GetLogicTickRate() override;
	virtual bool Tick(i32 LogicTick) override;

private:
	Board& MainBoard;
	InputTracker& Inputs;
	i32 MsgOffsetX;
};


} // ns tc
