#include "Utils.h"

BOOLEAN ErrorHandler(SCANNER_STATUS Status, DWORD LastError)
{
	switch (Status)
	{
	case SCANNER_STATUS::SCANNER_SUCCESS: {
		return TRUE; }
	case SCANNER_STATUS::SCANNER_INIT_FAILED: {
		Red; ErrLog("Initialization failed\n"); Gray; Gray; return FALSE; }
	case SCANNER_STATUS::SCANNER_WINDOW_NOT_FOUND: {
		Red; ErrLog("Can't get window handle\n\t[-] Err : %d(0x%X)\n", LastError, LastError); Gray; return FALSE; }
	case SCANNER_STATUS::SCANNER_PID_NOT_FOUND: {
		Red; ErrLog("Can't get process id\n\t[-] Err : %d(0x%X)\n", LastError, LastError); Gray; return FALSE; }
	case SCANNER_STATUS::SCANNER_FAILED_OPEN_PROCESS: {
		Red; ErrLog("Can't get process handle\n\t[-] Err : %d(0x%X)\n", LastError, LastError); Gray; return FALSE; }
	case SCANNER_STATUS::SCANNER_FAILED_GET_PROCESS_NAME: {
		Red; ErrLog("Can't get process name\n\t[-] Err : %d(0x%X)\n", LastError, LastError); Gray; return FALSE; }
	case SCANNER_STATUS::SCANNER_FAILED_GET_SNAPSHOT: {
		Red; ErrLog("Can't get process snapshot\n\t[-] Err : %d(0x%X)\n", LastError, LastError); Gray; return FALSE; }
	case SCANNER_STATUS::SCANNER_FAILED_GET_MEM_INFO: {
		Red; ErrLog("Can't get process memory information\n"); Gray; return FALSE; }
													//case SCANNER_STATUS::SCANNER_MEMORY_CANT_READ: {
													//	Red; ErrLog("Can't get process memory\n"); Gray; return FALSE; }
	case SCANNER_STATUS::SCANNER_POINTER_NOT_FOUND: {
		Red; ErrLog("Could not find requested pointer\n"); Gray; return FALSE; }
	}
	return TRUE;
}
