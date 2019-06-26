#include "GameModeStandard.h"

namespace tc
{

StandardGameMode::StandardGameMode(i32 Width, i32 Height)
	: board(Width, Height)
	, WaitState(this)
	, PlayState(this)
{
	Tracker.AddTickableSystem(&Inputs);

	Tracker.SetNextStateNode(&WaitState, &PlayState);
	Tracker.SetNextStateNode(&PlayState, &WaitState);
	Tracker.SetStartState(&WaitState);
}

void StandardGameMode::RegisterInput(EGameplayInput Input)
{
	Inputs.RegisterInput(Input);
}

void StandardGameMode::InternalTick(i32 ms)
{
	bool bShouldTickAgain = Tracker.Tick(ms);

	if (!bShouldTickAgain)
		SetOver();
}

void WaitStateNode::Enter()
{
	// config capabilities
	bCanFadeIn = false;
	bCanTick = true;
	bCanFadeOut = true;

	// init internal state
	CurrentFlashLine = 0;
	CleanupColumn = 0;
	Mode->board.Clear();
}

i32 WaitStateNode::GetLogicTickRate()
{
	return 100; // #tetris_todo no magic numbers
}

bool WaitStateNode::Tick(i32 LogicTick)
{
	Board& board = Mode->board;

	// advance logic
	CurrentFlashLine += LogicTick;
	CurrentFlashLine %= board.GetHeight();

	// draw on board
	board.ResetToConsolidated();

	i32 w = board.GetWidth();
	for (i32 x = 0; x < w; ++x)
	{
		board.At(x, CurrentFlashLine).state = true;
	}

	// exit condition check
	if (Mode->Inputs.IsAnyActionDown())
	{
		return false; // goto fadeout anim loop
	}
	return true;
}

bool WaitStateNode::TickFadeOut(i32 LogicTick)
{
	Board& board = Mode->board;

	// advance logic
	i32 TargetColCount = CleanupColumn + LogicTick;
	for (; CleanupColumn < TargetColCount; ++CleanupColumn)
	{
		if (CleanupColumn > board.GetWidth())
		{
			return false;
		}

		i32 h = board.GetHeight();
		for (i32 y = 0; y < h; ++y)
		{
			board.At(CleanupColumn, y).state = false;
		}
	}
	return true;
}

void PlayStateNode::Enter()
{
	bCanFadeIn = false;
	bCanTick = true;
	bCanFadeOut = false;

	LogicTickSum = 0;
	MovingBlockNature = Piece_None;
	MovingBlockX = 0;
	MovingBlockY = 0;
	MovingBlockOrient = Orient_N;

	bInLineDeleteAnim = false;
	GravityTickTreshold = 10; // not flexible enough... we'll see

	Board& board = Mode->board;
	board.Clear();
}

i32 PlayStateNode::GetLogicTickRate()
{
	return 100;
}

bool PlayStateNode::Tick(i32 LogicTick)
{
	LogicTickSum += LogicTick;

	Board& board = Mode->board;

	board.ResetToConsolidated();

	// get moving part
	bool bBlit = true;// LogicTickSum > GravityTickTreshold;
	if (MovingBlockNature == EPiece::Piece_None)
	{
		if (!bInLineDeleteAnim)
		{
			// Spawn a new piece !
			MovingBlockNature = RPG.pop(); // such random... Use random_shuffle and iota
			MovingBlockX = board.GetWidth()/2;
			MovingBlockY = board.GetHeight();
			bBlit = true; // force blit on spawn
		}
	}

	if (MovingBlockNature != EPiece::Piece_None)
	{
		// gravity
		if (bBlit)
		{
			// Try to go down... Or lock
			i32 x = MovingBlockX;

			i32 GravityDisplacement = 1;
			i32 y = MovingBlockY - GravityDisplacement;

			const Span& span = GetSpan(MovingBlockNature, MovingBlockOrient);
			Cell Value;
			Value.state = true;
			Value.nature = MovingBlockNature;

			if (board.TryBlit(span, x, y, Value))
			{
				MovingBlockY -= GravityDisplacement;
			}
			else
			{
				MovingBlockNature = EPiece::Piece_None;
			}
		}
	}


	// move based on inputs

	// blit moving part

	// detect end



// 	Cell FillCell;
// 	FillCell.state = (LogicTickSum /10)%2 == 0;
//
// 	board.Fill(FillCell);

	return !Mode->Inputs.IsAnyActionDown();
}


} // ns tc
