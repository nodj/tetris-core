#include "tc/details/InputTracker.h"

#include <assert.h>
#include <bitset>

namespace tc
{

InputTracker::InputTracker()
	: btnLeftDown(0)
	, btnRightDown(0)
	, btnDownDown(0)
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
		case EGameplayInput::Hold:
			actionHold = true;
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
		case EGameplayInput::MoveDownPressed:
			btnDownDown = true;
			break;
		case EGameplayInput::MoveDownReleased:
			btnDownDown = false;
			break;
		default:
			assert(false);
	}
}

void InputTracker::EndFrame()
{
	actionRotateLeft = false;
	actionRotateRight = false;
	actionStart = false;
	actionSelect = false;
	actionHardDrop = false;
	actionHold = false;
}

} // ns tc
