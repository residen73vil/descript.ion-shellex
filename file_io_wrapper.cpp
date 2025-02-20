#include "file_io_wrapper.h"
#include "errors_and_settings.h"

size_t IOWrapper::LoadFileIntoBuffer(LPCTSTR filename, char** buffer){
	m_error = 0; //no errors yet
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
									FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE) {
		DEBUG_LOG("Error opening file", GetLastError());
		return 0;
	}
	//TODO: limit file size to prevent stack overflow
	DWORD fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE) {
		DEBUG_LOG("LoadFileToMap:Error getting file size", GetLastError());
		CloseHandle(hFile);
		return 0;
	}

	m_lpcFileBuffer =  new char[fileSize / sizeof(CHAR) + 1];
	DWORD bytesRead;
	if (!ReadFile(hFile, m_lpcFileBuffer, fileSize, &bytesRead, NULL)) {
		DEBUG_LOG("LoadFileToMap:Error reading file", GetLastError());
		CloseHandle(hFile);
		return 0;
	}
	CloseHandle(hFile);
	*buffer = m_lpcFileBuffer;
	return bytesRead;
}

size_t IOWrapper::WriteBufferIntoFile(LPCTSTR filename, char* buffer, size_t size ){
		m_error = 0; //no errors yet
		DWORD bytesWritten = 0;
		HANDLE hFile = CreateFile(filename,
		GENERIC_WRITE,			// Desired access
		FILE_SHARE_READ | FILE_SHARE_WRITE,						// Share mode
		NULL,					// Security attributes
		CREATE_ALWAYS,		  	// Creation disposition
		FILE_ATTRIBUTE_NORMAL,	// File attributes
		NULL	 );
	
	if (hFile == INVALID_HANDLE_VALUE) {
		m_error  = GetLastError(); // Get the error code
		DEBUG_LOG("CreateFile failed. Error: ", m_error);
		CErrorsAndSettings::IOError(m_error);
	} else if (!WriteFile(hFile, buffer, size, &bytesWritten, NULL)) { 
		m_error = GetLastError(); // Get the error code
		CErrorsAndSettings::IOError(m_error);
	}
	CloseHandle(hFile);
	return bytesWritten;
}

void IOWrapper::CleanupBuffersMemory(){
	if (nullptr != m_lpcFileBuffer){
		delete[] m_lpcFileBuffer;
	}
}
