#include "file_io_wrapper.h"

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

		// Interpret the error code
		switch (m_error) {
			case ERROR_ACCESS_DENIED:
				DEBUG_LOG(L"Error opening file", ERROR_ACCESS_DENIED_MSG);
				break;
			case ERROR_FILE_NOT_FOUND:
				DEBUG_LOG(L"Error opening file",L"File not found.\n");
				break;
			case ERROR_PATH_NOT_FOUND:
				DEBUG_LOG(L"Error opening file",L"Path not found.\n");
				break;
			case ERROR_INVALID_PARAMETER:
				DEBUG_LOG(L"Error opening file",L"Invalid parameter.\n");
				break;
			case ERROR_ALREADY_EXISTS:
				DEBUG_LOG(L"Error opening file",L"File already exists.\n");
				break;
			case ERROR_DISK_FULL:
				DEBUG_LOG(L"Error opening file", ERROR_DISK_FULL_MSG);
				break;
			case ERROR_SHARING_VIOLATION:
				DEBUG_LOG(L"Error opening file",L"Sharing violation.\n");
				break;
			case ERROR_INVALID_HANDLE:
				DEBUG_LOG(L"Error opening file",L"Invalid handle.\n");
				break;
			case ERROR_NOT_ENOUGH_MEMORY:
				DEBUG_LOG(L"Error opening file",L"Not enough memory.\n");
				break;
			case ERROR_IO_DEVICE:
				DEBUG_LOG(L"Error opening file", ERROR_IO_DEVICE_MSG);
				break;
			default:
				DEBUG_LOG(L"Error opening file",L"An unknown error occurred.\n");
				break;
		}
	}
	DWORD bytesWritten = 0;
	if (!WriteFile(hFile, buffer, size, &bytesWritten, NULL)) {
		m_error = GetLastError(); // Get the error code

		// Interpret the error code
		switch (m_error) {
			case ERROR_DISK_FULL:
				DEBUG_LOG(L"Error opening file", ERROR_DISK_FULL_MSG);
				break;
			case ERROR_ACCESS_DENIED:
				DEBUG_LOG(L"Error opening file", ERROR_ACCESS_DENIED_MSG);
				break;
			case ERROR_INVALID_HANDLE:
				DEBUG_LOG(L"Error opening file",L"Invalid handle.\n");
				break;
			// Add more cases as needed
			default:
				DEBUG_LOG(L"Error opening file",L"An unknown error occurred.\n");
				break;
		}
	}
	CloseHandle(hFile);
	return bytesWritten;
}

void IOWrapper::CleanupBuffersMemory(){
	if (nullptr != m_lpcFileBuffer){
		delete[] m_lpcFileBuffer;
	}
}

LPCTSTR IOWrapper::GetIOErrorMsg(){
	if (m_error == 0)
		return L""; //no error
	switch (m_error) {
		case ERROR_ACCESS_DENIED:
			return ERROR_ACCESS_DENIED_MSG;
		case ERROR_DISK_FULL:
			return ERROR_DISK_FULL_MSG;
		case ERROR_IO_DEVICE:
			return ERROR_IO_DEVICE_MSG;
		default:
			return L"An unknown error occurred.\n";
	}
}