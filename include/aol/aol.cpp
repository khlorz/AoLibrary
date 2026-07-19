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
#include "third-party/stb/stb_sprintf.h"

/***************************
* #includes
***************************/

#include "configs.h"

#ifdef AOL_CONFIG_FLAG_USE_MIMALLOC_NEW_DELETE
#include "mimalloc/mimalloc-new-delete.h"
#elif defined(AOL_CONFIG_FLAG_USE_CUSTOM_NEW_AND_DELETE)
#error "No custom new and delete implementations yet!"
#else
#warning "Using the default new and delete implementations!"
#endif
