#include "stdafx.h"
#include "DLL_ASM.h"
#include <limits>

// block max macro
#ifdef max
#undef max 
extern "C" unsigned __int32 MAX_NUMBER_OF_PIXELS = std::numeric_limits<unsigned __int32>::max();
#endif