export module Lateralus.Logging.ConsoleLogger;

import Lateralus.Logging.RedirectLogger;
import <iostream>;
import <ostream>;
import <streambuf>;

using namespace std;

namespace Lateralus::Logging
{
    export class CoutLogger : public RedirectLogger
    {
    public:
        CoutLogger() : RedirectLogger(cout) { }
    };

    export class CerrLogger : public RedirectLogger
    {
    public:
        CerrLogger() : RedirectLogger(cerr) { }
    };
}