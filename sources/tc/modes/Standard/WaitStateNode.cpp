// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#include "tc/modes/Standard/WaitStateNode.h"

#include "tc/modes/NodeIds.h"

namespace tc
{

WaitStateNode::WaitStateNode(Board& ModeBoard, InputTracker& ModeInputs)
	: MainBoard(ModeBoard)
	, Inputs(ModeInputs)
{
}

void WaitStateNode::Enter(IStateNode* /*PreviousNode*/)
{
	// config capabilities
	bCanFadeIn = false;
	bCanTick = true;
	bCanFadeOut = true;

	// init internal state
	CurrentFlashLine = 0;
	CleanupColumn = 0;
	MainBoard.Clear();
}

i32 WaitStateNode::GetLogicTickRate()
{
	return 100;
}

bool WaitStateNode::Tick(i32 LogicTick)
{
	// advance logic
	CurrentFlashLine += LogicTick;
	CurrentFlashLine %= MainBoard.GetHeight();

	// draw on board
	MainBoard.ResetToConsolidated();

	Cell Value;
	Value.state = true;
	MainBoard.FillLine(CurrentFlashLine, Value);

	// exit condition check
	if (Inputs.IsStartInvoked())
	{
		return false; // goto fadeout anim loop
	}
	return true;
}

bool WaitStateNode::TickFadeOut(i32 LogicTick)
{
	// advance logic
	i32 TargetColCount = CleanupColumn + LogicTick;
	for (; CleanupColumn < TargetColCount; ++CleanupColumn)
	{
		if (CleanupColumn > MainBoard.GetWidth())
		{
			return false;
		}

		i32 h = MainBoard.GetHeight();
		for (i32 y = 0; y < h; ++y)
		{
			MainBoard.At(CleanupColumn, y).state = false;
		}
	}
	return true;
}

} // ns tc
