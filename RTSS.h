#include "StdAfx.h"
#include "RTSSSharedMemory.h"
#include <vector>
#include <map>

typedef std::map<DWORD, WCHAR*> ProcessApiMap;
typedef std::map<DWORD, DWORD> ProcessPosMap;
typedef std::vector<DWORD> ProcessIDList;

// func
void RefreshProcessApi(ProcessIDList& processIDList, ProcessApiMap& processApiMap);

void GetRtssFrametimeData(const WCHAR* name, bool outputToFile, bool outputTostd, double runTime);

WCHAR* GetApiInfo(DWORD processID);

void InitCsvOutput(const char* outputFile);

void WriteCsv(char* string);

void CloseCsv();

// const
const double REFRESH_INTERVAL_SECONDS = 1.0;
const char DefaultOutputFile[] = "output.csv";
const WCHAR DefaultProcessName[] = L"FurMark.exe";
const int MAX_LEN = 1000;