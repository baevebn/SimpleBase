#include "memory.h"

void StartFunction(HMODULE hModule)
{
	hModule = GetModuleHandle(NULL);
	do
	{
	start:
		HookEngine();
		goto start;
	} while (!hModule);
}

BOOL __declspec(dllexport) APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CloseHandle(CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)StartFunction, hModule, NULL, nullptr));
		break;
	case DLL_PROCESS_DETACH:
		FreeLibraryAndExitThread(hModule, NULL);
		break;
	}

	return TRUE;
}