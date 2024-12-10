//Test debug loging
//g++ dbg.test.cpp ../dbg.cpp -D_DEBUG -std=c++11 -lole32
//nothing should be printed if compiled without _DEBUG defined
//g++ dbg.test.cpp ../dbg.cpp -std=c++11 -lole32
#include "../dbg.h"
#include "windows.h"
#include <shlguid.h>
const GUID IID_Itest = { 0xed730b9b, 0x5779, 0x4692, { 0x9e, 0x36, 0xf7, 0xd9, 0xa4, 0xe8, 0x6e, 0x06 } };
int main(){
	DEBUG_INIT("c:\\Logs\\dbg.log");
	DEBUG_LOG( "test:", "printing to the log test");
	DEBUG_LOG_RIID(L"test:", IID_Itest) ;
	DEBUG_LOG_ANSI(L"test:", "test_ansi") ;
	_sleep(5000); //to keep file busy for 5 seconds 
	DEBUG_CLOSE;
}