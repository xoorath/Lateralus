#pragma once
#include "Core.Log.h"

#if PLATFORM_IS_WINDOWS
#include <debugapi.h>
#endif

#include <tuple>

#if !CONF_RETAIL
#if PLATFORM_IS_WINDOWS
#define LAT_DEBUGBREAK()                                                                           \
    do                                                                                             \
    {                                                                                              \
        if (IsDebuggerPresent())                                                                   \
        {                                                                                          \
            DebugBreak();                                                                          \
        }                                                                                          \
    } while (0)
#else
#define LAT_DEBUGBREAK()                                                                           \
    do                                                                                             \
    {                                                                                              \
        __debugbreak();                                                                            \
    } while (0)
#endif
#else
#define LAT_DEBUGBREAK()
#endif

#if !CONF_RETAIL
#define LAT_ASSERT(condition)                                                                      \
    do                                                                                             \
    {                                                                                              \
        if (!((condition)))                                                                        \
        {                                                                                          \
            LAT_DEBUGBREAK();                                                                      \
        }                                                                                          \
    } while (0)
#define LAT_ASSERT_MSG(condition, ...)                                                             \
    do                                                                                             \
    {                                                                                              \
        if (!((condition)))                                                                        \
        {                                                                                          \
            LOG_CRITICAL(__VA_ARGS__);                                                             \
            LAT_DEBUGBREAK();                                                                      \
        }                                                                                          \
    } \
    while (0)
#else
#define LAT_ASSERT(condition)
#define LAT_ASSERT_MSG(condition, ...)
#endif