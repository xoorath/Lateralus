module;

#include <atomic>

export module Lateralus.Logging.iLogger;

import <iostream>;
import <memory>;
import <mutex>;
import <optional>;
import <ostream>;
import <streambuf>;
import Lateralus.Core;
import Lateralus.Logging.Error;

using namespace std;

namespace Lateralus::Logging
{
    export class iLogger : private streambuf, public ostream
    {
    public:
        iLogger()
            : ostream(this)
            , streambuf()
            
        {
            m_Id = s_Id.fetch_add(1);
        }

        auto operator <=> (iLogger const& other) const { return (m_Id <=> other.m_Id); }

        enum class LoggerComposition { InsertNext, ReplaceNext, AddToEnd };

        optional<Error> operator |= (shared_ptr<iLogger> logger)
        {
            return ConnectLogger(logger, LoggerComposition::AddToEnd);
        }

        optional<Error> ConnectLogger(shared_ptr<iLogger> logger, LoggerComposition composition)
        {
            lock_guard lock(m_Mutex);
                
            if (!logger)
            {
                return Error("Logger missing.");
            }
            if (iLogger::operator<=>(*logger) == strong_ordering::equal)
            {
                return Error("Logger cannot add itself.");
            }
            if (LoggerExistsInChain(logger.get()) || logger->LoggerExistsInChain(this))
            {
                return Error("Logger already connected.");
            }
            
            if (m_NextLogger == nullptr)
            {
                m_NextLogger = logger;
                return {};
            }

            switch (composition)
            {
                case LoggerComposition::InsertNext:
                {
                    if (m_NextLogger)
                    {
                        logger->ConnectLogger(m_NextLogger, LoggerComposition::ReplaceNext);
                    }
                    m_NextLogger = logger;
                } break;
                case LoggerComposition::ReplaceNext:
                {
                    m_NextLogger = logger;
                } break;
                case LoggerComposition::AddToEnd:
                {
                    if (shared_ptr<iLogger> next = GetLastLogger(); next)
                    {
                        next->m_NextLogger = logger;
                    }
                } break;
            }
            return {};
        }

    protected:
        
        virtual
        bool Write(char character) = 0;

        virtual
        bool Flush()
        {
            return true;
        };

    private:
        uint32 WriteInternal(char character)
        {
            uint32 errors = 0;
            errors += Write(character) ? 0 : 1;

            auto nextLogger = m_NextLogger;
            if (nextLogger != nullptr)
            {
                errors += nextLogger->WriteInternal(character) ? 0 : 1;
            }
            return errors;
        }

        uint32 FlushInternal()
        {
            uint32 errors = 0;
            errors += Flush() ? 0 : 1;

            auto nextLogger = m_NextLogger;
            if (nextLogger != nullptr)
            {
                errors += nextLogger->FlushInternal() ? 0 : 1;
            }
            return errors;
        }
        
        int __CLR_OR_THIS_CALL /*streambuf::*/overflow(int character) override
        {
            return static_cast<int>(WriteInternal(static_cast<char>(character)));
        }

        int __CLR_OR_THIS_CALL /*streambuf::*/sync() override
        {
            return static_cast<int>(FlushInternal());
        }

        shared_ptr<iLogger> GetLastLogger() const
        {
            if (!m_NextLogger)
            {
                return nullptr;
            }
            shared_ptr<iLogger> next = m_NextLogger;
            while (true)
            {
                if (!next->m_NextLogger)
                {
                    return next;
                }
                next = next->m_NextLogger;
            }
        }

        bool LoggerExistsInChain(iLogger const* logger) const
        {
            if (logger == nullptr)
            {
                return false;
            }

            shared_ptr<iLogger> next = m_NextLogger;
            while (next)
            {
                if ((*next).operator<=>(*logger) == strong_ordering::equal)
                {
                    return true;
                }
                next = next->m_NextLogger;
            }
            return false;
        }

        shared_ptr<iLogger> m_NextLogger;
        mutex m_Mutex;
        uint32 m_Id;

        static atomic<uint32> s_Id;
    };

    atomic<uint32> iLogger::s_Id = 0;

    export using LoggerComposition = iLogger::LoggerComposition;
}



