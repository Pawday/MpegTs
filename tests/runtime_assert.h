#pragma once

#if defined(NDEBUG)
#include <stdbool.h>
void assert(bool value);
#else
#include <assert.h>
#endif
