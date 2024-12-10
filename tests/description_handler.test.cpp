//Test description_handler.h
//g++ description_handler.test.cpp ../description_handler.cpp ../dbg.cpp -D_DEBUG -DUNICODE -D_UNICODE -std=c++11 -g
#include "../description_handler.h"
#include "windows.h"


int main(){
	CDescriptionHandler test;
	DEBUG_INIT("c:\\Logs\\dbg.log");
	
	test.LoadFile(_T(".\\descript.ion"));
	test.LoadPath(_T("."));

	DEBUG_CLOSE;
}