#include "tc/TetrisHost.h"
#include "tc/details/Board.h"

#include <iostream>
#include <bitset>

using namespace tc;

void printSpan(const Span& span)
{
	std::bitset<16> bits(span.pixels);
	char line[] = "....";
	for (u32 i = 0; i < 16; ++i)
	{
		line[i % 4] = bits.test(15 - i) ? '#' : '.';
		if (i % 4 == 3)
		{
			std::cout << line << std::endl;
		}
	}
	std::cout << std::endl;
}

int main()
{
	Board b(10, 22);

	std::cout << "Hi there" << std::endl;

	for (i32 i = 0; i < Piece_Count; ++i)
	{
		for (i32 o = 0; o < 4; ++o)
		{
			const Span& s = GetSpan(EPiece(i), EOrient(o));
			printSpan(s);
		}
	}

	return 0;
}

