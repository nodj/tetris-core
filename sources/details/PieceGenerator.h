#pragma once

#include "types.h"

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
		std::generate(Buffer.begin(), Buffer.end(), []{return SevenPack();});
		CurrentIndex = 0;
	}

	EPiece pop()
	{
		EPiece Result = Buffer[CurrentIndex/7].seven[CurrentIndex%7];

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

			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(seven.begin(), seven.end(), g);
		}
		std::array<EPiece, 7> seven;
	};

	std::array<SevenPack, ActualPackCount> Buffer;
	i32 CurrentIndex = 0;
};

} // ns tc
