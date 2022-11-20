export module Lateralus.Logging.FileLogger;

import Lateralus.Logging.RedirectLogger;
import Lateralus.Logging.Error;

import <filesystem>;
import <format>;
import <fstream>;
import <optional>;
import <ostream>;
import <streambuf>;

using namespace std;
using namespace std::filesystem;

namespace Lateralus::Logging
{
    export class FileLogger : public RedirectLogger
    {
    public:
        FileLogger() : RedirectLogger(m_FileStream)
        {
        }

        optional<Error> Open(path path, ios_base::openmode openMode = ios_base::out)
        {
            if ((openMode & ios_base::in) == ios_base::in)
            {
                return Error("Can't open a log file in read mode");
            }
            m_FileStream.open(path, openMode);

            if (m_FileStream.fail())
            {
                return Error(std::format("FileLogger failed to open a stream. rdstate: {}", m_FileStream.rdstate()));
            }

            return {};
        }

    private:
        ofstream m_FileStream;
    };
}