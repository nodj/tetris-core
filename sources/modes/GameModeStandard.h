#pragma once
#include "GameMode.h"
#include "../details/Board.h"
#include <bitset>


namespace tc
{

template<typename T, int Size>
struct StateTracker
{
	StateTracker(T Initial) : Current(Initial), Snapshot(Initial) {}
	void Allow(T From, T To) { TransitionMatrix[Size*From+To] = true; }

	T Get() const { return Current; }
	void Set(T NewState)
	{
		if (TransitionMatrix[Size*Current+NewState])
		{
			EndFrame();
			Current = NewState;
		}
	}

	void StartFrame() const { }
	void EndFrame() { Snapshot = Current; }
	bool IsFirstFrame() const { return Snapshot != Current; }

private:
	T Current;
	T Snapshot;
	std::bitset<Size*Size> TransitionMatrix;
};

class StandardGameMode : public GameMode
{
public:
	StandardGameMode(i32 Width=10, i32 Height=22);
	~StandardGameMode() = default;

	virtual void RegisterInput(EGameplayInput Input) override;
	virtual void InternalTick(i32 ms) override;

	const Board& GetBoard() const { return board; }

private:

	bool ConsumeSubTicks(int RequiredTicks);

	i32 ConsumeAvailableLogicTick(int SubtickPerLogicTick);

	enum State
	{
		Setup, // init
		Wait, // menu loop
		Play, // std loop
		End, // death anim
		_count,
	};

	StateTracker<State, State::_count> currentState;

	template<State Current> bool Tick();
	Board board;
	i32 AccumulatedSubTicks;
	i32 SubTickPerFrame;

	struct
	{
		i32 CurrentFlashLine;
	} WaitStateContext;
};

} // ns tc
