//Test description_file_rw.h
//g++ description_file_rw.test.cpp ../description_file_rw.cpp ../dbg.cpp -D_DEBUG -DUNICODE -D_UNICODE -std=c++11 -g
#include "../description_file_rw.h"
#include "windows.h"
#include <iostream>

const wchar_t* file_names[] = { L".\\descript.ion", L".\\Descript.UTF8BOM.ion", L".\\Descript.UTF8.ion",
								L".\\Descript.UTF16LEBOM.ion", L".\\Descript.UTF16LE.ion", 
								L".\\Descript.UTF16BEBOM.ion", L".\\Descript.UTF16BE.ion" 
							};
#define FILE_NAMES_SIZE 7

int main(){
	CDescriptionFileRW test;
	std::wstring str;
	DEBUG_INIT("c:\\Logs\\dbg.log");


	for (int i = 0; i < FILE_NAMES_SIZE; i++){
		test.LoadFile( file_names[i] );
		size_t number_of_lines = test.FindLines();
		for ( int j = 0; j < number_of_lines; j++){
			test.GetConvertedLine(j, &str);
			//using MessageBox because iostream has problems with utf I don't want to solve right now
			MessageBox(NULL, str.c_str(), file_names[i], MB_OK | MB_ICONINFORMATION);
			test.GetConvertedLine(1, &str);
		}
	}
	
	test.LoadFile( L".\\descript.ion" );
	size_t number_of_lines = test.FindLines();
	str = L"12345 test";
	test.ChangeLine(1,&str);
	str = L"54321 test";
	test.ChangeLine(3,&str);
	test.ConvertAndSaveChanges(CP_ACP);

	test.LoadFile( L".\\Descript.UTF16LEBOM.ion" );
	number_of_lines = test.FindLines();
	str = L"12345 test";
	test.ChangeLine(1,&str);
	str = L"54321 test";
	test.ChangeLine(3,&str);
	test.ConvertAndSaveChanges(CP_UTF16LE);

	test.LoadFile( L".\\Descript.UTF16BEBOM.ion" );
	number_of_lines = test.FindLines();
	str = L"12345 test";
	test.ChangeLine(1,&str);
	str = L"54321 test";
	test.ChangeLine(3,&str);
	test.ConvertAndSaveChanges(CP_UTF16BE);
	
	test.LoadFile( L".\\Descript.UTF8.ion" );
	number_of_lines = test.FindLines();
	str = L"12345 test";
	test.ChangeLine(1,&str);
	str = L"54321 test";
	test.ChangeLine(3,&str);
	test.ConvertAndSaveChanges(CP_UTF8);
}