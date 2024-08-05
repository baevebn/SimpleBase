#include "memory.h"

void memory::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	DWORD oldprotect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

void memory::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);
	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}

void memory::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void memory::Nop(BYTE* dst, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

uintptr_t memory::FindDMAAddy(HANDLE hProc, uintptr_t ptr, vector<unsigned int>offsets)
{
	uintptr_t addr = ptr;

	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}

	return addr;
}

uintptr_t memory::FindDMAAddy(uintptr_t ptr, vector<unsigned int>offsets)
{
	uintptr_t addr = ptr;

	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}

	return addr;
}

uintptr_t memory::HookDMAAddy(const char* pszName, uintptr_t baseAddr, vector<unsigned int>offsets)
{
	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(pszName);
	uintptr_t addr = (uintptr_t)(moduleBase + baseAddr);

	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}

	return addr;
}

uintptr_t memory::HookAddy(const char* pszName, uintptr_t baseAddr)
{
	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(pszName);
	uintptr_t addr = (uintptr_t)(moduleBase + baseAddr);

	return addr;
}

uintptr_t memory::Addy(uintptr_t baseAddr)
{
	uintptr_t addr = (uintptr_t)(baseAddr);

	return addr;
}
