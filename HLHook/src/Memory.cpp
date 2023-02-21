#include "Memory.h"
#include "../../Zydis/Zydis.h"
#include <string>
#include <sstream>

HANDLE NativeMemory::ProcessHandle;

bool isHex(char c)
{
	// 0-9, A-F, a-f, X or x
	return (c >= 48 && c <= 57) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102) || (c == 120) || (c == 88);
}

void NativeMemory::UnregisterSymbol(const std::string& symbol)
{
	if (m_symbolMap.find(symbol) != m_symbolMap.end())
		m_symbolMap.erase(symbol);
}

void NativeMemory::RegisterSymbol(const std::string& symbol, char* address)
{
	if (m_symbolMap.find(symbol) == m_symbolMap.end())
		m_symbolMap.emplace(symbol, address);
	else
		m_symbolMap[symbol] = address;
}

char* NativeMemory::GetAddress(const std::string& symbol)
{
	if (m_symbolMap.find(symbol) != m_symbolMap.end())
		return m_symbolMap[symbol];
	return nullptr;
}

BOOL NativeMemory::Scan(const std::string& pattern, char* begin, char* end, std::vector<char*>& matches)
{
	std::string result_pattern;
	std::string result_mask;

	if (!CreatePattern(pattern, result_pattern, result_mask)) {
		Log("Invalid pattern %s\n", pattern.c_str());
		return FALSE;
	}

	return ScanEx(result_pattern.c_str(), result_mask.c_str(), begin, end, matches);
}

void Disassemble(char* address, std::string& result, int& instruction_size) {
	char* data = Read(address, 0x15); // x64 instruction is up to 15 bytes long
	ZydisDisassembledInstruction instruction;
	if (ZYAN_SUCCESS(ZydisDisassembleIntel(ZYDIS_MACHINE_MODE_LONG_64, (ZyanU64)address, data, 0x15, &instruction))) {
		result = instruction.text;
		instruction_size = instruction.info.length;
		return;
	}

	result = "";
}

char* NativeMemory::FindAddress(const std::string& symbol, int number, char* address, int size)
{
	if (address == nullptr) return nullptr;
	if (size <= 0) size = 0x200;

	int i = 0;
	std::vector<char*> addr;

	while (i < size) {
		std::string instruction;
		int instruction_size;
		Disassemble(address + i, instruction, instruction_size);
		//Log("%s\n", instruction.c_str());
		// TODO: Better parsing

		// Find all square brancket pairs in instruction and get the string between them, do not use regex
		std::string::size_type pos = 0;
		std::string::size_type lastPos = 0;
		while ((pos = instruction.find('[', pos)) != std::string::npos) {
			lastPos = pos;
			pos = instruction.find(']', pos);
			if (pos == std::string::npos) break;

			std::string match = instruction.substr(lastPos + 1, pos - lastPos - 1);

			bool validHex = true;
			for (int i = 0; i < match.length(); i++) {
				if (!isHex(match[i])) {
					validHex = false;
					break;
				}
			}

			if (validHex) {
				addr.push_back((char*)std::stoull(match, nullptr, 16));
			}
		}

		i += instruction_size;
	}

	if (number <= 0) return addr[0];

	RegisterSymbol(symbol, addr[number]);

	return addr[number];
}

char* NativeMemory::ScanIn(const char* pattern, const char* mask, char* begin, unsigned int size)
{
	unsigned int patternLength = strlen(mask);

	for (unsigned int i = 0; i < size - patternLength; i++)
	{

		bool found = true;
		for (unsigned int j = 0; j < patternLength; j++)
		{
			if (mask[j] != '?' && pattern[j] != *(begin + i + j))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return (begin + i);
		}
	}
	return nullptr;
}

BOOL NativeMemory::ScanEx(const char* pattern, const char* mask, char* begin, char* end, std::vector<char*>& matches)
{
	char* currentChunk = begin;
	SIZE_T bytesRead;

	while (currentChunk < end)
	{
		MEMORY_BASIC_INFORMATION mbi;

		//return nullptr if VirtualQuery fails
		if (!VirtualQueryEx(ProcessHandle, currentChunk, &mbi, sizeof(mbi)))
		{
			Log("Failed Virtual Query %X\n", GetLastError());
			return FALSE;
		}

		char* buffer = new char[mbi.RegionSize];

		if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS)
		{
			DWORD oldprotect;
			if (VirtualProtectEx(ProcessHandle, mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldprotect))
			{
				ReadProcessMemory(ProcessHandle, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead);
				VirtualProtectEx(ProcessHandle, mbi.BaseAddress, mbi.RegionSize, oldprotect, &oldprotect);

				char* internalAddress = ScanIn(pattern, mask, buffer, bytesRead);

				if (internalAddress != nullptr)
				{
					//calculate from internal to external
					uintptr_t offsetFromBuffer = internalAddress - buffer;
					matches.push_back(currentChunk + offsetFromBuffer);
				}
			}
		}

		currentChunk = currentChunk + mbi.RegionSize;
		delete[] buffer;
	}
	return TRUE;
}

BOOL NativeMemory::CreatePattern(const std::string& pattern, std::string& pattern_result, std::string& mask_result)
{
	bool result = false;
	size_t l = pattern.size();
	if (l-- > 0)
	{
		std::stringstream pattern_s;
		std::stringstream mask_s;
		for (size_t i = 0; i < l; i++)
		{
			if (!isHex(pattern[i]))
			{
				if (pattern[i] == 63 && (i == 0 || (i > 0 && pattern[i - 1] != 63)))
				{
					pattern_s << "\x90";
					mask_s << '?';
				}
			}
			else
			{
				char buffer[2];
				buffer[0] = pattern[i];
				buffer[1] = (l >= i + 1 && isHex(pattern[i + 1])) ? pattern[++i] : 0;
				pattern_s << (char)strtol(buffer, nullptr, 16);
				mask_s << 'x';
			}
		}
		result = true;
		pattern_result = pattern_s.str();
		mask_result = mask_s.str();
	}
	return result;
}

char* Read(PVOID Address, SIZE_T Size)
{
	char* Buffer = new char[Size];

	/*DWORD oldprotect;
	if (VirtualProtectEx(Memory::ProcessHandle, Address, Size, PAGE_EXECUTE_READWRITE, &oldprotect))
	{
		ReadProcessMemory(Memory::ProcessHandle, Address, Buffer, Size, NULL);
		VirtualProtectEx(Memory::ProcessHandle, Address, Size, oldprotect, &oldprotect);
	}*/

	if (!ReadProcessMemory(
		NativeMemory::ProcessHandle,
		Address,
		Buffer,
		Size,
		NULL)
		)
	{
		ErrorHandler(SCANNER_STATUS::SCANNER_MEMORY_CANT_READ, GetLastError());
	}
	return Buffer;
}
