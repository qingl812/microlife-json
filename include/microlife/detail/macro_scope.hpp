#pragma once

/***
 * @brief macro_scope.h
 * @details This file contains all internal macro definitions
 * You MUST include macro_unscope.h at the end of json.hpp to undef all
 * of them
 * @author qingl
 * @date 2022_04_14
 */

// allow overriding assert
#if !defined(JSON_ASSERT)
#include <cassert> // assert
#define JSON_ASSERT(x) assert(x)
#endif

// allow to access some private functions (needed by the test suite)
#if defined(JSON_TESTS_PRIVATE)
#define JSON_PRIVATE_UNLESS_TESTED public:
#else
#define JSON_PRIVATE_UNLESS_TESTED private:
#endif
