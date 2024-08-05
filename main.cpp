#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <codecvt>
#include <locale>
#include <iostream>
#include <vector>
#include <tchar.h>
#include <vector>

#pragma warning(disable:4996)
using namespace std;

DWORD FindProcessByName(const char* procname)
{
    HANDLE hSnapshot;
    PROCESSENTRY32 pe;

    int pid = 0;
    BOOL hResult;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        return 0;
    }

    pe.dwSize = sizeof(PROCESSENTRY32);
    hResult = Process32First(hSnapshot, &pe);

    while (hResult)
    {
        if (strcmp(procname, pe.szExeFile) == 0)
        {
            pid = pe.th32ProcessID;
            break;
        }

        hResult = Process32Next(hSnapshot, &pe);
    }

    CloseHandle(hSnapshot);

    return pid;
}

DWORD GetModuleBaseAddress(const char *lpszModuleName, DWORD pID)
{
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); 

    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) 
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));
    }

    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

uintptr_t GetPointerAddress(DWORD pID, uintptr_t gameBaseAddr, uintptr_t address, vector<unsigned int> offsets)
{
    HANDLE phandle = NULL;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);

    uintptr_t offset_null = NULL;

    ReadProcessMemory(phandle, (LPVOID*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
    uintptr_t pointeraddress = offset_null;

    for (unsigned int i = 0; i < offsets.size() - 1; i++) 
    {
        ReadProcessMemory(phandle, (LPVOID*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
    }

    return pointeraddress += offsets.at(offsets.size() - 1); 
}


int main()
{
	DWORD pID = FindProcessByName("gta_sa.exe");
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);

	uintptr_t moduleBaseAddr = GetModuleBaseAddress("gta_sa.exe", pID);
    uintptr_t addr = GetPointerAddress(pID, moduleBaseAddr, 0x771A38, { 0x540 });

	while (true)
	{
        float readValue;
        ReadProcessMemory(pHandle, (LPVOID)addr, &readValue, sizeof(readValue), NULL);

        if (readValue != 0 && readValue < 100)
        {
            float flHealth = 100.0f;
            WriteProcessMemory(pHandle, (LPVOID)addr, &flHealth, sizeof(flHealth), NULL);
        }
	}

	CloseHandle(pHandle);

	return 0;
}