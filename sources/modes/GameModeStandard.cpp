#include "GameModeStandard.h"
#include "../details/SRS.h"

namespace tc
{

StandardGameMode::StandardGameMode(i32 Width, i32 Height)
	: SharedBoard(Width, Height)
	, WaitState(SharedBoard, Inputs)
	, PlayState(SharedBoard, Inputs)
	, PauseState(SharedBoard, Inputs)
{
	Tracker.SetNextStateNode(&WaitState, &PlayState);
	Tracker.SetNextStateNode(&PlayState, &WaitState, WhenEnded);

	Tracker.SetNextStateNode(&PlayState, &PauseState, WhenPaused);
	Tracker.SetNextStateNode(&PauseState, &PlayState);

	Tracker.SetStartState(&WaitState);
}

void StandardGameMode::RegisterInput(EGameplayInput Input)
{
	Inputs.RegisterInput(Input);
}

void StandardGameMode::InternalTick(i32 Ms)
{
	if (Tracker.Tick(Ms))
		Inputs.EndFrame();

	if (!Tracker.IsTickable())
		SetOver();
}

bool StandardGameMode::IsInPlayMode() const
{
	return Tracker.GetCurrentStateNodeId() == SubState::Play
		|| Tracker.GetCurrentStateNodeId() == SubState::Wait;
}

WaitStateNode::WaitStateNode(Board& ModeBoard, InputTracker& ModeInputs)
	: MainBoard(ModeBoard)
	, Inputs(ModeInputs)
{
}

void WaitStateNode::Enter(IStateNode* PreviousNode)
{
	// config capabilities
	bCanFadeIn = false;
	bCanTick = true;
	bCanFadeOut = true;

	// init internal state
	CurrentFlashLine = 0;
	CleanupColumn = 0;
	MainBoard.Clear();
}

i32 WaitStateNode::GetLogicTickRate()
{
	return 100;
}

bool WaitStateNode::Tick(i32 LogicTick)
{
	// advance logic
	CurrentFlashLine += LogicTick;
	CurrentFlashLine %= MainBoard.GetHeight();

	// draw on board
	MainBoard.ResetToConsolidated();

	i32 w = MainBoard.GetWidth();
	for (i32 x = 0; x < w; ++x)
	{
		MainBoard.At(x, CurrentFlashLine).state = true;
	}

	// exit condition check
	if (Inputs.IsStartInvoked())
	{
		return false; // goto fadeout anim loop
	}
	return true;
}

bool WaitStateNode::TickFadeOut(i32 LogicTick)
{
	// advance logic
	i32 TargetColCount = CleanupColumn + LogicTick;
	for (; CleanupColumn < TargetColCount; ++CleanupColumn)
	{
		if (CleanupColumn > MainBoard.GetWidth())
		{
			return false;
		}

		i32 h = MainBoard.GetHeight();
		for (i32 y = 0; y < h; ++y)
		{
			MainBoard.At(CleanupColumn, y).state = false;
		}
	}
	return true;
}

PlayStateNode::PlayStateNode(Board& ModeBoard, InputTracker& ModeInputs)
	: MainBoard(ModeBoard)
	, Inputs(ModeInputs)
	, AutoRepeatDelay(170) // TDG.5.2: Approx 0.3s
	, AutoRepeatSpeed(50)
	, Level(LevelUpPolicy::VariableWithBonus)
	, BackupState(0, 0)
{
}

void PlayStateNode::Enter(IStateNode* PreviousNode)
{
	CurrentOutcomeId = StandardGameMode::WhenEnded;
	bCanFadeIn = false;
	bCanTick = true;
	bCanFadeOut = false;

	GravityTickBudget = 0;
	CompleteLineAnimBudget = 0;

	bool bWasInPause = (PreviousNode && PreviousNode->Id() == SubState::Pause);
	if (bWasInPause)
	{
		// restore the backup
		MainBoard = BackupState;
		BackupState = Board(0, 0);
	}
	else
	{
		MovingBlockNature = Piece_None;
		MovingBlockOrient = Orient_N;
		MovingBlockX = 0;
		MovingBlockY = 0;

		RPG.Reset();
		Level.Reset();
		CompletedLines.clear();
		CompletedLines.reserve(4);

		MainBoard.Clear();
	}
}

i32 PlayStateNode::GetLogicTickRate()
{
	return 1;
}

bool PlayStateNode::Tick(i32 LogicTick)
{
	GravityTickBudget += LogicTick;

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
			MovingBlockX = (MainBoard.GetWidth()-3)/2;
			MovingBlockY = MainBoard.GetHeight();

			// Validate that we spawn on an available area
			const Span& newSpan = GetSpan(MovingBlockNature, MovingBlockOrient);
			bool bSpawnSucceed = MainBoard.Blit(newSpan, MovingBlockX, MovingBlockY, Cell{}, Board::BlockLayer::Static, Board::BlockLayer::None);
			if (!bSpawnSucceed)
			{
				CurrentOutcomeId = StandardGameMode::WhenEnded;
				return false;
			}
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
					for (i32 x = 0; x < MainBoard.GetWidth(); ++x)
						MainBoard.At(x, y).state = bIsOn;
			}
			else
			{
				MainBoard.DeleteLines(CompletedLines);
				CompletedLines.clear();
			}
			return true;
		}
	}

	Span CurrentSpan = GetSpan(MovingBlockNature, MovingBlockOrient);
	Cell Value;
	Value.state = true;
	Value.nature = MovingBlockNature;

	bool bMustLockDown = false;
	if (MovingBlockNature != EPiece::Piece_None)
	{
		// Consume move inputs
		if (i32 HzDirection = Inputs.GetHorizontalDirection())
		{
			// reset state if we change direction
			if (hzit.LastDirection != HzDirection)
			{
				hzit = HorizontalInputTracker();
				hzit.LastDirection = HzDirection;
			}

			hzit.MoveTickBudget += LogicTick;

			while (true)
			{
				u32 TickThreshold = hzit.RepeatCount == 0 ? 0 : hzit.RepeatCount == 1 ? AutoRepeatDelay : AutoRepeatSpeed;
				if (hzit.MoveTickBudget > TickThreshold)
				{
					hzit.MoveTickBudget -= TickThreshold;
					hzit.RepeatCount++;

					if (MainBoard.Blit(CurrentSpan, MovingBlockX + HzDirection, MovingBlockY, Value, Board::BlockLayer::Static, Board::BlockLayer::None))
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
		if (i32 Rotation = Inputs.GetRotation())
		{
			EOrient TestOrient = EOrient((MovingBlockOrient + Rotation + 4) % 4);
			Span TestSpan = GetSpan(MovingBlockNature, TestOrient);

			bool bIsLeftRotation = Rotation == -1;
			const SRSOffsetLine& line = GetSRSOffsets(MovingBlockNature, TestOrient, bIsLeftRotation);

			for (const auto& offset : line)
			{
				i32 TestX = MovingBlockX+offset[0];
				i32 TestY = MovingBlockY+offset[1];
				if (MainBoard.Blit(TestSpan, TestX, TestY, Value, Board::BlockLayer::Static, Board::BlockLayer::None))
				{
					MovingBlockOrient = TestOrient;
					MovingBlockX = TestX;
					MovingBlockY = TestY;
					CurrentSpan = TestSpan;
					break;
				}
			}
		}

		// Compute phantom
		i32 HardDroppedY = MovingBlockY;
		while (MainBoard.Blit(CurrentSpan, MovingBlockX, HardDroppedY-1, Value, Board::BlockLayer::Static, Board::BlockLayer::None))
		{
			HardDroppedY--;
		}

		// Consume gravity
		bool bHardDrop = Inputs.IsHardDropInvoked();
		if (bHardDrop)
		{
			Level.RegisterDrop(MovingBlockY - HardDroppedY, true);
			MovingBlockY = HardDroppedY;
			bMustLockDown = true;
		}
		else
		{
			// Normal gravity evaluation
			u32 GravityThreshold = Level.GetCurrentFallSpeed();

			// TDG.5.5: SoftDrop is 20x faster than normal drop
			bool bSoftDrop = Inputs.IsSoftDropInvoked();
			if (bSoftDrop)
				GravityThreshold /= 20;

			while (GravityTickBudget >= GravityThreshold)
			{
				GravityTickBudget -= GravityThreshold;
				if (bHardDrop || bSoftDrop)
					GravityTickBudget = 0;
				i32 GravityDisplacement = 1;
				i32 TestY = MovingBlockY - GravityDisplacement;
				if (MainBoard.Blit(CurrentSpan, MovingBlockX, TestY, Value, Board::BlockLayer::Static, Board::BlockLayer::None))
				{
					if (bSoftDrop)
						Level.RegisterDrop(MovingBlockY - TestY, false);
					MovingBlockY = TestY;
				}
				else
				{
					bMustLockDown = true;
					GravityTickBudget = 0;
					break;
				}
			}
		}

		if (bMustLockDown)
		{
			// #tc_todo TDG.5.7: Extended/Infinite/Classic LockDown
			Value.locked = true;
			MainBoard.Blit(CurrentSpan, MovingBlockX, MovingBlockY, Value, Board::BlockLayer::None, Board::BlockLayer::Static);
			MainBoard.ResetToConsolidated();

			// clear MovingBlock state (to spawn a new one in the next frame)
			MovingBlockNature = EPiece::Piece_None;
			MovingBlockX = -1;
			MovingBlockY = -1;
			GravityTickBudget = 0;

			// complete lines
			for (i32 y = 0; y < MainBoard.GetHeight(); ++y)
			{
				if (MainBoard.IsLineComplete(y))
				{
					CompletedLines.push_back(y);
				}
			}

			if (u32 ClearedLinesCount = (u32)CompletedLines.size())
			{
				Level.RegisterClearedLines(ClearedLinesCount);
				bool bFancyFlash = ClearedLinesCount >= 4;
				bool bSimpleFlash = ClearedLinesCount > 0;
				CompleteLineAnimBudget = 150 * 1 * bSimpleFlash + 150 * 4 * bFancyFlash;
			}
		}
		else
		{
			// update board if state changed
			if (PreviousX != MovingBlockX || PreviousY != MovingBlockY || PreviousO != MovingBlockOrient)
			{
				MainBoard.ResetToConsolidated();
				MainBoard.Blit(CurrentSpan, MovingBlockX, HardDroppedY, Value.AsPhantom(), Board::BlockLayer::None, Board::BlockLayer::Merged);
				MainBoard.Blit(CurrentSpan, MovingBlockX, MovingBlockY, Value, Board::BlockLayer::None, Board::BlockLayer::Merged);
			}
		}
	}

	bool bContinue = true;
	if (Inputs.IsSelectInvoked())
	{
		CurrentOutcomeId = StandardGameMode::WhenEnded;
		bContinue = false;
	}

	if (Inputs.IsStartInvoked())
	{
		CurrentOutcomeId = StandardGameMode::WhenPaused;
		BackupState = MainBoard;
		bContinue = false;
	}

	return bContinue;
}


OutcomeId PlayStateNode::Exit()
{
	return CurrentOutcomeId;
}

PauseStateNode::PauseStateNode(Board& ModeBoard, InputTracker& ModeInputs)
	: MainBoard(ModeBoard)
	, Inputs(ModeInputs)
{
	bCanTick = true;
}

void PauseStateNode::Enter(IStateNode* /*PreviousNode*/)
{
	MainBoard.Clear();
}

bool PauseStateNode::Tick(i32 LogicTick)
{
	bool bContinue = !Inputs.IsStartInvoked();

	return bContinue;
}

} // ns tc
