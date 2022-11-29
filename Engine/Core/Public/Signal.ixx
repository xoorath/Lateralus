export module Lateralus.Core.Signal;

import <functional>;
import <list>;
import <mutex>;
import Lateralus.Core;

using namespace std;

namespace Lateralus
{

    export
    template<typename T>
    class iSignalSubscribe
    {
    public:
        using Delegate = function<T>;
        using Token = list<Delegate>::iterator;

        virtual Token Add(Delegate func) = 0;
        virtual bool Remove(Token& token) = 0;

        Token operator += (function<T> func)
        {
            return Add(func);
        }

        bool operator -= (Token& token)
        {
            return Remove(token);
        }
    };

    export 
    template<typename T>
    class Signal : public iSignalSubscribe<T>
    {
    public:
        using SignalSubscribe = iSignalSubscribe<T>;
        using Delegate = function<T>;
        using Token = list<Delegate>::iterator;

        Token Add(Delegate func) override
        {
            lock_guard lock(m_Mutex);
            m_Functions.push_back(func);
            return --m_Functions.end();
        }

        bool Remove(Token& token) override
        {
            lock_guard lock(m_Mutex);
            if (token != m_Functions.end())
            {
                m_Functions.erase(token);
                token = m_Functions.end();
                return true;
            }
            return false;
        }

        template<typename... Args>
        void Invoke(Args&&... args) const
        {
            lock_guard lock(m_Mutex);
            for (auto const& func : m_Functions)
            {
                func(forward<Args>(args)...);
            }
        }

        template<typename... Args>
        void operator()(Args&&... args) const
        {
            Invoke(std::forward<Args>(args)...);
        }

    private:
        mutable mutex m_Mutex;
        list<Delegate> m_Functions;
    };
}