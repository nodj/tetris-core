#include "InputTracker.h"

#include <bitset>
#include <assert.h>

namespace tc
{

void InputTracker::RegisterInput(EGameplayInput Input)
{
	// Set state (down or not) of buttons.
	// Tick interpret that state.
	switch (Input)
	{
		case EGameplayInput::RotateLeft:
			btnRotateLeftDown = true;
			break;
		case EGameplayInput::RotateRight:
			btnRotateRightDown = true;
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

void InputTracker::ConsumeLogicalTicks(i32 LogicTickCount)
{
	btnRightCount = btnRightDown ? btnRightCount + LogicTickCount : 0;
	btnLeftCount = btnLeftDown ? btnLeftCount + LogicTickCount : 0;
	moveDir = LogicTickCount * (btnRightDown - btnLeftDown);
}

bool InputTracker::IsAnyActionDown()
{
	return btnRotateLeftDown || btnRotateRightDown;
}

} // ns tc