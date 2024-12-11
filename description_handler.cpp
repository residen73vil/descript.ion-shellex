#include "description_handler.h"
#include <iostream>

bool CDescriptionHandler::LoadFile(LPCTSTR filename)
{
	DEBUG_LOG( "CDescriptionHandler::LoadFile", filename);
	LoadFileToMap(filename);
	return true;
}

bool CDescriptionHandler::LoadPath(LPCTSTR path) //calls LoadFile("%path%/descript.ion")
{
	TCHAR filename[MAX_PATH];
	_tcscpy(filename, path);
	_tcscat( filename, _T("\\") );
	_tcscat( filename, DESCRIPTION_FILE );
	
	LoadFile( filename );
	return true;
}

bool CDescriptionHandler::SaveChanges()
{
	return true;
}

bool CDescriptionHandler::ReadComment(LPCTSTR filename, /*out*/ LPTSTR comment)
{
	return true;
}

bool CDescriptionHandler::IsCommented(LPCTSTR filename)
{
	return true;
}

bool CDescriptionHandler::AddChangeComment(LPCTSTR filename, LPCTSTR comment)
{
	return true;
}

bool CDescriptionHandler::FilesWithComments(/*out*/ string_list* files) //returns files that have comments
{
	return true;
}



bool CDescriptionHandler::LoadFileToMap(LPCTSTR &filePath) {
    HANDLE hFile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
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

    std::vector<CHAR> buffer(fileSize / sizeof(CHAR) + 1);
    DWORD bytesRead;
    if (!ReadFile(hFile, buffer.data(), fileSize, &bytesRead, NULL)) {
        DEBUG_LOG("LoadFileToMap:Error reading file", GetLastError());
        CloseHandle(hFile);
        return false;
    }

    buffer[bytesRead / sizeof(CHAR)] = 0; // Null-terminate the string

    CloseHandle(hFile);

    std::basic_string<CHAR> content(buffer.data());
    std::basic_istringstream<CHAR> iss(content);
    std::basic_string<CHAR> line;

    while (std::getline(iss, line)) {
		//std::cout << line << std::endl;
		size_t pos = 0;
		if (line[0] == '\"'){
        	pos = line.find_first_of('\"', 1);
			if (pos != std::basic_string<CHAR>::npos && pos != 2){
				line.erase(0,1); //get rid of first \"
				pos = pos - 1; //correct to accommodate deletions 
				line.erase(pos,1); //get rid of last \"
			} else {
				DEBUG_LOG("LoadFileToMap:error in descript.ion file", 
								"filename is too small/large");
				continue; //skip broken filename
			}
		} else {
			pos = line.find_first_of(" \t");
		}
        if (pos != std::basic_string<CHAR>::npos && pos != 0) {
        	std::basic_string<CHAR> key = line.substr(0, pos);
            std::basic_string<CHAR> value = line.substr(pos + 1);
			pos = value.find_last_of('\r');
			if ( pos != std::basic_string<CHAR>::npos){
				value.erase(pos,1); //get rid of \r at the end
			}
			value.erase(value.find_last_not_of(" \t")+1); //strip spaces at the beginning
			if (comments_map.count(key) > 0) {
				DEBUG_LOG_ANSI("LoadFileToMap:error in descript.ion filename mentioned several times", key );
				//old value will be overwritten!
				//TODO: does nothing on non-debug builds, so add something here!
			}
            comments_map[key] = value;
        }
    }

    return true;
}