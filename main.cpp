#include "stdafx.h"
#include <WinBase.h>

#include "RTSS.h"
#include "tools.h"

const wchar_t* usage = L"Usage:\n-p processName.exe (Case Sensitive) \n-o filename.csv (default stdout)\n-t 0 (runtime seconds, defalut=0 means no limit)\n-s close stdout (Default true)";

int main()
{
    const WCHAR* name           = DefaultProcessName;
    const char* outputFile      = DefaultOutputFile;
    bool output                 = false;
    bool outstd                 = true;
    double runTimeSeconds       = 0;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    //for (int i = 0; i < argc; i++)
    //{
    //    wprintf(L"i:%d, argv:%s\n", i, argv[i]);
    //}
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            TCHAR* arg = argv[i];
            if ( (_tcsicmp(arg, _T("-p")) == 0 || _tcsicmp(arg, _T("-P")) == 0)&& i < argc - 1)
            {
                // set process
                TCHAR* value = argv[++i];
                name = value;
            }
            else if ((_tcsicmp(arg, _T("-s")) == 0 || _tcsicmp(arg, _T("-S")) == 0) && i < argc - 1)
            {
                outstd = false;
            }
            else if ((_tcsicmp(arg, _T("-O")) == 0 || _tcsicmp(arg, _T("-o")) == 0) && i < argc - 1)
            {
                output = true;
                TCHAR* value = argv[++i];
                outputFile = ConvertTCharToConstChar(value);
                InitCsvOutput(outputFile);
            }
            else if ((_tcsicmp(arg, _T("-t")) == 0 || _tcsicmp(arg, _T("-T")) == 0) && i < argc - 1)
            {
                TCHAR* value = argv[++i];
                runTimeSeconds = std::wcstod(value, nullptr);
            }
            else
            {
                wprintf(usage);
                break;
            }
        }
        //wprintf(L"name:%s", name);
        GetRtssFrametimeData(name, output, outstd, runTimeSeconds * 1000);
    }
    else
    {
        wprintf(L"need params!\n");
        wprintf(usage);
    }
    

    LocalFree(argv);
    return 0;
}
