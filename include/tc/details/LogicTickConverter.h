#pragma once

#include "tc/details/types.h"


namespace tc
{

class LogicTickConverter
{
public:
	LogicTickConverter();

	void AddTickBuffer(i32 ms) { AccumulatedSubTicks += ms; }

	i32 ConsumeAvailableLogicTick(int SubtickPerLogicTick);

private:
	i32 AccumulatedSubTicks;
};

} // ns tc
