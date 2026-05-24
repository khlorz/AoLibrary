#pragma once

/********************************************************************
* Tests configuration
********************************************************************/

// Change to 1 if you want to run all tests
#define AOL_TESTS_ALL_TESTS_ENABLED_FLAG 0

// Change to 1 if you want to disable all tests
#define AOL_TESTS_ALL_TESTS_DISABLED_FLAG 0

#if (AOL_TESTS_ALL_TESTS_ENABLED_FLAG == 1) && (AOL_TESTS_ALL_TESTS_DISABLED_FLAG == 1)
#error "Both flags cannot be on at the same time!"
#endif

// Change to 1 if you want a specific test to run
// No effect if AOL_TESTS_ALL_TESTS_ENABLED_FLAG is 1 or AOL_TESTS_ALL_TESTS_DISABLED_FLAG is 1
#define AOL_TESTS_BASIC_DATA_TESTS_FLAG 0
#define AOL_TESTS_GENERIC_MANAGER_TESTS_FLAG 0
#define AOL_TESTS_PARTITION_TESTS_FLAG 0
#define AOL_TESTS_SERIALIZATION_TESTS_FLAG 1

#define AOL_TESTS_FLAG(flag_name) !AOL_TESTS_ALL_TESTS_DISABLED_FLAG && (AOL_TESTS_ALL_TESTS_ENABLED_FLAG || AOL_TESTS_##flag_name##_TESTS_FLAG)
#define AOL_TESTS_DECLARATION_ON(test_name) void test_name##Tests() noexcept;
#define AOL_TESTS_DECLARATION_OFF(test_name) inline void test_name##Tests() { }