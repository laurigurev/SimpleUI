#pragma once

#ifdef DEBUG

#include <assert.h>
#define sui_assert(x) assert(x)
#else
#define sui_assert(x)
#endif

#define sui_max(a, b) ((a) > (b) ? (a) : (b))

