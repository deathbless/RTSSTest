#include "RTSS.h"
#include <iostream>
#include <fstream>
using namespace std;
ofstream out;

void InitCsvOutput(const char* outputFile)
{
	out.open(outputFile);
	out << "ProcessName,Unixtime,ProcessID,ExeApi,FrameTime,FrameRate,FrameMemPos,TimeInMs" << endl;
}

void WriteCsv(char* string)
{
	out << string << endl;
}

void CloseCsv()
{
	out.close();
}