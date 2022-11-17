#pragma once

typedef unsigned char u8;
typedef unsigned int u32;
typedef int i32;
typedef unsigned long long u64;
typedef float f32;

#ifdef DEBUG

#include <assert.h>
#define sui_assert(x) assert(x)

#else

#define sui_assert(x)

#endif
