#include "tc/modes/Standard/PauseStateNode.h"

#include "tc/modes/NodeIds.h"

namespace tc
{

PauseStateNode::PauseStateNode(Board& ModeBoard, InputTracker& ModeInputs)
	: MainBoard(ModeBoard)
	, Inputs(ModeInputs)
	, MsgOffsetX(0)
{
	bCanTick = true;
}

void PauseStateNode::Enter(IStateNode* /*PreviousNode*/)
{
	MsgOffsetX = -MainBoard.GetWidth();
	MainBoard.Clear();
}

tc::i32 PauseStateNode::GetLogicTickRate()
{
	return 300;
}

bool PauseStateNode::Tick(i32 /*LogicTick*/)
{
	bool bContinue = !Inputs.IsStartInvoked();

	u32 MsgBitmap[] = {
		0b1100010010100110011,
		0b1010101010101000100,
		0b1100111010100100110,
		0b1000101010100010100,
		0b1000101001001100011,
	};
	i32 MsgHeight = 5;
	i32 MsgWidth = 19;
	i32 MsgPadding = 5;

	MsgOffsetX++;

	i32 w = MainBoard.GetWidth();
	i32 h = MainBoard.GetHeight();
	i32 MinRow = (h-MsgHeight) / 2;
	for (i32 x = 0; x < w; ++x)
	{
		for (i32 y = 0; y < MsgHeight; ++y)
		{
			i32 InMsgPixelColumn = (x + MsgOffsetX) % (MsgWidth + MsgPadding);
			i32 Mask = 1 << (MsgWidth - 1 - InMsgPixelColumn);
			bool bIsOn = x + MsgOffsetX >= 0 && (MsgBitmap[MsgHeight-1-y] & Mask);
			MainBoard.At(x, MinRow + y).phantom = bIsOn;
		}
	}

	return bContinue;
}

} // ns tc
