#pragma once
#include "types.h"

namespace tc
{

struct ILogicTickableSystem
{
    virtual void LogicalTick(i32 LogicTickCount) = 0;
    virtual void EndFrame(i32 LogicTickCount) = 0;
};

} // ns tc

