// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#pragma once

namespace tc
{

enum NodeIds
{
	Setup, // init
	Wait, // menu loop
	Play, // std loop
	Pause, // blank screen
	End, // death anim
	_count,
};

} // ns tc
