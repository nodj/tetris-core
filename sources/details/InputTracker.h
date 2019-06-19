#pragma once

#include "types.h"
#include "GameplayInput.h"

namespace tc
{

class InputTracker
{
public:
	void RegisterInput(EGameplayInput Input);
	void ConsumeLogicalTicks(i32 LogicTickCount);

	bool IsAnyActionDown();
private:
	// Input state
	bool btnLeftDown;
	bool btnRightDown;
	bool btnRotateLeftDown;
	bool btnRotateRightDown;

	// Input interpretation
	i32 btnLeftCount;
	i32 btnRightCount;
	i32 moveDir; // right when > 0
};

} // ns tc