#pragma once

#include "../details/types.h"
#include "../details/GameplayInput.h"


namespace tc
{

struct GameMode
{
	virtual ~GameMode() = default;

	virtual void RegisterInput(EGameplayInput Input) = 0;
	void Tick(float delta_s);

protected:
	virtual void InternalTick(i32 ms) = 0;

private:
	float lostTime = 0; // handle offset between int ticks and elapsed time
};



struct NullGameMode : public GameMode
{
	virtual void RegisterInput(EGameplayInput) override {}
	virtual void InternalTick(i32) override {}
};

} // ns tc
