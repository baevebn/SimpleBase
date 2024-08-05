#pragma once
#define WIN32_LEAN_AND_MEAN 

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <type_traits>

using namespace std;

namespace memory
{
	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);

	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);

	uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, vector<unsigned int>offsets);
	uintptr_t FindDMAAddy(uintptr_t ptr, vector<unsigned int>offsets);

	uintptr_t HookDMAAddy(const char* pszName, uintptr_t baseAddr, vector<unsigned int>offsets);
	uintptr_t HookAddy(const char* pszName, uintptr_t baseAddr);

	uintptr_t Addy(uintptr_t baseAddr);
}

template<typename T>
inline void HookFunc(T value, const char* pszName, uintptr_t baseAddr, vector<unsigned int> offsets)
{
	DWORD ProcessId = GetCurrentProcessId();
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
	uintptr_t addr = memory::HookDMAAddy(pszName, baseAddr, offsets);

	if constexpr (is_integral_v<T>)
	{
		WriteProcessMemory(hProc, (LPVOID)addr, &value, sizeof(value), NULL);
	} 
	else if constexpr (is_floating_point_v<T>)
	{
		WriteProcessMemory(hProc, (LPVOID)addr, &value, sizeof(value), NULL);
	}

	CloseHandle(hProc);
}

inline void HookEngine() 
{
	
}