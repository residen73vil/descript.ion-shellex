#ifndef DBG_H
#define DBG_H
#ifdef _DEBUG
	#include <fstream>
	extern std::wofstream dbgout;
	void dbg_open_log_file(const char* log_file);
	//open log file (must be called somwere in main)
	#define DEBUG_INIT(log_file) dbg_open_log_file(log_file);
	//close log file
	#define DEBUG_CLOSE dbgout.close();
	//converts riid into a string and print it to the log
    #define DEBUG_LOG_RIID(where_from, riid) WCHAR buffer[39]; \
		StringFromGUID2(riid, buffer, sizeof(buffer) / sizeof(WCHAR)); \
		dbgout << where_from << L": " << buffer << std::endl ;
	//print a string into the log
	#define DEBUG_LOG(where_from, msg) dbgout << where_from << L": " << msg << std::endl ;
#else
    #define DEBUG_LOG_RIID(where_from, msg)
	#define DEBUG_LOG(where_from, msg)
	#define DEBUG_INIT(log_file)
	#define DEBUG_CLOSE
#endif


#endif