#include "GameMode.h"


namespace tc
{

void GameMode::Tick(float delta_s)
{
    float advanceBy = delta_s + lostTime;

    int subTickCount = int(advanceBy * 1000);
    lostTime = advanceBy - 0.001f * subTickCount;

    InternalTick(subTickCount);
}

} // ns tc
