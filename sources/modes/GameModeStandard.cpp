#include "GameModeStandard.h"

namespace tc
{

StandardGameMode::StandardGameMode(i32 Width, i32 Height)
	: board(Width, Height)
	, WaitState(this)
	, PlayState(this)
{
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
	if (Tracker.Tick(ms))
		Inputs.EndFrame();

	if (!Tracker.IsTickable())
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
	if (Mode->Inputs.IsStartInvoked())
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

	TickIndex = 0;

	GravityTickBudget = 0;
	GravityTickTreshold = 300;

	MovingBlockNature = Piece_None;
	MovingBlockX = 0;
	MovingBlockY = 0;
	MovingBlockOrient = Orient_N;

	CompletedLines.clear();
	CompletedLines.reserve(4);
	CompleteLineAnimBudget = 0;

	Board& board = Mode->board;
	board.Clear();
}

i32 PlayStateNode::GetLogicTickRate()
{
	return 1;
}

bool PlayStateNode::Tick(i32 LogicTick)
{
	TickIndex += LogicTick;
	GravityTickBudget += LogicTick;

	Board& board = Mode->board;

	// get moving part
	i32 PreviousX = MovingBlockX;
	i32 PreviousY = MovingBlockY;
	i32 PreviousO = MovingBlockOrient;

	// Spawn a new piece
	if (MovingBlockNature == EPiece::Piece_None)
	{
		if (CompletedLines.empty())
		{
			// Spawn a new piece !
			MovingBlockNature = RPG.pop();
			MovingBlockOrient = Orient_N;
			MovingBlockX = board.GetWidth()/2;
			MovingBlockY = board.GetHeight();

			// Validate that we spawn on an available area
			const Span& newSpan = GetSpan(MovingBlockNature, MovingBlockOrient);
			bool bSpawnSucceed = board.Blit(newSpan, MovingBlockX, MovingBlockY, Cell{}, Board::BlockLayer::Static, Board::BlockLayer::None);
			if (!bSpawnSucceed)
				return false;
		}
		else
		{
			// We have lines pending delete:
			// 2 modes: blink anim or actual delete
			if (CompleteLineAnimBudget > 0)
			{
				CompleteLineAnimBudget -= std::min<u32>(LogicTick, CompleteLineAnimBudget);
				u32 WaveSide = 150;
				bool bIsOn = (CompleteLineAnimBudget / WaveSide) & 1;
				for (i32 y : CompletedLines)
					for (i32 x = 0; x < board.GetWidth(); ++x)
						board.At(x, y).state = bIsOn;
			}
			else
			{
				board.DeleteLines(CompletedLines);
				CompletedLines.clear();
			}
			return true;
		}
	}

	Span span = GetSpan(MovingBlockNature, MovingBlockOrient);
	Cell Value;
	Value.state = true;
	Value.nature = MovingBlockNature;

	bool bMustConsolidate = false;
	if (MovingBlockNature != EPiece::Piece_None)
	{
		// Consume move inputs
		if (i32 HzDirection = Mode->Inputs.GetHorizontalDirection())
		{
			// reset state if we change direction
			if (hzit.LastDirection != HzDirection)
			{
				hzit = HorizontalInputTracker();
				hzit.LastDirection = HzDirection;
			}

			hzit.MoveTickBudget += LogicTick;

			while(true)
			{
				u32 TickThreshold = hzit.RepeatCount == 0 ? 0 : hzit.RepeatCount == 1 ? AutoRepeatDelay : AutoRepeatSpeed;
				if (hzit.MoveTickBudget > TickThreshold)
				{
					hzit.MoveTickBudget -= TickThreshold;
					hzit.RepeatCount++;

					if (board.Blit(span, MovingBlockX + HzDirection, MovingBlockY, Value, Board::BlockLayer::Static, Board::BlockLayer::None))
					{
						MovingBlockX += HzDirection;
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			hzit.LastDirection = 0;
		}

		// Consume Rotation
		if (i32 Rotation = Mode->Inputs.GetRotation())
		{
			EOrient NewOrient = EOrient((MovingBlockOrient + Rotation + 4) % 4);
			Span TestSpan = GetSpan(MovingBlockNature, NewOrient);
			if (board.Blit(TestSpan, MovingBlockX, MovingBlockY, Value, Board::BlockLayer::Static, Board::BlockLayer::None))
			{
				MovingBlockOrient = NewOrient;
				span = TestSpan;
			}
		}

		// Consume gravity
		bool bHardDrop = Mode->Inputs.IsHardDropInvoked();
		u32 UsedGravityTickTreshold = bHardDrop ? 0 : GravityTickTreshold;
		while (GravityTickBudget >= UsedGravityTickTreshold)
		{
			GravityTickBudget -= UsedGravityTickTreshold;

			i32 GravityDisplacement = 1;
			i32 TestY = MovingBlockY - GravityDisplacement;
			if (board.Blit(span, MovingBlockX, TestY, Value, Board::BlockLayer::Static, Board::BlockLayer::None))
			{
				MovingBlockY = TestY;
			}
			else
			{
				bMustConsolidate = true;
				GravityTickBudget = 0;
				break;
			}
		}

		if (bMustConsolidate)
		{
			board.Blit(span, MovingBlockX, MovingBlockY, Value, Board::BlockLayer::None, Board::BlockLayer::Static);
			board.ResetToConsolidated();

			// clear MovingBlock state (to spawn a new one in the next frame)
			MovingBlockNature = EPiece::Piece_None;
			MovingBlockX = -1;
			MovingBlockY = -1;

			// complete lines
			for (i32 y = 0; y < board.GetHeight(); ++y)
			{
				if (board.IsLineComplete(y))
				{
					CompletedLines.push_back(y);
				}
			}

			bool bFancyFlash = CompletedLines.size() >= 4;
			bool bSimpleFlash = !CompletedLines.empty();
			CompleteLineAnimBudget = 150 * 1 * bSimpleFlash + 150 * 4 * bFancyFlash;
		}
		else
		{
			// update board if state changed
			if (PreviousX != MovingBlockX || PreviousY != MovingBlockY || PreviousO != MovingBlockOrient)
			{
				board.ResetToConsolidated();
				board.Blit(span, MovingBlockX, MovingBlockY, Value, Board::BlockLayer::None, Board::BlockLayer::Merged);
			}
		}
	}

	return !Mode->Inputs.IsStartInvoked();
}


} // ns tc
