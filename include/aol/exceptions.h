/***************************************************************************************
* AoLibrary exceptions
****************************************************************************************
* - The library's custom exception
* - With the standard exception library using std::string which is different from the
*	library's string, I had to make a custom exception
* - Do make sure to use the standard library exception for stl-related things
***************************************************************************************/
#ifndef AOL_HEADER_EXCEPTIONS_H
#define AOL_HEADER_EXCEPTIONS_H


#include "configs.h"
#include "macros.h"
#include "traits.h"
#include "types.h"

#include "internal/exceptions/base.h"
#include "internal/exceptions/runtime.h"


#endif // AOL_HEADER_EXCEPTIONS_H