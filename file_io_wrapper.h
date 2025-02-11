#include <fileapi.h>
class IOWrapper
{
public:
	char* m_lpcFileBuffer = nullptr;

	size_t LoadFileIntoBuffer(LPCTSTR filename, char** buffer){
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
		 //making copy because m_lpcFileBuffer may be changed by LookForBomInBuffer
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
	
	size_t WriteBufferIntoFile(LPCTSTR filename, char* buffer, size_t size ){
			HANDLE hFile = CreateFile(filename,
			GENERIC_WRITE,			// Desired access
			FILE_SHARE_READ | FILE_SHARE_WRITE,						// Share mode
			NULL,					// Security attributes
			CREATE_ALWAYS,		  	// Creation disposition
			FILE_ATTRIBUTE_NORMAL,	// File attributes
			NULL	 );
		
		if (hFile == INVALID_HANDLE_VALUE) {
			DWORD error = GetLastError(); // Get the error code
			printf("CreateFile failed. Error: %lu\n", error);
	
			// Interpret the error code
			switch (error) {
				case ERROR_ACCESS_DENIED:
					DEBUG_LOG(L"Error opening file",L"Access denied.\n");
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
					DEBUG_LOG(L"Error opening file",L"Disk is full.\n");
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
					DEBUG_LOG(L"Error opening file",L"I/O device error.\n");
					break;
				default:
					DEBUG_LOG(L"Error opening file",L"An unknown error occurred.\n");
					break;
			}
		}
		DWORD bytesWritten = 0;
		if (!WriteFile(hFile, buffer, size, &bytesWritten, NULL)) {
			DWORD error = GetLastError(); // Get the error code
	
			// Interpret the error code
			switch (error) {
				case ERROR_DISK_FULL:
					DEBUG_LOG(L"Error opening file",L"Disk is full.\n");
					break;
				case ERROR_ACCESS_DENIED:
					DEBUG_LOG(L"Error opening file",L"Access denied.\n");
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
	
	void CleanupBuffersMemory(){
		if (nullptr != m_lpcFileBuffer){
			delete[] m_lpcFileBuffer;
		}
	}
	
	//LPCTSTR GetIOErrorMsg(){

	//}
};