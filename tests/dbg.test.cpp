//g++ dbg.test.cpp dbg.cpp -D_DEBUG -std=c++11
#include "dbg.h"
int main(){
	DEBUG_INIT("c:\\Logs\\dbg.log");
	DEBUG_LOG( "test:", "printing to the log test");
	_sleep(5000);
}