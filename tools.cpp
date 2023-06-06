#include "tools.h"

std::vector<DWORD> GetProcessIdByName(const WCHAR* name)
{
    std::vector<DWORD> res;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 entry = { 0 };
        entry.dwSize = sizeof(entry);
        if (Process32First(snapshot, &entry)) {
            do {

                if (wcscmp(entry.szExeFile, name) == 0) {
                    res.push_back(entry.th32ProcessID);
                }
            } while (Process32Next(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }
    return res;
}

double getTimeInmiliseconds()
{
    LARGE_INTEGER freq, tick;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&tick);
    return (double)tick.QuadPart * 1000 / (double)freq.QuadPart;
}

bool isProcessInList(DWORD processID, std::vector<DWORD>& processIDList)
{
    auto it = std::find(processIDList.begin(), processIDList.end(), processID);
    return it != processIDList.end();
}

DWORD GetUnixTimestamp()
{
    time_t unix_timestamp = time(NULL);
    return (DWORD)unix_timestamp;
}

WCHAR* ConvertConstCharToWChar(const char* inputStr)
{
    int strLength = MultiByteToWideChar(CP_UTF8, 0, inputStr, -1, NULL, 0);
    WCHAR* outputStr = new WCHAR[strLength];
    MultiByteToWideChar(CP_UTF8, 0, inputStr, -1, outputStr, strLength);
    return outputStr;
}

const char* ConvertTCharToConstChar(TCHAR* inputStr)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, inputStr, -1, NULL, 0, NULL, NULL); // Get required buffer size
    char* outputStr = new char[bufferSize];
    WideCharToMultiByte(CP_UTF8, 0, inputStr, -1, outputStr, bufferSize, NULL, NULL); // Convert the string
    return outputStr;
}

bool IsKeyInMap(DWORD key, std::map<DWORD, DWORD> m)
{
    if (m.find(key) != m.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}
