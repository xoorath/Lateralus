export module Lateralus.Core.Matrix;

import Lateralus.Core.Vector;

namespace Lateralus::Core
{
    export struct Matrix4x4
    {
        Matrix4x4()
        : r0(1.0f, 0.0f, 0.0f, 0.0f)
        , r1(0.0f, 1.0f, 0.0f, 0.0f)
        , r2(0.0f, 0.0f, 1.0f, 0.0f)
        , r3(0.0f, 0.0f, 0.0f, 1.0f)
        {
        }
        Vector4 r0, r1, r2, r3;
    };
}