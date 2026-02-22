/****************************************************************
* AoLibrary implementations
* - This houses the implementations needed for the library
* - e.g stb implementations, mimalloc-new-delete override, etc.
* - Put all future implementations here [developer]
****************************************************************/

/***************************
* stb implementations
***************************/

#define STB_SPRINTF_IMPLEMENTATION


/***************************
* #includes
***************************/

#include "aol.h"

#ifdef AOL_USE_MIMALLOC_NEW_DELETE
#include "mimalloc/mimalloc-new-delete.h"
#endif
