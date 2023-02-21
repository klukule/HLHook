#include "Scanner.h"
#include "Memory.h"

SCANNER_STATUS Scanner::Initialize()
{
	Mem = NativeMemory::GetInstance();

	if (!GetProcessInfo()) { return SCANNER_STATUS::SCANNER_INIT_FAILED; }
	return SCANNER_STATUS::SCANNER_SUCCESS;
}


SCANNER_STATUS Scanner::FindUWorlds() {
	bool bFound = false;
	std::vector<std::string> AOB = { "48 8B 05 ??  ??  ??  ??  48 83 C4 20 5B C3 8B 41 10" };
	for (int i = 0; i < AOB.size(); i++)
	{
		std::vector<char*> result1;
		NativeMemory::GetInstance()->Scan(AOB[i], (char*)Data.ImageBegin, (char*)Data.ImageEnd, result1);
		for (int j = 0; j < result1.size(); j++)
		{
			char* pointer = NativeMemory::GetInstance()->FindAddress("UWorlds", 0, result1[i], 0x50);

			// TODO: Validate
			{
				NativeMemory::GetInstance()->RegisterSymbol("UWorlds", pointer);
				bFound = true;
			}

			if (bFound)
				break;
		}
		if (bFound)
			break;
	}
	return bFound ? SCANNER_STATUS::SCANNER_SUCCESS : SCANNER_STATUS::SCANNER_POINTER_NOT_FOUND;
}


BOOLEAN Scanner::GetProcessInfo()
{
	DWORD BufferSize = MAX_PATH;
	wchar_t ProcPath[MAX_PATH] = { 0, };
	VS_FIXEDFILEINFO version = { 0, };

	HWND WindowHandle = FindWindowA("UnrealWindow", NULL);
	if (!WindowHandle) { ErrorHandler(SCANNER_STATUS::SCANNER_WINDOW_NOT_FOUND, GetLastError()); return FALSE; }

	GetWindowThreadProcessId(WindowHandle, &Data.ProcessId);
	if (!Data.ProcessId) { ErrorHandler(SCANNER_STATUS::SCANNER_PID_NOT_FOUND, GetLastError()); return FALSE; }
	//CloseHandle(WindowHandle);

	Data.ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Data.ProcessId);
	if (!Data.ProcessHandle) { ErrorHandler(SCANNER_STATUS::SCANNER_FAILED_OPEN_PROCESS, GetLastError()); return FALSE; }

	Mem->ProcessHandle = Data.ProcessHandle;

	if (!QueryFullProcessImageName(Data.ProcessHandle, 0, ProcPath, &BufferSize))
	{
		ErrorHandler(SCANNER_STATUS::SCANNER_FAILED_GET_PROCESS_NAME, GetLastError());
		return FALSE;
	}

	wcscpy(Data.ProcessFullName, ProcPath);
	std::filesystem::path ProcessName = std::filesystem::path(Data.ProcessFullName).filename();
	Data.ProcessName = ProcessName;
	auto Root = std::filesystem::path(std::filesystem::current_path());
	auto Game = Root / "Process" / Data.ProcessName.stem();
	Data.BasePath = Game;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, Data.ProcessId);
	if (!hSnapshot) { ErrorHandler(SCANNER_STATUS::SCANNER_FAILED_GET_SNAPSHOT, GetLastError()); CloseHandle(Data.ProcessHandle); return FALSE; }

	MODULEENTRY32W ModuleEntry = { sizeof(MODULEENTRY32W) };
	while (Module32NextW(hSnapshot, &ModuleEntry))
	{
		if (!wcscmp(ProcessName.c_str(), ModuleEntry.szModule))
		{
			Log("Found Target Process : ");
			Green;
			printf("%ws\n\n", ModuleEntry.szModule);
			Gray;
			Data.ImageBegin = ModuleEntry.modBaseAddr;
			Data.ImageEnd = ModuleEntry.modBaseAddr + ModuleEntry.modBaseSize;
			Data.ImageSize = ModuleEntry.modBaseSize;
			CloseHandle(hSnapshot);
			break;
		}
	}
	if (!Data.ImageBegin) { ErrorHandler(SCANNER_STATUS::SCANNER_FAILED_GET_MEM_INFO, 0); return FALSE; }

	return TRUE;
}
