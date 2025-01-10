//Test description_file_rw.h
//g++ description_file_rw.test.cpp ../description_file_rw.cpp ../dbg.cpp -D_DEBUG -DUNICODE -D_UNICODE -std=c++11 -g
#include "../description_file_rw.h"
#include "windows.h"
#include <iostream>



int main(){

	CDescriptionFileRW test;
	std::wstring str;
	
	const wchar_t* file_names[] = { L".\\descript.ion", L".\\Descript.UTF8BOM.ion", L".\\Descript.UTF8.ion",
								L".\\Descript.UTF16LEBOM.ion", L".\\Descript.UTF16LE.ion", 
								L".\\Descript.UTF16BEBOM.ion", L".\\Descript.UTF16BE.ion" 
							};
#define FILE_NAMES_SIZE 7

	for (int i = 0; i < FILE_NAMES_SIZE; i++){
		test.LoadFile( file_names[i] );
		size_t number_of_lines = test.FindLines();
		for ( int j = 0; j < number_of_lines; j++){
			test.GetConvertedLine(j, &str);
			MessageBox(NULL, str.c_str(), file_names[i], MB_OK | MB_ICONINFORMATION);
			test.GetConvertedLine(1, &str);
		}
	}
}