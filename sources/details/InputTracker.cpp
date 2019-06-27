#include "InputTracker.h"

#include <bitset>
#include <assert.h>

namespace tc
{

InputTracker::InputTracker()
	: btnLeftDown(0)
	, btnRightDown(0)
	, actionRotateLeft(0)
	, actionRotateRight(0)
	, btnLeftCount(0)
	, btnRightCount(0)
	, moveDir(0)
{
}

void InputTracker::RegisterInput(EGameplayInput Input)
{
	// Set state (down or not) of buttons.
	// Tick interpret that state.
	switch (Input)
	{
		case EGameplayInput::RotateLeft:
			actionRotateLeft = true;
			break;
		case EGameplayInput::RotateRight:
			actionRotateRight = true;
			break;
		case EGameplayInput::MoveLeftPressed:
			btnLeftDown = true;
			break;
		case EGameplayInput::MoveLeftReleased:
			btnLeftDown = false;
			break;
		case EGameplayInput::MoveRightPressed:
			btnRightDown = true;
			break;
		case EGameplayInput::MoveRightReleased:
			btnRightDown = false;
			break;
		default:
			assert(false);
	}
}

bool InputTracker::IsAnyActionDown() const
{
	return actionRotateLeft || actionRotateRight;
}

i32 InputTracker::GetHorizontalDirection() const
{
	return btnRightDown - btnLeftDown;
}

void InputTracker::LogicalTick(i32 LogicTickCount)
{
	btnRightCount = btnRightDown ? btnRightCount + LogicTickCount : 0;
	btnLeftCount = btnLeftDown ? btnLeftCount + LogicTickCount : 0;
	moveDir = LogicTickCount * (btnRightDown - btnLeftDown);
}

void InputTracker::EndFrame(i32 /*LogicTickCount*/)
{
	actionRotateLeft = false;
	actionRotateRight = false;
}

} // ns tc