#pragma once

/***************************************************************************************
* AoLibrary hashes
****************************************************************************************
* - The main hash functions/objects used by the library
* - These are the hashes used for several containers like map and unordered_set
***************************************************************************************/

#include "config.h"

#ifdef AOL_USE_UNORDERED_DENSE_HASH
#include "unordered_dense/unordered_dense.h"
#else
#include <memory>
#endif

namespace AoL::Internal
{

/**
* The default hash class used by the library
* 
* - By default uses ankerl::unordered_dense::hash
* 
* -- This hash seems to be way more performant than std::hash so it stays as the default
* 
* -- This hash can be specialize the same way std::hash is specialized. Go to unordered_dense.h for more information
* 
* @tparam T type to be hashed
*/
template<typename T>
using DefaultHash
#ifdef AOL_USE_UNORDERED_DENSE_HASH
= ankerl::unordered_dense::hash<T>;
#else
= std::hash<T>;
#endif // AOL_USE_UNORDERED_DENSE_HASH


} // namespace AoL::Internal