#ifndef DBG_H
#define DBG_H
#ifdef _DEBUG
	#include <fstream>
	#include <windows.h>
	extern std::wofstream dbgout;
	void dbg_open_log_file(const char* log_file);
	//open log file (must be called somewhere in main)
	#define DEBUG_INIT(log_file)
	//close log file
	#define DEBUG_CLOSE
	//converts riid into a string and print it to the log
	#define DEBUG_LOG_RIID(where_from, riid) \
		WCHAR buffer[39]; \
		StringFromGUID2(riid, buffer, sizeof(buffer) / sizeof(WCHAR)); \
		OutputDebugString(where_from); \
		OutputDebugString(L": "); \
		OutputDebugString(buffer); \
		OutputDebugString(L"\n");
	//print a string into the log
	#define DEBUG_LOG(where_from, msg) \
		OutputDebugString(where_from); \
		OutputDebugString(L"\n");
	#define DEBUG_LOG_ANSI(where_from, msg) \
		std::string msg_char = msg; \
		std::wstring msg_wchar(msg_char.begin(), msg_char.end()); \
		OutputDebugString(where_from); \
		OutputDebugString(L": "); \
		OutputDebugString(msg_wchar.c_str()); \
		OutputDebugString(L"\n");
#else
	#define DEBUG_LOG_RIID(where_from, msg)
	#define DEBUG_LOG(where_from, msg)
	#define DEBUG_LOG_ANSI(where_from, msg)
	#define DEBUG_INIT(log_file)
	#define DEBUG_CLOSE
	
#endif //_DEBUG


#endif //DBG_H
