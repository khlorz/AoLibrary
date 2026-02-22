#pragma once

/***************************************************************************************
* AoLibrary Configurations
****************************************************************************************
* - Uncomment the defines you want to enable
* - This will apply to all AoLibrary files
***************************************************************************************/

/***************************************
* General Allocator
****************************************/

/**
* This replaces every new and delete functions to mimalloc new and delete
*
* - By default we will be using mimalloc
*/
#define AOL_USE_MIMALLOC_NEW_DELETE


/***************************************
* STL Allocator
****************************************/

/**
* This makes the whole AoLibrary use mimalloc library for STL allocators
*
* - When undefined, the library will use the default STL allocator, std::allocator<T>
*
* - However, by default, we will use std::allocator since the overridden new/delete already come from mimalloc
*/
//#define AOL_USE_MIMALLOCATOR_ALLOCATOR


/***************************************
* Pool Allocator
****************************************/

/**
* This makes the whole AoLibrary use mimalloc library for pool allocators
*
* - By default, the library will be using mimalloc library for pool allocators
*
* - Do make sure that AOL_USE_BOOST_POOL_ALLOCATOR is undefined before defining this
*
* - Allocators can still be customized per container; this is the default allocator when unspecified
*/
#define AOL_USE_MIMALLOCATOR_POOL_ALLOCATOR

/**
* This makes the whole AoLibrary use boost library for pool allocators
*
* - Do make sure that AOL_USE_MIMALLOCATOR_POOL_ALLOCATOR is undefined before defining this
*
* - Allocators can still be customized per container; this is the default allocator when unspecified
*/
//#define AOL_USE_BOOST_POOL_ALLOCATOR

#if defined(AOL_USE_MIMALLOCATOR_POOL_ALLOCATOR) && defined(AOL_USE_BOOST_POOL_ALLOCATOR)
#error "Only define one pool allocator type!"
#endif


/***************************************
* String Allocator
****************************************/

/**
* This makes the whole AoLibrary use mimalloc library for string allocators
* 
* - By default, we use the default allocator, std::allocator<char>
*/
//#define AOL_USE_MIMALLOCATOR_STRING_ALLOCATOR


/***************************************
* Custom Vector Types
****************************************/

/**
* This makes all AoLibrary containers using Vector use a custom vector type
*
* - By default, the library uses std::vector
*/
//#define AOL_USE_CUSTOM_VECTOR_TYPE


/***************************************
* Custom HashMap Types
****************************************/

/**
* This makes all AoLibrary containers using HashMap use a custom hash map
*
* - By default, the library already uses ankerl::unordered_dense::map
*/
#define AOL_USE_ROBINHOOD_UNORDERED_DENSE_MAP


/***************************************
* Custom HashSet Types
****************************************/

/**
* This makes all AoLibrary containers using HashSet use a custom hash set
*
* - By default, the library already uses ankerl::unordered_dense::set
*/
#define AOL_USE_ROBINHOOD_UNORDERED_DENSE_SET


/***************************************
* Custom KeyOrderMap Types
****************************************/

/**
* This makes all AoLibrary containers using KeyOrderMap use the STL key ordered map implementation
*
* - By default, the library uses absl::btree_map
*/
//#define AOL_USE_STD_KEYORDERED_MAP

/**
* This makes all AoLibrary containers using KeyOrderMap use the Boost container key ordered map implementation
*
* - By default, the library uses absl::btree_map
*/
//#define AOL_USE_BOOST_KEYORDERED_MAP

/**
* This makes all AoLibrary containers using KeyOrderMap use absl::btree_map
*
* - By default, the library uses absl::btree_map
*/
#define AOL_USE_ABSEIL_KEYORDERED_MAP

#if defined(AOL_USE_STD_KEYORDERED_MAP) && defined(AOL_USE_BOOST_KEYORDERED_MAP) && defined(AOL_USE_ABSEIL_KEYORDERED_MAP)
#error "Only define one key ordered map type!"
#endif


/***************************************
* Custom InsertOrderMap Types
****************************************/

/**
* This makes all AoLibrary containers using InsertOrderMap use tsl::ordered_map
*
* - By default, the library already uses tsl::ordered_map
*/
#define AOL_USE_TSL_INSERTORDERED_MAP


/***************************************
* Custom InsertOrderSet Types
****************************************/

/**
* This makes all AoLibrary containers using InsertOrderSet use tsl::ordered_set
*
* - By default, the library already uses tsl::ordered_set
*/
#define AOL_USE_TSL_INSERTORDERED_SET


/***************************************
* Custom KeyOrderSet Types
****************************************/
/**
* This makes all AoLibrary containers using KeyOrderSet use absl::btree_set
*
* - By default, the library already uses absl::btree_set
*/
#define AOL_USE_ABSEIL_KEYORDERED_SET


/***************************************
* Custom Hashes
****************************************/

/**
* This makes all AoLibrary containers using DefaultHash use ankerl::unordered_dense::hash
*
* - By default, the library already uses ankerl::unordered_dense::hash
*/
#define AOL_USE_UNORDERED_DENSE_HASH