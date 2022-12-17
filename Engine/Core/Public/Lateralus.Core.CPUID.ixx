module;
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#endif
export module Lateralus.Core.CPUID;

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
        int regs[4] = {0};

        // Execute CPUID with function ID 0 to get the maximum input value
        __cpuid(regs, 0);
        fn0.MaxInputValue = regs[0];

        memcpy(fn0.ManufacturerID.data(), &regs[1], fn0.ManufacturerID.size()-1);
        fn0.ManufacturerID[12] = '\0';

        // Execute CPUID with function ID 1 to get the feature flags
        __cpuid(regs, 1);
        fn1.ebx = bitset<32>(regs[1]);
        fn1.ecx = bitset<32>(regs[2]);
        fn1.edx = bitset<32>(regs[3]);

        // Execute CPUID with function ID 7 to get the extended feature flags
        __cpuid(regs, 7);
        fn7.ebx = bitset<32>(regs[1]);
        fn7.ecx = bitset<32>(regs[2]);
        fn7.edx = bitset<32>(regs[3]);
#elif defined(__GNUC__) || defined(__clang__)
        uint32 eax, ebx, ecx, edx;
        // Execute CPUID with function ID 0 to get the maximum input value
        __get_cpuid(0, &eax, &ebx, &ecx, &edx);
        fn0.MaxInputValue = eax;
        memcpy(fn0.ManufacturerID.data(), &ebx, 4);
        memcpy(fn0.ManufacturerID.data()+4, &ecx, 4);
        memcpy(fn0.ManufacturerID.data()+8, &edx, 4);

        // Execute CPUID with function ID 1 to get the feature flags
        __get_cpuid(1, &eax, &ebx, &ecx, &edx);
        fn1.ebx = bitset<32>(ebx);
        fn1.ecx = bitset<32>(ecx);
        fn1.edx = bitset<32>(edx);
        // Execute CPUID with function ID 7 to get the extended feature flags
        __get_cpuid(7, &eax, &ebx, &ecx, &edx);
        fn7.ebx = bitset<32>(ebx);
        fn7.ecx = bitset<32>(ecx);
        fn1.edx = bitset<32>(edx);
#else
#   error "Unsupported compiler"
#endif
    }


    bool HasPopcnt() const { return fn1.ecx[23]; }

    // Always false on physical CPUs. 
    // This should only be used for telemetry. The value is easily spoofed if your intent is to
    // deny the game running in a virtual machine.
    bool IsHypervisor() const {return fn1.ecx[31];}

    SSEVersion GetSSEVersion() const
    {
        if (fn7.ebx[16] && fn7.ebx[17] && fn7.ebx[21] && fn7.ebx[26] && fn7.ebx[27] &&
            fn7.ebx[28] && fn7.ebx[30] && fn7.ebx[31] && fn7.ecx[11] && fn7.ecx[12] &&
            fn7.ecx[14] && fn7.edx[2] && fn7.edx[3] && fn7.edx[8] && fn7.edx[23])
        {
            return SSEVersion::AVX512;
        }

        if (fn7.ebx[5]) {
            return SSEVersion::AVX2;
        }

        if (fn1.ecx[28]) {
            return SSEVersion::AVX;
        }
        if (fn1.ecx[20]) {
            return SSEVersion::SSE4_2;
        }
        if (fn1.ecx[19])
        {
            return SSEVersion::SSE4_1;
        }
        if (fn1.ecx[9])
        {
            return SSEVersion::SSSE3;
        }
        if (fn1.ecx[0])
        {
            return SSEVersion::SSE3;
        }

        if (fn1.edx[26])
        {
            return SSEVersion::SSE2;
        }
        if (fn1.edx[25])
        {
            return SSEVersion::SSE2;
        }

        return SSEVersion::None;

    }


    struct
    {
        uint32 MaxInputValue = 0;
        array<char, 13> ManufacturerID;
    } fn0;

    struct
    {
        bitset<32> ebx, ecx, edx;
    } fn1;

    struct
    {
        bitset<32> ebx, ecx, edx;
    } fn7;
};

}