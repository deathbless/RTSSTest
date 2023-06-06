#pragma once
#include "stdafx.h"
#include <tlhelp32.h>

#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <string>
#include <locale>

// func
std::vector<DWORD> GetProcessIdByName(const WCHAR* name);

double getTimeInmiliseconds();

bool isProcessInList(DWORD processID, std::vector<DWORD>& processIDList);

DWORD GetUnixTimestamp();

WCHAR* ConvertConstCharToWChar(const char* inputStr);

const char* ConvertTCharToConstChar(TCHAR* inputStr);

bool IsKeyInMap(DWORD key, std::map<DWORD, DWORD> m);