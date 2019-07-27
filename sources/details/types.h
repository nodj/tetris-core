#pragma once

#include <stdint.h>

namespace tc
{

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

template<typename T>
T Clamp(T Val, T Min, T Max) { return Val < Min ? Min : Val > Max ? Max : Val; }

} // ns tc
