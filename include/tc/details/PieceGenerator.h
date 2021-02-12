// This file is part of the tetris-core project. Copyright (c) Johan Duparc.

#pragma once

#include "tc/details/types.h"

#include <algorithm>
#include <array>
#include <random>


namespace tc
{

template<i32 PackCount=1>
class PieceGenerator
{
	static const i32 ActualPackCount = PackCount + 1; // PackCount complete, + 1 consuming
	static const i32 BufferSize = ActualPackCount * 7;

public:
	PieceGenerator()
		: CurrentIndex(0)
	{
		Reset();
	}

	void Reset()
	{
		CurrentIndex = 0;
		std::generate(Buffer.begin(), Buffer.end(), []{return SevenPack();});

		// Prevent a start with a 'snake' piece
		i8 i = 4;
		while(i-- && (Buffer[0][0] == EPiece::Piece_S || Buffer[0][0] == EPiece::Piece_Z))
			Buffer[0].Randomize();
	}

	EPiece pop()
	{
		EPiece Result = Buffer[CurrentIndex/7][CurrentIndex%7];

		// Advance
		++CurrentIndex;
		CurrentIndex %= BufferSize;

		// replace consumed pack
		if (CurrentIndex % 7 == 0)
		{
			i32 LastPackIndex = (CurrentIndex/7)-1;
			LastPackIndex = (LastPackIndex + ActualPackCount) % ActualPackCount;
			Buffer[LastPackIndex] = SevenPack();
		}
		return Result;
	}

private:
	struct SevenPack
	{
		SevenPack()
		{
			i32 Source = 0;
			std::generate(seven.begin(), seven.end(), [&Source]{ return EPiece(Source++); });
			Randomize();
		}

		void Randomize()
		{
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(seven.begin(), seven.end(), g);
		}

		EPiece operator [](size_t i) const { return seven[i]; }

	private:
		std::array<EPiece, 7> seven;
	};

	std::array<SevenPack, ActualPackCount> Buffer;
	i32 CurrentIndex = 0;
};

} // ns tc
