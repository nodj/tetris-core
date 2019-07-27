#pragma once

#include "pieces.h"
#include "types.h"

#include <array>

namespace tc
{

using SRSOffset = std::array<i8, 2>;
using SRSOffsetLine = std::array<SRSOffset, 5>;
using SRSTable = std::array<SRSOffsetLine, 16>;

const SRSOffsetLine& GetSRSOffsets(EPiece Nature, EOrient NewOrient, bool IsLeftRotation);

} // ns tc
