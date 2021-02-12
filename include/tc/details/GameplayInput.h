// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#pragma once

namespace tc
{

enum EGameplayInput
{
	Start,
	Select,

	RotateLeft,
	RotateRight,
	MoveLeftPressed,
	MoveLeftReleased,
	MoveRightPressed,
	MoveRightReleased,
	MoveDownPressed,
	MoveDownReleased,

	HardDrop,
	Hold,
};

} // ns tc
