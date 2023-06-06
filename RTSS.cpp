#include "RTSS.h"
#include "tools.h"

void GetRtssFrametimeData(const WCHAR* name,bool outputToFile, bool outputTostd,double runTime) {
    wprintf(L"### START WATCHING PROCESS:%s RUNTIME(MS):%f ###\n", name, runTime);
    HANDLE hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");
    float currentFramerate = 0;
    float currentFrametime = 0;
    bool  RTSSFlag = false;

    double start = getTimeInmiliseconds();
    double pass  = 0.0;
    double refreshTime = 0.0;
    ProcessIDList processIDList = GetProcessIdByName(name);
    ProcessApiMap processApiMap;
    RefreshProcessApi(processIDList, processApiMap);
    

    // if pos changed, one frame is completed, need to output frametime & framerate
    // every process need it's own pos
    ProcessPosMap lastFrameTimePos;
    ProcessPosMap nowFrameTimePos;

    for (auto pid : processIDList) {
        lastFrameTimePos[pid] = 0;
    }

    if (hMapFile) {
        LPVOID pMapAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        LPRTSS_SHARED_MEMORY pMem = (LPRTSS_SHARED_MEMORY)pMapAddr;
        if (pMem)
        {
            if ((pMem->dwSignature == 'RTSS') &&
                (pMem->dwVersion >= 0x00020000))
            {
                RTSSFlag = true;
                while (true)
                {
                    pass = getTimeInmiliseconds() - start;
                    for (DWORD dwEntry = 0; dwEntry < pMem->dwAppArrSize; dwEntry++)
                    {
                        RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_APP_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_APP_ENTRY)((LPBYTE)pMem + pMem->dwAppArrOffset + dwEntry * pMem->dwAppEntrySize);
                        DWORD pid = pEntry->dwProcessID;
                        if (isProcessInList(pid, processIDList))
                        {
                            DWORD nowPos = pEntry->dwStatFrameTimeBufPos;
                            nowFrameTimePos[pid] = nowPos;
                            // new process need init pos
                            if (!IsKeyInMap(pid, lastFrameTimePos))
                            {
                                lastFrameTimePos[pid] = 0;
                            }
                            if (lastFrameTimePos[pid] == 0) {
                                lastFrameTimePos[pid] = nowFrameTimePos[pid];
                            }
                            else {
                                // if multi process, need another way
                                if (nowFrameTimePos[pid] == lastFrameTimePos[pid]) {
                                    continue;
                                }
                            }
                            lastFrameTimePos[pid] = nowFrameTimePos[pid];
                            currentFrametime = pEntry->dwStatFrameTimeBuf[(nowFrameTimePos[pid] - 1) & 1023] / 1000.0f;
                            currentFramerate = pEntry->dwStatFrameTimeBufFramerate / 10.0f;
                            if (outputToFile)
                            {
                                char line[MAX_LEN] = { 0 };
                                sprintf_s(line, MAX_LEN, "\"%S\",%d,%d,%S,%f,%f,%d,%f", name, GetUnixTimestamp(), pid, processApiMap[pid], currentFrametime, currentFramerate, nowFrameTimePos[pid], pass);
                                WriteCsv(line);
                            }
                            if (outputTostd) // stdout
                            {
                                wprintf(L"ProcessName:\"%s\",Unixtime:%d,ProcessID:%d,ExeApi:%s,FrameTime:%f,FrameRate:%f,FrameMemPos:%d,TimeInMs:%f\n", name, GetUnixTimestamp(), pid, processApiMap[pid], currentFrametime, currentFramerate, nowFrameTimePos[pid], pass);
                            }

                        }
                    }
                    // after a few time, refresh processList
                    if (pass - refreshTime > REFRESH_INTERVAL_SECONDS * 1000)
                    {
                        processIDList = GetProcessIdByName(name);
                        RefreshProcessApi(processIDList, processApiMap);
                        refreshTime = pass;
                    }
                    // runtime is not zero, need break
                    if (runTime != 0 && runTime - pass <= 0)
                    {
                        CloseCsv();
                        break;
                    }
                }
            }
        }
    }
    if (!RTSSFlag)
    {
        printf("please install & run RTSS process!\n");
        exit(-1);
    }
}

void RefreshProcessApi(ProcessIDList& pidList, ProcessApiMap& processApiMap)
{
    for (auto pid : pidList)
    {
        processApiMap[pid] = GetApiInfo(pid);
    }
}

WCHAR* GetApiInfo(DWORD processID)
{
    const char* api = "unknown";
    HANDLE hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

    if (hMapFile)
    {
        LPVOID pMapAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        LPRTSS_SHARED_MEMORY pMem = (LPRTSS_SHARED_MEMORY)pMapAddr;

        if (pMem)
        {
            if ((pMem->dwSignature == 'RTSS') && (pMem->dwVersion >= 0x00020000))
            {
                for (DWORD dwEntry = 0; dwEntry < pMem->dwAppArrSize; dwEntry++)
                {
                    RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_APP_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_APP_ENTRY)((LPBYTE)pMem + pMem->dwAppArrOffset + dwEntry * pMem->dwAppEntrySize);

                    if (pEntry->dwProcessID == processID)
                    {
                        api = (pEntry->dwFlags & APPFLAG_API_USAGE_MASK) == APPFLAG_OGL ? "OpenGL"
                            : (pEntry->dwFlags & APPFLAG_API_USAGE_MASK) == APPFLAG_DD ? "DirectDraw"
                            : (pEntry->dwFlags & APPFLAG_API_USAGE_MASK) == APPFLAG_D3D8 ? "DX8"
                            : (pEntry->dwFlags & APPFLAG_API_USAGE_MASK) == APPFLAG_D3D9 ? "DX9"
                            : (pEntry->dwFlags & APPFLAG_API_USAGE_MASK) == APPFLAG_D3D9EX ? "DX9 EX"
                            : (pEntry->dwFlags & APPFLAG_API_USAGE_MASK) == APPFLAG_D3D10 ? "DX10"
                            : (pEntry->dwFlags & APPFLAG_API_USAGE_MASK) == APPFLAG_D3D11 ? "DX11"
                            : (pEntry->dwFlags & APPFLAG_API_USAGE_MASK) == APPFLAG_D3D12 ? "DX12"
                            : (pEntry->dwFlags & APPFLAG_API_USAGE_MASK) == APPFLAG_D3D12AFR ? "DX12 AFR"
                            : (pEntry->dwFlags & APPFLAG_API_USAGE_MASK) == APPFLAG_VULKAN ? "Vulkan"
                            : "unknown";
                        
                        break;
                    }
                }
                UnmapViewOfFile(pMapAddr);
            }
            CloseHandle(hMapFile);
        }
    }
    else
    {
        printf("please install & run RTSS process!\n");
        exit(-1);
    }

    return ConvertConstCharToWChar(api);
}
