export module Lateralus.Core.Signal;
import <functional>;
import <list>;
import <mutex>;
import Lateralus.Core;

using namespace std;

namespace Lateralus::Core
{
export template <typename T> class iSignalSubscribe
{
public:
    using Delegate = function<T>;
    using Token = list<Delegate>::iterator;

    virtual Token Add(Delegate func) = 0;
    virtual bool Remove(Token &token) = 0;

    Token operator+=(function<T> func) { return Add(func); }

    bool operator-=(Token &token) { return Remove(token); }
};

// Can be used as the LockType template parameter of a signal
// When you do - the signal will use a mutex to protect it's function list.
export struct SignalLockMutex
{
    auto Lock() { return lock_guard(m_Mutex); }
    mutex m_Mutex;
};

// Can be used as the LockType template parameter of a signal
// When you do - the signal is lockless.
export struct SignalLockNull
{
    auto Lock() { return 0; }
};

export template <typename T, typename LockType = SignalLockMutex>
class Signal : public iSignalSubscribe<T>
{
public:
    using SignalSubscribe = iSignalSubscribe<T>;
    using Delegate = function<T>;
    using Token = list<Delegate>::iterator;

    Token Add(Delegate func) override
    {
        auto lock = m_Lock.Lock();
        m_Functions.push_back(func);
        return --m_Functions.end();
    }

    bool Remove(Token &token) override
    {
        auto lock = m_Lock.Lock();
        if (token != m_Functions.end())
        {
            m_Functions.erase(token);
            token = m_Functions.end();
            return true;
        }
        return false;
    }

    template <typename... Args> void Invoke(Args &&...args) const
    {
        auto lock = m_Lock.Lock();
        for (auto const &func : m_Functions)
        {
            func(forward<Args>(args)...);
        }
    }

    template <typename... Args> void operator()(Args &&...args) const
    {
        auto lock = m_Lock.Lock();
        for (auto const &func : m_Functions)
        {
            func(forward<Args>(args)...);
        }
    }

private:
    mutable LockType m_Lock;
    list<Delegate> m_Functions;
};
} // namespace Lateralus