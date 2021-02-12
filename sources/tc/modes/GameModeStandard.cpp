#include "tc/modes/GameModeStandard.h"

#include "tc/modes/Standard/PlayStateNode.h"
#include "tc/modes/Standard/WaitStateNode.h"
#include "tc/modes/Standard/PauseStateNode.h"


namespace tc
{

StandardGameMode::StandardGameMode(i32 Width, i32 Height)
	: SharedBoard(Width, Height)
{
	PlayState = std::make_unique<PlayStateNode>(SharedBoard, Inputs);
	WaitState = std::make_unique<WaitStateNode>(SharedBoard, Inputs);
	PauseState = std::make_unique<PauseStateNode>(SharedBoard, Inputs);

	Tracker.SetNextStateNode(WaitState.get(), PlayState.get());
	Tracker.SetNextStateNode(PlayState.get(), WaitState.get(), PlayStateNode::WhenEnded);
	Tracker.SetNextStateNode(PlayState.get(), PauseState.get(), PlayStateNode::WhenPaused);
	Tracker.SetNextStateNode(PauseState.get(), PlayState.get());

	Tracker.SetStartState(WaitState.get());
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
	return false
		|| Tracker.GetCurrentStateNodeId() == NodeIds::Play
		|| Tracker.GetCurrentStateNodeId() == NodeIds::Wait
		|| Tracker.GetCurrentStateNodeId() == NodeIds::Pause
	;
}

const BasicGameStats& StandardGameMode::GetBasicStats() const
{
	return PlayState->GetLevelManager().GetCurrentStats();
}

} // ns tc
