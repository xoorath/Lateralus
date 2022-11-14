#pragma once

import Lateralus.Logging;

import <string_view>;

namespace Lateralus::Logging
{
    template <typename... Args>
    void Log(Lateralus::Logging::Internal::LineInfo lineInfo, std::string_view fmt, Args&&... args) {
        Lateralus::Logging::Internal::Log(lineInfo, fmt, args...);
    }
}

#define LINE_INFO Lateralus::Logging::Internal::LineInfo(__FILE__, __LINE__)

#define LATERALUS_LOG(...) Lateralus::Logging::Log(LINE_INFO, __VA_ARGS__)

