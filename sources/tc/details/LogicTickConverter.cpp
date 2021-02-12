#include "tc/details/LogicTickConverter.h"


namespace tc
{

LogicTickConverter::LogicTickConverter()
	: AccumulatedSubTicks(0)
{
}

i32 LogicTickConverter::ConsumeAvailableLogicTick(int SubtickPerLogicTick)
{
	i32 LogicTickCount = AccumulatedSubTicks / SubtickPerLogicTick;
	AccumulatedSubTicks -= LogicTickCount * SubtickPerLogicTick;
	return LogicTickCount;
}

} // ns tc
