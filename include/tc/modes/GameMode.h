// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#pragma once

#include "tc/details/GameplayInput.h"
#include "tc/details/types.h"


namespace tc
{

struct GameMode
{
	virtual ~GameMode() = default;

	virtual void RegisterInput(EGameplayInput Input) = 0;
	void Tick(float delta_s);
	bool IsOver() const { return bIsOver; }

protected:
	virtual void InternalTick(i32 ms) = 0;
	void SetOver() { bIsOver = true; }

private:
	// handle offset between int ticks and elapsed time
	float lostTime = 0;

	// Pollable flag that declares this GameMode over
	bool bIsOver = 0;
};



struct NullGameMode : public GameMode
{
	virtual void RegisterInput(EGameplayInput) override {}
	virtual void InternalTick(i32) override {}
};

} // ns tc
