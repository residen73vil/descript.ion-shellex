#include "description_file_rw.h"
#include <iostream>



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
	DWORD bytesRead;
	if (!ReadFile(hFile, m_lpcFileBuffer, fileSize, &bytesRead, NULL)) {
		DEBUG_LOG("LoadFileToMap:Error reading file", GetLastError());
		CloseHandle(hFile);
		return false;
	}

	m_nFileSize = bytesRead;

	return true;
}

bool CDescriptionFileRW::FindLines() {

	char* start = m_lpcFileBuffer;
	//TODO: check bom
	char* end = m_lpcFileBuffer;

	for ( ; end < m_lpcFileBuffer + m_nFileSize; end++) {
		if (*end == '\n') {
			lines.emplace_back(start, end);
			start = end + 1;
		} 
	}
	if (start < end) { //add last line
		lines.emplace_back(start, end);
	}

	return true;
}

CDescriptionFileRW::~CDescriptionFileRW(){

	delete	m_lpcFileBuffer;
}