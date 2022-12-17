#pragma once

// Header for global utilities with no dependencies (except core).

import Lateralus.Core;

// Easy encapsulation. Standard get/set.
#define ENCAPSULATE(type, name)                                                                    \
public:                                                                                            \
    type Get##name() const                                                                         \
    {                                                                                              \
        return m_##name;                                                                           \
    }                                                                                              \
    void Set##name(type const &name)                                                               \
    {                                                                                              \
        m_##name = name;                                                                           \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    type m_##name

// encapsulates an optional value with a default so a value is always available
#define ENCAPSULATE_O(type, name, defaultValue)                                                    \
public:                                                                                            \
    type Get##name() const                                                                         \
    {                                                                                              \
        return m_##name.has_value() ? m_##name.value() : defaultValue;                             \
    }                                                                                              \
    void Set##name(type const &name)                                                               \
    {                                                                                              \
        m_##name = name;                                                                           \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    std::optional<type> m_##name

// V = "set by value"
#define ENCAPSULATE_V(type, name)                                                                  \
public:                                                                                            \
    type Get##name() const                                                                         \
    {                                                                                              \
        return m_##name;                                                                           \
    }                                                                                              \
    void Set##name(type paramName)                                                                 \
    {                                                                                              \
        m_##name = paramName;                                                                      \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    type m_##name

// RO = "read only"
#define ENCAPSULATE_RO(type, name)                                                                 \
public:                                                                                            \
    type Get##name() const                                                                         \
    {                                                                                              \
        return m_##name;                                                                           \
    }                                                                                              \
                                                                                                   \
protected:                                                                                         \
    void Set##name(type const &name)                                                               \
    {                                                                                              \
        m_##name = name;                                                                           \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    type m_##name

// RO = "read only"
// V = "set by value"
#define ENCAPSULATE_RO_V(type, name)                                                               \
public:                                                                                            \
    type Get##name() const                                                                         \
    {                                                                                              \
        return m_##name;                                                                           \
    }                                                                                              \
                                                                                                   \
protected:                                                                                         \
    void Set##name(type paramName)                                                                 \
    {                                                                                              \
        m_##name = paramName;                                                                      \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    type m_##name

// Encapsulates a Lateralus signal.
// Usage:
//      // In owning class Foo
//      using KeyCallback = Signal<void(int)>;
//      ENCAPSULATE_SIGNAL(KeyCallback, KeyCallback)
//
//      // At subscription site
//      foo.GetKeyCallback() += [](int key){ };
#define ENCAPSULATE_SIGNAL(type, name)                                                             \
public:                                                                                            \
    type::SignalSubscribe &Get##name()                                                             \
    {                                                                                              \
        return m_##name;                                                                           \
    }                                                                                              \
                                                                                                   \
protected:                                                                                         \
    type m_##name;

namespace Lateralus::Core::Internal
{
template <size_t S> struct EnumIntSized;
template <> struct EnumIntSized<1>
{
    using type = Lateralus::Core::int8;
};
template <> struct EnumIntSized<2>
{
    using type = Lateralus::Core::int16;
};
template <> struct EnumIntSized<4>
{
    using type = Lateralus::Core::int32;
};
template <class T> struct EnumInt
{
    using type = EnumIntSized<sizeof(T)>::type;
};
} // namespace Lateralus::Core::Internal

#define ENUM_FLAG_OPERATORS(EnumType)                                                              \
    constexpr inline EnumType operator|(EnumType a, EnumType b)                                    \
    {                                                                                              \
        return static_cast<EnumType>(                                                              \
            static_cast<Lateralus::Core::Internal::EnumInt<EnumType>::type>(a) |                   \
            static_cast<Lateralus::Core::Internal::EnumInt<EnumType>::type>(b));                   \
    }                                                                                              \
    constexpr inline EnumType operator&(EnumType a, EnumType b)                                    \
    {                                                                                              \
        return static_cast<EnumType>(                                                              \
            static_cast<Lateralus::Core::Internal::EnumInt<EnumType>::type>(a) &                   \
            static_cast<Lateralus::Core::Internal::EnumInt<EnumType>::type>(b));                   \
    }                                                                                              \
    constexpr inline EnumType operator^(EnumType a, EnumType b)                                    \
    {                                                                                              \
        return static_cast<EnumType>(                                                              \
            static_cast<Lateralus::Core::Internal::EnumInt<EnumType>::type>(a) ^                   \
            static_cast<Lateralus::Core::Internal::EnumInt<EnumType>::type>(b));                   \
    }                                                                                              \
    constexpr inline EnumType operator~(EnumType a)                                                \
    {                                                                                              \
        return static_cast<EnumType>(                                                              \
            ~static_cast<Lateralus::Core::Internal::EnumInt<EnumType>::type>(a));                  \
    }
