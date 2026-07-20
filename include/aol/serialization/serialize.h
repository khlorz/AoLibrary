/***************************************************************************************
* AoLibrary serialization
****************************************************************************************
* - Serialization of the library types require the cereal library
* - To serialize the type, just include this header after all other includes
* - Does not need to be included everywhere, include only when needed
***************************************************************************************/
#ifndef AOL_SERIALIZATION_SERIALIZE_H
#define AOL_SERIALIZATION_SERIALIZE_H


/*****************************************************
* Essential includes
*****************************************************/

#include "cereal/cereal.hpp"										/* main serialization include */
#include "cereal/types/base_class.hpp"								/* for inheritance types*/
#include "cereal/types/string.hpp"									/* std::string and the likes */
#include "cereal/types/vector.hpp"									/* std::vector and the likes */
#include "cereal/types/array.hpp"									/* std::array and the likes */
#include "cereal/types/set.hpp"										/* std::set and the likes */
#include "cereal/types/unordered_set.hpp"							/* std::unordered_set and the likes */
#include "cereal/types/concepts/pair_associative_container.hpp"		/* paired-containers like map and the likes */

#include "aol/configs.h"
#include "aol/macros.h"
#include "aol/traits.h"
#include "aol/types.h"


/*****************************************************
* Implementation includes
*****************************************************/

#include "serialize-containers-impl.h"
#include "serialize-data-components-impl.h"


#endif // AOL_SERIALIZATION_SERIALIZE_H