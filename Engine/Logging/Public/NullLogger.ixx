export module Lateralus.Logging.NullLogger;

import Lateralus.Logging.iLogger;
import <ostream>;
import <streambuf>;

using namespace std;

namespace Lateralus::Logging
{
    export class NullLogger : public iLogger
    {
    public:
        bool Write(char c) override
        {
            return true;
        }
    };
}