//Test description_handler.h
//g++ description_handler.test.cpp ../description_handler.cpp ../description_file_rw.cpp ../dbg.cpp -D_DEBUG -DUNICODE -D_UNICODE -std=c++11 -g
#include "../description_handler.h"
#include "windows.h"
#include <iostream>



int main(){
	CDescriptionHandler test;
	DEBUG_INIT("c:\\Logs\\dbg.log");
	
	test.LoadFile(_T(".\\descript.UTF8BOM.ion"));
	//test.LoadPath(_T("."));
	test.IsCommented(L"a.exe");
	std::basic_string<TCHAR> comment;
	test.ReadComment(L"a.exe", comment);
	test.AddChangeComment(L"a.exe",L"test change");
	test.AddChangeComment(L"new file.exe",L"test new line");
	test.AddChangeComment(L"new file2.exe",L"test new line");
	test.SaveChanges();
	std::wcout << comment << std::endl;
}