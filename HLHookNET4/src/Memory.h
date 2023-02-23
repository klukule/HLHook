#pragma once

#include "Utils.h"

class NativeMemory
{
private:
	NativeMemory() { ProcessHandle = nullptr; }

public:
	static HANDLE ProcessHandle;

	static NativeMemory* GetInstance() {
		static NativeMemory* instance = new NativeMemory(); return instance;
	}

	void UnregisterSymbol(const std::string& symbol);
	void RegisterSymbol(const std::string& symbol, char* address);
	char* GetAddress(const std::string& symbol);

	BOOL Scan(const std::string& pattern, char* begin, char* end, std::vector<char*>& matches);

	char* FindAddress(const std::string& symbol, int number, char* address, int size);
private:
	char* ScanIn(const char* pattern, const char* mask, char* begin, unsigned int size);
	BOOL ScanEx(const char* pattern, const char* mask, char* begin, char* end, std::vector<char*>& matches);

	BOOL CreatePattern(const std::string& pattern, std::string& pattern_result, std::string& mask_result);

private:
	std::unordered_map<std::string, char*> m_symbolMap;
};

template <typename T>
FORCEINLINE T Read(PVOID Address);

template <typename T>
FORCEINLINE T Read(PVOID Address)
{
	T Buffer{};
	if (!ReadProcessMemory(
		NativeMemory::ProcessHandle,
		Address,
		&Buffer,
		sizeof(T),
		NULL)
		)
	{
		ErrorHandler(SCANNER_STATUS::SCANNER_MEMORY_CANT_READ, GetLastError());
	}
	return Buffer;
}

FORCEINLINE UINT64* ReadPointer(PVOID Address) {
	return Read<UINT64*>(Address);
}

char* Read(PVOID Address, SIZE_T Size);


FORCEINLINE void ReadString(PVOID Address, std::string& result)
{
	// read string from pointer in chunks until null byte is found
	result = "";

	bool bZero = false;
	char offset = 0x0;
	char chunkSize = 0xF;
	do {
		char* data = Read((char*)Address + offset, chunkSize);
		for (int i = 0; i < chunkSize; i++)
		{
			if (data[i] == 0) {
				bZero = true;
				break;
			}
			result += data[i];
		}
		offset += chunkSize;
	} while (!bZero);

	return;
}