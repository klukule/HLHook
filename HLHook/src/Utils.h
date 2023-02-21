#pragma once

#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <functional>


#pragma warning(disable:4996)

#define Log(...) printf( "[LOG] " __VA_ARGS__ )
#define WarnLog(...) printf( "[WARN] " __VA_ARGS__ )
#define ErrLog(...) printf( "\n[ERROR] " __VA_ARGS__ )

#define Blue SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#define Green SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#define Gray SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#define Red  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
#define Yellow SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#define White SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#define Purple SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

enum class SCANNER_STATUS
{
	SCANNER_SUCCESS,
	SCANNER_INIT_FAILED,
	SCANNER_WINDOW_NOT_FOUND,
	SCANNER_PID_NOT_FOUND,
	SCANNER_FAILED_OPEN_PROCESS,
	SCANNER_FAILED_GET_PROCESS_NAME,
	SCANNER_FAILED_GET_SNAPSHOT,
	SCANNER_FAILED_GET_MEM_INFO,
	SCANNER_MEMORY_CANT_READ,
	SCANNER_POINTER_NOT_FOUND
};

BOOLEAN ErrorHandler(SCANNER_STATUS Status, DWORD LastError);