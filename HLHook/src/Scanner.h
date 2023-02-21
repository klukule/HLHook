#pragma once
#include "Utils.h"
#include <filesystem>

class NativeMemory;

typedef struct _SCANNER_DATA {
	HANDLE ProcessHandle;
	DWORD ProcessId;
	WCHAR ProcessFullName[MAX_PATH];
	std::filesystem::path ProcessName;
	std::filesystem::path BasePath;
	PVOID ImageBegin;
	PVOID ImageEnd;
	DWORD64 ImageSize;
}SCANNER_DATA, * PSCANNER_DATA;

class Scanner
{
private:
	Scanner() { Data = { 0 }; Mem = nullptr; }
public:
	static Scanner* GetInstance() {
		static Scanner* instance = new Scanner();
		return instance;
	}


	SCANNER_STATUS Initialize();

	// Specific pointer finding stuff
	//SCANNER_STATUS FindGUObjectArray();
	//SCANNER_STATUS FindFNamePool();
	SCANNER_STATUS FindUWorlds();
private:
	BOOLEAN GetProcessInfo();
public:
	SCANNER_DATA Data;
	NativeMemory* Mem;
};