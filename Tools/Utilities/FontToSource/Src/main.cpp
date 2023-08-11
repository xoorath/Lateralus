#include <assert.h>
#include <format>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;
using namespace std::string_view_literals;

static string g_AppName = "FontToSource.exe";

bool binary_to_compressed_c(string_view inputParam, string_view outputParam,
                            string_view symbolParam, string_view const namespaceParam);

void PrintHelp()
{
    cout << g_AppName << " [-help|?]"
         << "\n";
    cout << g_AppName << " -i input [-o output][-s symbol_name][-n namespace]\n";
    cout << setw(24) << left << "    -help|?"
         << ": prints this help text then exits.\n";
    cout << setw(24) << left << "    -i input"
         << ": input font file\n";
    cout << setw(24) << left << "    -o output"
         << ": output file (ie: name.cpp or name.ixx; Default: input name .cpp)\n";
    cout << setw(24) << left << "    -s symbol"
         << ": symbol name. (Default: output file name before the extension.)\n";
    cout << setw(24) << left << "    -n namespace"
         << ": namespace name for the symbol (ie: root::inner)\n";
    cout << endl;
}

int main(int argc, char **argv)
{
    g_AppName = argv[0];
    g_AppName = g_AppName.substr(g_AppName.find_last_of("/\\") + 1);

    vector<string_view> params;
    params.reserve(argc - 1);
    for (int i = 1; i < argc; ++i)
    {
        params.emplace_back(argv[i]);
    }

    for (auto const &param : params)
    {
        if (param == "-help"sv || param == "?")
        {
            PrintHelp();
            return 0;
        }
    }

    if (params.size() == 0)
    {
        PrintHelp();
        return 0;
    }

    string_view inputParam, outputParam, symbolParam, namespaceParam;

    for (int i = 0; i < params.size() - 1; ++i)
    {
        if (params[i] == "-i"sv)
        {
            if (!inputParam.empty())
            {
                cout << "[warning] input (-i) param already provided." << endl;
            }
            inputParam = params[++i];
        }
        else if (params[i] == "-o"sv)
        {
            if (!outputParam.empty())
            {
                cout << "[warning] output (-o) param already provided." << endl;
            }
            outputParam = params[++i];
        }
        else if (params[i] == "-s"sv)
        {
            if (!symbolParam.empty())
            {
                cout << "[warning] symbol (-s) param already provided." << endl;
            }
            symbolParam = params[++i];
        }
        else if (params[i] == "-n"sv)
        {
            if (!namespaceParam.empty())
            {
                cout << "[warning] namespace (-n) param already provided." << endl;
            }
            namespaceParam = params[++i];
        }
    }

    if (binary_to_compressed_c(inputParam, outputParam, symbolParam, namespaceParam))
    {
        return 0;
    }

    return 1;
}