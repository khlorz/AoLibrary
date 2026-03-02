#pragma once

/***************************************************************************************
* AoLibrary logging
****************************************************************************************
* - Logging used for AoLibrary
* - All logging functions/objects/values will be here and can only be found here
***************************************************************************************/

/*******************************************************
* spdlog tweakme.h configurations
*******************************************************/

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


/*******************************************************
* Includes
*******************************************************/

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/stopwatch.h"

#include "spdlog/sinks/base_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/callback_sink.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/ringbuffer_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/stdout_sinks.h"

namespace AoL
{

namespace Log
{

// Since I don't have anything meaningful to add for logging
// For now, we'll be using the spdlog namespace under AoL::Log
using namespace spdlog;

}

} // AoL namespace

// EOF