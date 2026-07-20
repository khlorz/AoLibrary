/***************************************************************************************
* AoLibrary Macros - Attributes
****************************************************************************************
* - All macros for attributes used in the codebase
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_MACROS_ATTRIBUTES_H
#define AOL_HEADER_INTERNAL_MACROS_ATTRIBUTES_H


#include "aol/configs.h"


#define AOL_ATTRIB_NO_DISCARD [[nodiscard]]
#define AOL_ATTRIB_BRANCH_UNLIKELY [[unlikely]]
#define AOL_ATTRIB_BRANCH_LIKELY [[likely]]
#define AOL_ATTRIB_NO_UNQ_ADDRESS [[no_unique_address]]


#endif // AOL_HEADER_INTERNAL_MACROS_ATTRIBUTES_H