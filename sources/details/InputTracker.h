#pragma once

#include "types.h"
#include "GameplayInput.h"

namespace tc
{

class InputTracker
{
public:
	InputTracker();
	void RegisterInput(EGameplayInput Input);

	bool IsStartInvoked() const { return actionStart; }
	bool IsSelectInvoked() const { return actionSelect; }
	i32 GetHorizontalDirection() const;
	i32 GetRotation() const;
	bool IsHardDropInvoked() const { return actionHardDrop; }

	void EndFrame();

private:
	bool btnLeftDown:1;
	bool btnRightDown:1;
	bool actionRotateLeft:1;
	bool actionRotateRight:1;
	bool actionHardDrop:1;

	bool actionStart:1;
	bool actionSelect:1;
};

} // ns tc