#ifndef IO_WRAPPER_H
#define IO_WRAPPER_H

#include <fileapi.h>
#include <windows.h>
#include "dbg.h"

/**
 * @class IOWrapper
 * @brief A class for handling input and output operations.
 *
 * The IOWrapper class provides a unified interface for performing
 * various input and output operations, such as reading from and
 * writing to files, managing buffers and also handling io errors.
 *
 * This class is designed to simplify the process of data
 * manipulation and ensure efficient resource management.
 */

class IOWrapper

{
const wchar_t* ERROR_ACCESS_DENIED_MSG = L"Access denied.\n";
const wchar_t* ERROR_DISK_FULL_MSG = L"Disk is full.\n";
const wchar_t* ERROR_IO_DEVICE_MSG = L"I/O device error.\n";

DWORD m_error = 0;

public:
	char* m_lpcFileBuffer = nullptr; //holds buffer with data read from the file

	bool CheckIfFileExists(LPCTSTR filename);

	/**
	 * @brief Loads the contents of a file into a buffer.
	 *
	 * This function reads the entire contents of the specified file
	 * and stores it in a dynamically allocated buffer. The caller is
	 * responsible for freeing the allocated memory via GetIOErrorMsg
	 * function of the class.
	 *
	 * @param filename The name of the file to be loaded. This should
	 *                 be a valid path to an existing file.
	 * @param buffer A pointer to a character pointer that will be
	 *               allocated and filled with the file contents. The
	 *               caller must free this memory after use.
	 *
	 * @return The size of the file in bytes if the operation was
	 *         successful, or 0 if there was an error (e.g., file not
	 *         found, read error).
	 */
	size_t LoadFileIntoBuffer(LPCTSTR filename, char** buffer);
	
	/**
	 * @brief Writes a buffer's contents to a specified file.
	 *
	 * This function takes a buffer containing data and writes it to
	 * the specified file. If the file does not exist, it will be created.
	 * If the file already exists, its contents will be overwritten.
	 *
	 * @param filename The name of the file to which the buffer will be written.
	 *                 This should be a valid path. The function will create
	 *                 the file if it does not exist.
	 * @param buffer A pointer to the data buffer that contains the data to be written.
	 * @param size The number of bytes to write from the buffer to the file.
	 *
	 * @return The number of bytes successfully written to the file. If the
	 *         operation fails (e.g., due to file system errors), it may return
	 *         a value less than `size`.
	 */
	size_t WriteBufferIntoFile(LPCTSTR filename, char* buffer, size_t size );
	
	/**
	 * @brief Cleans up allocated, for buffer, memory if it was allocated in LoadFileIntoBuffer.
	 */
	void CleanupBuffersMemory();
	
		/**
	 * @brief Returns string containing error occurred while IO operations.

	 * @return Pointer to a string representation of IO error
	 */
	LPCTSTR GetIOErrorMsg();
};

#endif