#pragma once

#include "types.h"
#include "GameplayInput.h"
#include "ILogicTickableSystem.h"

namespace tc
{

class InputTracker : public ILogicTickableSystem
{
public:
	InputTracker();
	void RegisterInput(EGameplayInput Input);

	bool IsAnyActionDown() const;
	i32 GetHorizontalDirection() const;

	virtual void LogicalTick(i32 LogicTickCount) override;
	virtual void EndFrame(i32 LogicTickCount) override;

private:
	// Input state
	bool btnLeftDown:1;
	bool btnRightDown:1;
	bool actionRotateLeft:1;
	bool actionRotateRight:1;

	// Input interpretation
	i32 btnLeftCount;
	i32 btnRightCount;
	i32 moveDir; // right when > 0
};

} // ns tc