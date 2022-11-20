#pragma once

import Lateralus.Logging;

#define PLATFORM_LOG(...)                                   \
    ::Lateralus::Logging::Log(                              \
        ::Lateralus::Logging::LineInfo(__FILE__, __LINE__), \
        __VA_ARGS__                                         \
    )