#include "modes/Standard/PlayStateNode.h"

#include "modes/NodeIds.h"
#include "details/SRS.h"


namespace tc
{

PlayStateNode::PlayStateNode(Board& ModeBoard, InputTracker& ModeInputs)
	: MainBoard(ModeBoard)
	, Inputs(ModeInputs)
	, Level(LevelUpPolicy::VariableWithBonus)
	, BackupState(0, 0)
{
}

void PlayStateNode::Enter(IStateNode* PreviousNode)
{
	CurrentOutcomeId = WhenEnded;
	bCanFadeIn = false;
	bCanTick = true;
	bCanFadeOut = true;

	DeathAnimTime = 0;
	DeathAnimLastY = 0;
	DeathAnimHasTopped = false;

	bool bWasInPause = (PreviousNode && PreviousNode->Id() == NodeIds::Pause);

	if (!bWasInPause)
	{
		// init the game
		GravityTickBudget = 0;
		CompleteLineAnimBudget = 0;
		HoldBlockNature = Piece_None;
		MovingBlockNature = Piece_None;
		MovingBlockOrient = Orient_N;
		MovingBlockX = 0;
		MovingBlockY = 0;
		bCanHold = true;

		RPG.Reset();
		Level.Reset();
		CompletedLines.clear();
		CompletedLines.reserve(4);

		MainBoard.Clear();
	}
	else
	{
		// restore the backup
		MainBoard = BackupState;
		BackupState = Board(0, 0);
	}

}

i32 PlayStateNode::GetLogicTickRate()
{
	return 1;
}

bool PlayStateNode::Tick(i32 LogicTick)
{
	auto TickEnding = [&]()
	{
		if (Inputs.IsSelectInvoked())
		{
			CurrentOutcomeId = WhenEnded;
			bCanFadeOut = true;
			return false;
		}

		if (Inputs.IsStartInvoked())
		{
			BackupState = MainBoard;
			CurrentOutcomeId = WhenPaused;
			bCanFadeOut = false;
			return false;
		}
		return true;
	};

	bool bHoldTriggered = bCanHold && Inputs.IsHoldInvoked();
	if (bHoldTriggered)
	{
		bCanHold = false;

		std::swap(MovingBlockNature, HoldBlockNature);
		MovingBlockX = -1;
		MovingBlockY = -1;
		GravityTickBudget = 0;
	}

	GravityTickBudget += LogicTick;

	// get moving part
	i32 PreviousX = MovingBlockX;
	i32 PreviousY = MovingBlockY;
	i32 PreviousO = MovingBlockOrient;

	// Spawn a new piece
	if (MovingBlockNature == EPiece::Piece_None || bHoldTriggered)
	{
		if (CompletedLines.empty())
		{
			// Spawn a new piece !
			if (MovingBlockNature == EPiece::Piece_None)
				MovingBlockNature = RPG.pop();
			MovingBlockOrient = Orient_N;
			MovingBlockX = (MainBoard.GetWidth()-3)/2;
			MovingBlockY = MainBoard.GetHeight();
			bIsRotationMove = false;

			// Validate that we spawn on an available area
			const Span& newSpan = GetSpan(MovingBlockNature, MovingBlockOrient);
			bool bSpawnSucceed = MainBoard.Blit(newSpan, MovingBlockX, MovingBlockY, Cell{}, Board::BlockLayer::Static, Board::BlockLayer::None);
			if (!bSpawnSucceed)
			{
				CurrentOutcomeId = WhenEnded;
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
			return TickEnding();
		}
	}

	if (MovingBlockNature == EPiece::Piece_None)
	{
		return false;
	}

	Span CurrentSpan = GetSpan(MovingBlockNature, MovingBlockOrient);
	Cell Value;
	Value.state = true;
	Value.nature = MovingBlockNature;

	bool bMustLockDown = false;

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
					bIsRotationMove = false;
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
				bIsRotationMove = true;
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
		bIsRotationMove &= MovingBlockY == HardDroppedY; // if drop of 0 cells, it's more a lock than a drop. Don't invalidate T-Spin.
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
				bIsRotationMove = false;
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

		// TSpin detection
		bool bIsMiniTSpin = false;
		bool bIsTSpin = false;
		if (MovingBlockNature == EPiece::Piece_T && bIsRotationMove)
		{
			// off board cells are considered filled, Over-the-top cells are empty. We hack that:
			MainBoard.At(-1, 0).state = true;
			MainBoard.At(0, MainBoard.GetHeight()).state = false;

			// TDG.9.1: find the A B C and D cells: (respectively at indices 0 1 3 2)
			static std::array<i32, 4> dx{0, 2, 2, 0}, dy{0, 0,-2,-2};
			auto GetState = [&](i32 i){ return MainBoard.At(MovingBlockX + dx[i % 4], MovingBlockY + dy[i % 4]).state; };

			bool A = GetState(MovingBlockOrient + 0); //
			bool B = GetState(MovingBlockOrient + 1); //   A # B
			bool C = GetState(MovingBlockOrient + 3); //   # # #
			bool D = GetState(MovingBlockOrient + 2); //   C   D

			bIsMiniTSpin = C && D && (A || B);
			bIsTSpin = A && B && (C || D);
		}

		// clear MovingBlock state (to spawn a new one in the next frame)
		MovingBlockNature = EPiece::Piece_None;
		MovingBlockX = -1;
		MovingBlockY = -1;
		GravityTickBudget = 0;
		bCanHold = true;

		// complete lines
		for (i32 y = 0; y < MainBoard.GetHeight(); ++y)
		{
			if (MainBoard.IsLineComplete(y))
			{
				CompletedLines.push_back(y);
			}
		}

		u32 ClearedLinesCount = (u32)CompletedLines.size();
		if (ClearedLinesCount)
		{
			bool bFancyFlash = ClearedLinesCount >= 4;
			bool bSimpleFlash = ClearedLinesCount > 0;
			CompleteLineAnimBudget = 150 * 1 * bSimpleFlash + 150 * 4 * bFancyFlash;
		}
		Level.RegisterScore(ClearedLinesCount, bIsTSpin, bIsMiniTSpin);
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

	return TickEnding();
}

bool PlayStateNode::TickFadeOut(i32 LogicTick)
{
	DeathAnimTime += LogicTick;

	// First, observe your death...
	if (DeathAnimTime < 500)
		return true;

	i32 UpAnim = DeathAnimTime - 700;
	// 50 block per seconds:
	i32 CurrentY = UpAnim * 15 / 1000;

	i32 H = MainBoard.GetHeight();

	if (DeathAnimHasTopped)
		CurrentY -= H;

	Cell c;
	c.state = !DeathAnimHasTopped;

	for (i32 y = DeathAnimLastY; y < CurrentY; ++y)
		MainBoard.FillLine(y, c);

	DeathAnimLastY = CurrentY;
	if (!DeathAnimHasTopped && CurrentY >= H)
	{
		DeathAnimHasTopped = true;
		DeathAnimLastY = 0;
		return true;
	}
	return CurrentY < H;
}

} // ns tc
