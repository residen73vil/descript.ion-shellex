#ifndef DBG_H
#define DBG_H
#ifdef _DEBUG
	#include <fstream>
	#include <windows.h>
	extern CRITICAL_SECTION dbg_critical_section;
	extern std::wofstream dbgout;
	void dbg_open_log_file(const char* log_file);
	//open log file (must be called somwere in main)
	#define DEBUG_INIT(log_file) InitializeCriticalSection(&dbg_critical_section);\
		EnterCriticalSection(&dbg_critical_section); \
		dbg_open_log_file(log_file); \
		LeaveCriticalSection(&dbg_critical_section);
	//close log file
	#define DEBUG_CLOSE dbgout.close(); DeleteCriticalSection(&dbg_critical_section);
	//converts riid into a string and print it to the log
    #define DEBUG_LOG_RIID(where_from, riid) WCHAR buffer[39]; \
		StringFromGUID2(riid, buffer, sizeof(buffer) / sizeof(WCHAR)); \
		EnterCriticalSection(&dbg_critical_section); \
		dbgout << where_from << L": " << buffer << std::endl ;\
		LeaveCriticalSection(&dbg_critical_section);
	//print a string into the log
	#define DEBUG_LOG(where_from, msg) EnterCriticalSection(&dbg_critical_section); \
		dbgout << where_from << L": " << msg << std::endl ; \
		LeaveCriticalSection(&dbg_critical_section);
	#define DEBUG_LOG_ANSI(where_from, msg)  std::string msg_char = msg; \
		std::wstring msg_wchar(msg_char.begin(), msg_char.end()); \
		EnterCriticalSection(&dbg_critical_section); \
		dbgout << where_from << L": " << msg_wchar << std::endl ; \
		LeaveCriticalSection(&dbg_critical_section);
#else
    #define DEBUG_LOG_RIID(where_from, msg)
	#define DEBUG_LOG(where_from, msg)
	#define DEBUG_LOG_ANSI(where_from, msg)
	#define DEBUG_INIT(log_file)
	#define DEBUG_CLOSE
	
#endif //_DEBUG


#endif //DBG_H