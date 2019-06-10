
#pragma once

#include "stdint.h"
#include "details/Board.h"


using int32 = int32_t;


/*
 *https://tetris.fandom.com/wiki/Tetris_Guideline
tickable, expected 60 tick/s
speed per level



*/




class TetrisState
{
    TetrisState();


private:
    Board board;
    int32 tickCount; // current frame
    int32 ticksPerDrop; // drives the speed of the game
};
