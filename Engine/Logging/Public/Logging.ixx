export module Lateralus.Logging;

import Lateralus.Core;

import <chrono>;
import <format>;
import <iostream>;
import <string_view>;

namespace Lateralus::Logging::Internal
{
    export
    struct LineInfo
    {
        LineInfo(std::string_view file, uint32 const line)
            : file(file)
            , line(line) {}

        std::string_view file;
        uint32 const line;
    };

    export
    template <typename... Args>
    void Log(LineInfo lineInfo, std::string_view fmt, Args&&... args) {
        std::cout << lineInfo.file << "(" << lineInfo.line << "): " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
    }
}