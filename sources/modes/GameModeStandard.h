#pragma once
#include "GameMode.h"

#include "../details/Board.h"
#include "../details/InputTracker.h"
#include "../details/pieces.h"
#include "../details/StateTracker.h"

#include <map>
#include <numeric>
#include <algorithm>


namespace tc
{

enum SubState
{
	Setup, // init
	Wait, // menu loop
	Play, // std loop
	End, // death anim
	_count,
};

class WaitStateNode : public IStateNode
{
public:
	WaitStateNode(class StandardGameMode* Mode) : Mode(Mode) {}
	virtual u32 Id() const override { return SubState::Wait; }
	virtual void Enter() override;
	virtual i32 GetLogicTickRate() override;
	virtual bool Tick(i32 LogicTick) override;
	virtual bool TickFadeOut(i32 LogicTick) override;

private:
	class StandardGameMode* Mode;

	i32 CurrentFlashLine;
	i32 CleanupColumn;
};

template<i32 PackCount=1>
class RandomPieceGenerator
{
	static const i32 ActualPackCount = PackCount + 1;
	static const i32 PieceCount = ActualPackCount * 7;

public:
	RandomPieceGenerator()
		: CurrentIndex(0)
	{
		std::generate(Buffer.begin(), Buffer.end(), []{return SevenPack();});
	}

	EPiece pop()
	{
		EPiece Result = Buffer[CurrentIndex/7].seven[CurrentIndex%7];

		// Advance
		++CurrentIndex;
		CurrentIndex %= PieceCount;

		// replace consumed pack
		if (CurrentIndex % 7 == 0)
		{
			i32 LastPackIndex = (CurrentIndex/7)-1;
			LastPackIndex = (LastPackIndex + ActualPackCount) % ActualPackCount;
			Buffer[LastPackIndex] = SevenPack();
		}
		return Result;
	}

private:
	struct SevenPack
	{
		SevenPack()
		{
			i32 Source = 0;
			std::generate(seven.begin(), seven.end(), [&Source]{ return EPiece(Source++); });
			std::random_shuffle(seven.begin(), seven.end());
		}
		std::array<EPiece, 7> seven;
	};

	std::array<SevenPack, ActualPackCount> Buffer;
	i32 CurrentIndex = 0;
};


class PlayStateNode : public IStateNode
{
public:
	PlayStateNode(class StandardGameMode* Mode) : Mode(Mode) {}
	virtual u32 Id() const override { return SubState::Play; }
	virtual void Enter() override;
	virtual i32 GetLogicTickRate() override;
	virtual bool Tick(i32 LogicTick) override;

private:
	class StandardGameMode* Mode = nullptr;
	u32 LogicTickSum = 0;

	EPiece MovingBlockNature = Piece_None;
	i32 MovingBlockX = 0;
	i32 MovingBlockY = 0;
	EOrient MovingBlockOrient = Orient_N;

	bool bInLineDeleteAnim = false;
	u32 GravityTickTreshold = 10; // not flexible enough... we'll see
	RandomPieceGenerator<> RPG;
};


class StandardGameMode : public GameMode
{
public:
	StandardGameMode(i32 Width=10, i32 Height=22);

	virtual void RegisterInput(EGameplayInput Input) override;

	virtual void InternalTick(i32 ms) override;

	const Board& GetBoard() const { return board; }

private:
	Board board;

	InputTracker Inputs;

	StateTracker Tracker;

	WaitStateNode WaitState;
	friend WaitStateNode;

	PlayStateNode PlayState;
	friend PlayStateNode;
};

} // ns tc
