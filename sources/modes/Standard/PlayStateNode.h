#pragma once

#include "details/Board.h"
#include "details/InputTracker.h"
#include "details/LevelManager.h"
#include "details/PieceGenerator.h"
#include "details/pieces.h"
#include "details/StateTracker.h"

#include "modes/NodeIds.h"


namespace tc
{

class PlayStateNode : public IStateNode
{
public:
	static constexpr OutcomeId WhenEnded = 0;
	static constexpr OutcomeId WhenPaused = 1;



public:
	PlayStateNode(Board& ModeBoard, InputTracker& ModeInputs);
	virtual StateNodeId Id() const override { return NodeIds::Play; }
	virtual void Enter(IStateNode* PreviousNode) override;
	virtual i32 GetLogicTickRate() override;
	virtual bool Tick(i32 LogicTick) override;
	virtual bool TickFadeOut(i32 LogicTick) override;
	virtual OutcomeId Exit() override;

public:
	const LevelManager& GetLevelManager() const { return Level; }

private:
	Board& MainBoard;
	InputTracker& Inputs;

	u32 GravityTickBudget = 0;

	u32 AutoRepeatDelay = 170; // TDG.5.2: Approx 0.3s
	u32 AutoRepeatSpeed = 50;

	struct HorizontalInputTracker
	{
		i32 LastDirection = 0;
		u32 MoveTickBudget = 0;
		u32 RepeatCount = 0;
	};
	HorizontalInputTracker hzit;

	EPiece MovingBlockNature = Piece_None;
	EOrient MovingBlockOrient = Orient_N;
	i32 MovingBlockX = 0;
	i32 MovingBlockY = 0;

	std::vector<i32> CompletedLines;
	u32 CompleteLineAnimBudget = 0;
	PieceGenerator<> RPG;
	LevelManager Level;

	Board BackupState;
	OutcomeId CurrentOutcomeId;

	// Death anim
	i32 DeathAnimTime = 0;
	i32 LastY = 0;
};

} // ns tc
