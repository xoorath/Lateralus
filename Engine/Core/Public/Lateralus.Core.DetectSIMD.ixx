export module Lateralus.Core.SIMDSupport;

import Lateralus.Core;

#define LATERALUS_SSE_NONE 0x00
#define LATERALUS_SSE1     0x10
#define LATERALUS_SSE2     0x20
#define LATERALUS_SSE3     0x30
#define LATERALUS_SSSE3    0x31
#define LATERALUS_SSE4_1   0x41
#define LATERALUS_SSE4_2   0x42
#define LATERALUS_AVX      0x50
#define LATERALUS_AVX2     0x60
#define LATERALUS_AVX512   0x70

#if defined(_MSC_VER)
#   if defined(__AVX512__) // this define is a guess.
#       define LATERALUS_SSE_CURRENT LATERALUS_AVX512
#   elif defined(__AVX2__)
#       define LATERALUS_SSE_CURRENT LATERALUS_AVX2
#   elif defined(__AVX__)
#       define LATERALUS_SSE_CURRENT LATERALUS_AVX
#   elif defined(_M_IX86_FP)
#       if _M_IX86_FP == 1
#           define LATERALUS_SSE_CURRENT LATERALUS_SSE1
#       elif _M_IX86_FP == 2
#           define LATERALUS_SSE_CURRENT LATERALUS_SSE2
#       else
#           define LATERALUS_SSE_CURRENT LATERALUS_SSE1
#       endif
#   else
#       define LATERALUS_SSE_CURRENT LATERALUS_SSE_NONE
#       define LATERALUS_HAS_SSE 0
#   endif
#elif defined(__clang__) || defined (__GNUC__)
#   if defined(__AVX512__) || defined(__AVX512F__)
#       define LATERALUS_SSE_CURRENT LATERALUS_AVX512
#   elif defined(__AVX2__)
#       define LATERALUS_SSE_CURRENT LATERALUS_AVX2
#   elif defined(__AVX__)
#       define LATERALUS_SSE_CURRENT LATERALUS_AVX
#   elif defined(__SSE4_2__)
#       define LATERALUS_SSE_CURRENT LATERALUS_SSE4_2
#   elif defined(__SSE4_1__)
#       define LATERALUS_SSE_CURRENT LATERALUS_SSE4_1
#   elif defined(__SSSE3__)
#       define LATERALUS_SSE_CURRENT LATERALUS_SSSE3
#   elif defined(__SSE3__)
#       define LATERALUS_SSE_CURRENT LATERALUS_SSE3
#   elif defined(__SSE2__)
#       define LATERALUS_SSE_CURRENT LATERALUS_SSE2
#   elif defined(__SSE__)
#       define LATERALUS_SSE_CURRENT LATERALUS_SSE1
#else
#       define LATERALUS_SSE_CURRENT LATERALUS_SSE_NONE
#       define LATERALUS_HAS_SSE 0
#   endif
#else
#   define LATERALUS_SSE_CURRENT LATERALUS_SSE_NONE
#   define LATERALUS_HAS_SSE 0
#endif

#if !defined(LATERALUS_HAS_SSE)
#    define LATERALUS_HAS_SSE 1
#endif

namespace Lateralus::Core
{

export enum class SSEVersion : uint8
{
    None    = LATERALUS_SSE_NONE,
    SSE1    = LATERALUS_SSE1,
    SSE2    = LATERALUS_SSE2,
    SSE3    = LATERALUS_SSE3,
    SSSE3   = LATERALUS_SSSE3,
    SSE4_1  = LATERALUS_SSE4_1,
    SSE4_2  = LATERALUS_SSE4_2,
    AVX     = LATERALUS_AVX,
    AVX2    = LATERALUS_AVX2,
    AVX512  = LATERALUS_AVX512,

    CompilerCurrent = LATERALUS_SSE_CURRENT
};

export constexpr char const* SSEGetName(SSEVersion version = SSEVersion::CompilerCurrent)
{
    switch (version)
    {
    case SSEVersion::None:      return "none";
    case SSEVersion::SSE1:      return "SSE1";
    case SSEVersion::SSE2:      return "SSE2";
    case SSEVersion::SSE3:      return "SSE3";
    case SSEVersion::SSSE3:     return "SSSE3";
    case SSEVersion::SSE4_1:    return "SSE4.1";
    case SSEVersion::SSE4_2:    return "SSE4.2";
    case SSEVersion::AVX:       return "AVX";
    case SSEVersion::AVX2:      return "AVX2";
    case SSEVersion::AVX512:    return "AVX512";
    default:                    return "unknown";
    }
}

constexpr char const* SSEVersionName = SSEGetName();

}

#define LATERALUS_NEON_NONE 0x00
#define LATERALUS_NEON7     0x70

#if LATERALUS_HAS_SSE == 0
#   if defined(_MSC_VER)
#       if defined(_M_ARM)
#           define LATERALUS_NEON_CURRENT LATERALUS_NEON7
#       else
#           define LATERALUS_NEON_CURRENT LATERALUS_NEON_NONE
#           define LATERALUS_HAS_NEON 0
#       endif
#   elif defined(__clang__) || defined (__GNUC__)
#       if defined(__arm__) && defined(__ARM_NEON__)
#           define LATERALUS_NEON_CURRENT LATERALUS_NEON7
#       else
#           define LATERALUS_NEON_CURRENT LATERALUS_NEON_NONE
#           define LATERALUS_HAS_NEON 0
#       endif
#   else
#       define LATERALUS_NEON_CURRENT LATERALUS_NEON_NONE
#       define LATERALUS_HAS_NEON 0
#   endif
#else
#   define LATERALUS_NEON_CURRENT LATERALUS_NEON_NONE
#   define LATERALUS_HAS_NEON 0
#endif

#if !defined(LATERALUS_HAS_NEON)
#    define LATERALUS_HAS_NEON 1
#endif

namespace Lateralus::Core 
{
enum class NeonVersion : uint8
{
    None    = LATERALUS_NEON_NONE,
    Neon7        = LATERALUS_NEON7,

    CompilerCurrent = LATERALUS_NEON_CURRENT
};

constexpr char const* NEONGetName(NeonVersion version = NeonVersion::CompilerCurrent)
{
    switch (version)
    {
    case NeonVersion::None:     return "none";
    case NeonVersion::Neon7:    return "v7";
    default:                    return "unknown";
    }
}

constexpr char const* NEONVersionName = NEONGetName();
}