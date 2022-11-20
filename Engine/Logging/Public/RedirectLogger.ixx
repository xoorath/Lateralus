export module Lateralus.Logging.RedirectLogger;

import Lateralus.Logging.iLogger;
import <ostream>;
import <streambuf>;

using namespace std;

namespace Lateralus::Logging
{
    export class RedirectLogger : public iLogger
    {
    public:
        // Stream lifetime must be longer than RedirectLogger
        RedirectLogger(ostream& stream)
            : iLogger()
            , m_ChildStream(stream)
        {
        }

        bool Write(char c) override
        {
            m_ChildStream.put(c);
            return !m_ChildStream.fail();
        }

        bool Flush() override
        {
            m_ChildStream.flush();
            return !m_ChildStream.fail();
        }

    private:
        ostream& m_ChildStream;
    };
}