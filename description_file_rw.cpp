#include "description_file_rw.h"




bool CDescriptionFileRW::LoadFile(LPCTSTR filename) {
	HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
									FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE) {
		DEBUG_LOG("Error opening file", GetLastError());
		return false;
	}
	//TODO: limit file size to prevent stack overflow
	DWORD fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE) {
		DEBUG_LOG("LoadFileToMap:Error getting file size", GetLastError());
		CloseHandle(hFile);
		return false;
	}

	m_lpcFileBuffer =  new char[fileSize / sizeof(CHAR) + 1];
	m_lpcFileBuffer_copy = m_lpcFileBuffer; //making copy because m_lpcFileBuffer may be changed by LookForBomInBuffer
	DWORD bytesRead;
	if (!ReadFile(hFile, m_lpcFileBuffer, fileSize, &bytesRead, NULL)) {
		DEBUG_LOG("LoadFileToMap:Error reading file", GetLastError());
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);

	m_nFileSize = bytesRead;
	LookForBomInBuffer();

	return true;
}

size_t CDescriptionFileRW::FindLines() {
	
	m_vLines.clear();
	char* start = m_lpcFileBuffer;
	char* end = m_lpcFileBuffer;

	for ( ; end < m_lpcFileBuffer + m_nFileSize; end++) {
		if (*end == '\n') {
			m_vLines.emplace_back(start, end);
			start = end + 1;
			if (*start == '\0') //fix utf16 off by 1 problem, TODO: more consistent fix
				start=start+1;
		} 
	}
	if (start < end) { //add last line
		m_vLines.emplace_back(start, end);
	}

	return m_vLines.size();
}

UINT CDescriptionFileRW::LookForBomInBuffer(){
	//WARNIGN changing m_lpcFileBuffer pointer causes error upon its deletion, use real pointer instead!!!
	if (m_lpcFileBuffer[0] == BOM_UTF8[0] && 
			m_lpcFileBuffer[1] == BOM_UTF8[1] &&
			m_lpcFileBuffer[2] == BOM_UTF8[2]
		)
	{
		m_lpcFileBuffer = m_lpcFileBuffer + 3; //skip BOM if found
		m_nBitOrder = BOM_UTF8_MODE;
		m_nCodepage = CP_UTF8;
		return  m_nBitOrder;
	}
	
	if (m_lpcFileBuffer[0] == BOM_UTF16_LE[0] && 
			m_lpcFileBuffer[1] == BOM_UTF16_LE[1]
		)
	{
		m_lpcFileBuffer = m_lpcFileBuffer + 2; //skip BOM if found
		m_nCodepage = CP_UTF16LE;
		m_nBitOrder = BOM_UTF16_LE_MODE;
		return  m_nBitOrder;
	}

	if (m_lpcFileBuffer[0] == BOM_UTF16_BE[0] && 
			m_lpcFileBuffer[1] == BOM_UTF16_BE[1]
		)
	{
		m_lpcFileBuffer = m_lpcFileBuffer + 2; //skip BOM if found
		m_nCodepage = CP_UTF16BE;
		m_nBitOrder = BOM_UTF16_BE_MODE;
		return  m_nBitOrder;
	}
	m_nBitOrder = BOM_NONE_MODE;
	return m_nBitOrder;
}

int CDescriptionFileRW::GetConvertedLine(int number, /*out*/ std::wstring* line){
	std::pair<char*,char*> linebonds =  m_vLines[number];
	if (m_nCodepage == CP_UTF16LE){
		size_t wstr_len = (linebonds.second - linebonds.first)/2;
		*line = std::wstring(reinterpret_cast<wchar_t*>(linebonds.first), wstr_len);
		return wstr_len;
	//just swat bytes if BIG ENDIAN
	}else if (m_nCodepage == CP_UTF16BE){
		char* multiByteStr = linebonds.first-1; // -1 to compensate for start+=1 in FindLines at "\n\0"s
		if (number == 0) multiByteStr = linebonds.first; //except for the first one, it always points tho the
														//start of the buffer, so no compensation needed
		size_t length_in_bytes = linebonds.second - linebonds.first;
		//TODO: check mod(length_in_bytes, 2) to make sure it is multiple of 2
		char* swap_buffer = new char[length_in_bytes+2];
		for (int i = 0; i < length_in_bytes; i+=2){
			swap_buffer[i] = multiByteStr[i+1];
			swap_buffer[i+1] = multiByteStr[i];
		}
		swap_buffer[length_in_bytes] = '\0'; //null terminate the string
		swap_buffer[length_in_bytes+1] = '\0';
		
		*line = std::wstring(reinterpret_cast<wchar_t*>(swap_buffer), length_in_bytes/2+1);
		delete[] swap_buffer;
		return length_in_bytes/2;
	//converting non utf16 files   
	} else {
		size_t multi_byte_str_size = linebonds.second - linebonds.first;
		char* multiByteStr = linebonds.first;
		int wideCharSize = MultiByteToWideChar(m_nCodepage, 0, multiByteStr, multi_byte_str_size, NULL, 0);
		if (wideCharSize <= 0) {
			DEBUG_LOG( "Error getting wide char size", GetLastError() );
			return 0;
		}
	
		// Allocate buffer for wide character string
		wchar_t* wideStr = new wchar_t[wideCharSize + 1];
	
		// Perform the conversion
		int result = MultiByteToWideChar(m_nCodepage, 0, multiByteStr, multi_byte_str_size,
											 wideStr, wideCharSize);
		if (result <= 0) {
			DEBUG_LOG( "Error converting to wide char", GetLastError() );
			delete[] wideStr; // Clean up allocated memory
			return 0;
		}
		wideStr[wideCharSize] = '\0'; //zero terminate the string 
		*line = std::wstring(wideStr);
		delete[] wideStr;
		return wideCharSize;
	}
}


CDescriptionFileRW::~CDescriptionFileRW(){

	delete[]	m_lpcFileBuffer_copy;
}