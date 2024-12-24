//Test description_file_rw.h
//g++ description_file_rw.test.cpp ../description_file_rw.cpp ../dbg.cpp -D_DEBUG -DUNICODE -D_UNICODE -std=c++11 -g
#include "../description_file_rw.h"
#include "windows.h"
#include <iostream>



int main(){
	CDescriptionFileRW test;
	DEBUG_INIT("c:\\Logs\\dbg.log");
	
	test.LoadFile(_T(".\\descript.ion"));
	test.FindLines();

	test.LoadFile(_T(".\\Descript.UTF8BOM.ion"));
	test.FindLines();

	test.LoadFile(_T(".\\Descript.UTF8.ion"));
	test.FindLines();

	test.LoadFile(_T(".\\Descript.UTF16LEBOM.ion"));
	test.FindLines();
	
	test.LoadFile(_T(".\\Descript.UTF16LE.ion"));
	test.FindLines();

	test.LoadFile(_T(".\\Descript.UTF16BEBOM.ion"));
	test.FindLines();

	test.LoadFile(_T(".\\Descript.UTF16BE.ion"));
	test.FindLines();
}