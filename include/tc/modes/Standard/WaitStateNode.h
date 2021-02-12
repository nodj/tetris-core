// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#pragma once

#include "tc/details/Board.h"
#include "tc/details/InputTracker.h"
#include "tc/details/StateTracker.h"

#include "tc/modes/NodeIds.h"


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
