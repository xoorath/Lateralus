module;
#if PLATFORM_IS_AMD64 || PLATFORM_IS_X86
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#endif
#endif
export module Lateralus.Core.CPUID;
#if PLATFORM_IS_AMD64 || PLATFORM_IS_X86
import <array>;
import <bitset>;
import <string>;

import Lateralus.Core;
import Lateralus.Core.SIMDSupport;

using namespace std;

namespace Lateralus::Core
{

export struct CPUID
{
    CPUID()
    {
#if defined(_MSC_VER)
        int32 regs[4] = {0};
        int32 &eax = regs[0];
        int32 &ebx = regs[1];
        int32 &ecx = regs[2];
        int32 &edx = regs[3];
        auto cpuid = [&regs](int num) { __cpuid(regs, num); };
#elif defined(__GNUC__) || defined(__clang__)
        uint32 eax, ebx, ecx, edx;
        auto cpuid = [&eax, &ebx, &ecx, &edx](int num) {
            __get_cpuid(num, &eax, &ebx, &ecx, &edx);
        };
#else
#error "Unsupported compiler"
#endif
        cpuid(0);
        // highest function
        fn00.eax = eax;
        // ManufacturerID (not byte order: ebx, edx, ecx)
        fn00.ebx = ebx;
        fn00.edx = edx;
        fn00.ecx = ecx;

        if (fn00.HighestFunction >= 0x01)
        {
            cpuid(0x01);
            fn01.ebx = bitset<32>(ebx);
            fn01.ecx = bitset<32>(ecx);
            fn01.edx = bitset<32>(edx);
        }

        if (fn00.HighestFunction >= 0x07)
        {
            cpuid(0x07);
            fn07.ebx = bitset<32>(ebx);
            fn07.ecx = bitset<32>(ecx);
            fn07.edx = bitset<32>(edx);
        }

        cpuid(0x80000000);
        // Higest extended function
        fn80000000.eax = eax;

        if (fn80000000.HighestExtendedFunction >= 0x80000004)
        {
            ProcessorBrand.fill('\0');
            cpuid(0x80000002);
            fn80000002.eax = eax;
            fn80000002.ebx = ebx;
            fn80000002.ecx = ecx;
            fn80000002.edx = edx;
            cpuid(0x80000003);
            fn80000003.eax = eax;
            fn80000003.ebx = ebx;
            fn80000003.ecx = ecx;
            fn80000003.edx = edx;
            cpuid(0x80000004);
            fn80000004.eax = eax;
            fn80000004.ebx = ebx;
            fn80000004.ecx = ecx;
            fn80000004.edx = edx;
        }
    }

    bool HasPopcnt() const { return fn01.ecx[23]; }

    SSEVersion GetSSEVersion() const
    {
        // Currently we check for the full suite of AVX512 features, which is a pretty bad way to do
        // this. We should change these to || instead and have another enumeration for AVX512
        // features
        if (fn07.ebx[16] && fn07.ebx[17] && fn07.ebx[21] && fn07.ebx[26] && fn07.ebx[27] &&
            fn07.ebx[28] && fn07.ebx[30] && fn07.ebx[31] && fn07.ecx[11] && fn07.ecx[12] &&
            fn07.ecx[14] && fn07.edx[2] && fn07.edx[3] && fn07.edx[8] && fn07.edx[23])
        {
            return SSEVersion::AVX512;
        }

        if (fn07.ebx[5])
        {
            return SSEVersion::AVX2;
        }

        if (fn01.ecx[28])
        {
            return SSEVersion::AVX;
        }
        if (fn01.ecx[20])
        {
            return SSEVersion::SSE4_2;
        }
        if (fn01.ecx[19])
        {
            return SSEVersion::SSE4_1;
        }
        if (fn01.ecx[9])
        {
            return SSEVersion::SSSE3;
        }
        if (fn01.ecx[0])
        {
            return SSEVersion::SSE3;
        }

        if (fn01.edx[26])
        {
            return SSEVersion::SSE2;
        }
        if (fn01.edx[25])
        {
            return SSEVersion::SSE2;
        }

        return SSEVersion::None;
    }

    //////////////////////////////////////////////////////////////////////////
    // Function 0
    // https://en.wikipedia.org/wiki/CPUID#EAX=0:_Highest_Function_Parameter_and_Manufacturer_ID

    struct
    {
        /// <summary>
        /// This value corresponds with the standard functions supported on the CPU.
        /// For example if this value is less than 7, then the values in the fn07 struct will not be
        /// populated.
        /// See also: fn80000000.HighestExtendedFunction
        /// </summary>
        union {
            int32 HighestFunction = 0;
            int32 eax;
        };
        /// <summary>
        /// A non-null terminated manufacturer id string.
        /// Use GetManufacturerID for a string_view
        /// </summary>
        union {
            array<char, 12> ManufacturerID;
            struct
            {
                // Note: order is ebx, edx, ecx so ManufacturerID can represent a string of
                // characters
                int32 ebx, edx, ecx;
            };
        };
    } fn00;

    string_view GetManufacturerID() const
    {
        return string_view(fn00.ManufacturerID.data(), fn00.ManufacturerID.size());
    }

    //////////////////////////////////////////////////////////////////////////
    // Function 1 - Processor Info and Feature Bits
    // https://en.wikipedia.org/wiki/CPUID#EAX=1:_Processor_Info_and_Feature_Bits

    struct
    {
        bitset<32> ebx, ecx, edx;
    } fn01;

    //////////////////////////////////////////////////////////////////////////
    // Function 7 - Extended Features
    // https://en.wikipedia.org/wiki/CPUID#EAX=7,_ECX=0:_Extended_Features

    struct
    {
        bitset<32> ebx, ecx, edx;
    } fn07;

    //////////////////////////////////////////////////////////////////////////
    // Function 80000000 - Highest extended function implemented
    // https://en.wikipedia.org/wiki/CPUID#EAX=80000000h:_Get_Highest_Extended_Function_Implemented

    struct
    {
        /// <summary>
        /// Similar to fn00.HighestFunction this value corresponds with the highest extended
        /// function supported. For example if this value is less than 0x80000004 then
        /// fn80000002.ProcessorBrand will not be populated (as it spans fn80000002-fn80000004)
        /// See also: fn00.HighestFunction
        /// </summary>
        union {
            uint32 HighestExtendedFunction = 0;
            bitset<32> eax;
        };

        // unused
        // int32 ebx, ecx, edx;
    } fn80000000;

    //////////////////////////////////////////////////////////////////////////
    // Function 80000002,80000003,80000003 - Processor brand string
    // https://en.wikipedia.org/wiki/CPUID#EAX=80000002h,80000003h,80000004h:_Processor_Brand_String

    union {
        /// <summary>
        /// A null terminated string describing the processor.
        /// Example: "Intel(R) Core(TM) i5-1035G7 CPU @ 1.20GHz\0\0\0\0\0\0\0"
        /// </summary>
        array<char, 48> ProcessorBrand;
        struct
        {
            struct
            {
                int32 eax, ebx, ecx, edx;
            } fn80000002;
            struct
            {
                int32 eax, ebx, ecx, edx;
            } fn80000003;
            struct
            {
                int32 eax, ebx, ecx, edx;
            } fn80000004;
        };
    };
    string_view GetProcessorBrandString() const
    {
        size_t len = strlen(ProcessorBrand.data());
        return string_view(ProcessorBrand.data(), len);
    }
};

} // namespace Lateralus::Core
#endif