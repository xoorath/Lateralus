export module Lateralus.Logging;

import Lateralus.Core;

import <format>;
import <iostream>;
import <string_view>;

using namespace std;

import Lateralus.Logging.Internal;

namespace Lateralus::Logging
{
    export
    struct LineInfo
    {
        constexpr
        LineInfo(string_view file, uint32 const line)
            : File(file)
            , Line(line) {}

        string_view const File;
        uint32 const Line;
    };

    export
    struct LineInfoCategory
    {
        constexpr
        LineInfoCategory(string_view category, string_view file, uint32 const line)
            : Category(category)
            , File(file)
            , Line(line) {}

        string_view const Category;
        string_view const File;
        uint32 const Line;
    };

    export
    template <typename... Args>
    void Log(LineInfo const&& ctx, string_view fmt, Args&&... args)
    {
        cout << std::format("{}({}): {}\n",
            PathRelativeToLateralus(ctx.File),
            ctx.Line,
            vformat(fmt, make_format_args(args...))
        );
    }

    export
    template <typename... Args>
    void Log(LineInfoCategory const&& ctx, string_view fmt, Args&&... args)
    {
        cout << std::format("{}({}) [{}]: {}\n",
            PathRelativeToLateralus(ctx.File),
            ctx.Line,
            ctx.Category,
            vformat(fmt, make_format_args(args...))
        );
    }
}