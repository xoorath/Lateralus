module;
#define _USE_MATH_DEFINES 1
#include <math.h>
#include <limits>
export module Lateralus.Core.Math;

import Lateralus.Core;

namespace Lateralus::Core
{
export constexpr float Pi = static_cast<float>(M_PI);
export constexpr float HalfPi = static_cast<float>(M_PI_2);
export constexpr float MachineEpsilon = std::numeric_limits<float>::epsilon();
export constexpr float Deg2Rad = 0.0174532925f;
export constexpr float Rad2Deg = 57.2957795f;

export template <typename T> constexpr T Abs(T num) { return num >= 0 ? num : -num; }
export template <typename T> constexpr T Max(T a, T b) { return a > b ? a : b; }
export template <typename T> constexpr T Max(T a, T b, T c) { return Max(Max(a, b), c); }
export template <typename T> constexpr T Min(T a, T b) { return a < b ? a : b; }
export template <typename T> constexpr T Min(T a, T b, T c) { return Min(Min(a, b), c); }

export constexpr float Clamp(float val, float minVal, float maxVal)
{
    return Max(Min(val, maxVal), minVal);
}

float WrapMinMax(float val, float minVal, float maxVal);

export constexpr float Lerp(float start, float end, float t)
{
    return start + t * (end - start);
}

export constexpr float RelativeEpsilon(float a)
{
    return MachineEpsilon * Max(1.f, Abs(a));
}

export constexpr float RelativeEpsilon(float a, float b)
{
    return MachineEpsilon * Max(1.f, Abs(a), Abs(b));
}

float Pow(float val, int power);
template <int power> float Pow(float val) { return Pow(val, power); }
template <> float Pow<2>(float val) { return val * val; }
template <> float Pow<3>(float val) { return val * val * val; }

export float Sin(float val) { return ::sin(val); }
export float Cos(float val) { return ::cos(val); }
export float ASin(float val) { return ::asin(val); }
export float ACos(float val) { return ::acos(val); }
export void SinCos(float val, float &sinOut, float &cosOut) { sinOut = Sin(val); cosOut = Cos(val); }
export void ASinACos(float val, float &asinOut, float &acosOut) { asinOut = ASin(val); acosOut = ACos(val); }
export float Pow(float val, int power) { return ::powf(val, static_cast<float>(power)); }
export float Sqrt(float val) { return ::sqrtf(val); }

// See: http://realtimecollisiondetection.net/blog/?p=89
// Example accuracy:
// CloseEnough(0.00000001f, 0.00000009f) == true
// CloseEnough(0.0000001f,  0.0000009f) == false
export constexpr bool CloseEnough(float left, float right)
{
    return Abs(left - right) <= RelativeEpsilon(left, right);
}

//////////////////////////////////////////////////////////////////////////
// Implementations that reply on above functionality

export float WrapMinMax(float val, float minVal, float maxVal)
{
    if (CloseEnough(val, minVal) || CloseEnough(val, maxVal))
    {
        return val;
    }
    else if (val > 0.f)
    {
        return fmod(val, maxVal) + minVal;
    }
    else
    {
        return maxVal - fmod(Abs(val), maxVal) + minVal;
    }
}
}