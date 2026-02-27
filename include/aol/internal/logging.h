#pragma once

#include "spdlog/tweakme.h"

// For the most part, the author uses this on Windows
// Important for dealing with wchars in Windows
#ifndef SPDLOG_WCHAR_FILENAMES
#error "Must define SPDLOG_WCHAR_FILENAMES"
#endif

// Dealing with wchars
// Conversion will be needed
#ifndef SPDLOG_WCHAR_TO_UTF8_SUPPORT
#error "Must define SPDLOG_WCHAR_TO_UTF8_SUPPORT"
#endif

// Since the library is using fmt already
// It's better that we use an external one instead
#ifndef SPDLOG_FMT_EXTERNAL
#error "Must define SPDLOG_FMT_EXTERNAL"
#endif

// Function name logging
// Better to have a function name when logging in my case
#ifndef SPDLOG_FUNCTION
#error "Must define SPDLOG_FUNCTION"
#endif




// EOF