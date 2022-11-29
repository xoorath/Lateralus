#pragma once

// Header for global utilities with no dependencies 

// Easy encapsulation. Standard get/set.
#define ENCAPSULATE(type, name) \
    public:\
        type Get##name() const { return m_##name; } \
        void Set##name(type const& name) { m_##name = name; } \
    private: \
        type m_##name

// encapsulates an optional value with a default so a value is always available
#define ENCAPSULATE_O(type, name, defaultValue) \
    public:\
        type Get##name() const { return m_##name .has_value() ? m_##name .value() : defaultValue; } \
        void Set##name(type const& name) { m_##name = name; } \
    private: \
        std::optional<type> m_##name

// V = "set by value"
#define ENCAPSULATE_V(type, name) \
    public:\
        type Get##name () const { return m_##name; } \
        void Set##name (type paramName) { m_##name = paramName; } \
    private: \
        type m_##name

// RO = "read only"
#define ENCAPSULATE_RO(type, name) \
    public:\
        type Get##name() const { return m_##name; } \
    protected: \
        void Set##name(type const& name) { m_##name = name; } \
    private: \
        type m_##name

// RO = "read only"
// V = "set by value"
#define ENCAPSULATE_RO_V(type, name) \
    public:\
        type Get##name () const { return m_##name; } \
    protected: \
        void Set##name (type paramName) { m_##name = paramName; } \
    private: \
        type m_##name

namespace Lateralus
{
    template <size_t S>
    struct EnumIntSized;

    template <>
    struct EnumIntSized<1>
    {
        using type = signed char;
    };

    template <>
    struct EnumIntSized<2>
    {
        using type = signed short;
    };

    template <>
    struct EnumIntSized<4>
    {
        using type = signed int;
    };

    template <class T>
    struct EnumInt
    {
        typedef typename EnumIntSized<sizeof(T)>::type type;
    };
}

#define ENUM_FLAG_OPERATORS(EnumType) \
constexpr inline EnumType operator | (EnumType a, EnumType b)    { return static_cast<EnumType>(static_cast<Lateralus::EnumInt<EnumType>::type>(a) | static_cast<Lateralus::EnumInt<EnumType>::type>(b)); } \
constexpr inline EnumType operator & (EnumType a, EnumType b)    { return static_cast<EnumType>(static_cast<Lateralus::EnumInt<EnumType>::type>(a) & static_cast<Lateralus::EnumInt<EnumType>::type>(b)); } \
constexpr inline EnumType operator ^ (EnumType a, EnumType b)    { return static_cast<EnumType>(static_cast<Lateralus::EnumInt<EnumType>::type>(a) ^ static_cast<Lateralus::EnumInt<EnumType>::type>(b)); } \
constexpr inline EnumType operator ~ (EnumType a)                { return static_cast<EnumType>(~static_cast<Lateralus::EnumInt<EnumType>::type>(a)); } 