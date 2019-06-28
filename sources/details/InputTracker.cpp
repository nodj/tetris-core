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
	, actionHardDrop(0)
	, actionStart(0)
	, actionSelect(0)
{
}

void InputTracker::RegisterInput(EGameplayInput Input)
{
	switch (Input)
	{
		case EGameplayInput::Start:
			actionStart = true;
			break;
		case EGameplayInput::Select:
			actionSelect = true;
			break;
		case EGameplayInput::HardDrop:
			actionHardDrop = true;
			break;
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

i32 InputTracker::GetHorizontalDirection() const
{
	return btnRightDown - btnLeftDown;
}

i32 InputTracker::GetRotation() const
{
	return actionRotateRight - actionRotateLeft;
}

void InputTracker::EndFrame()
{
	actionRotateLeft = false;
	actionRotateRight = false;
	actionStart = false;
	actionSelect = false;
	actionHardDrop = false;
}

} // ns tc