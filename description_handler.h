#ifndef DESCRIPTION_HANDLER_H
#define DESCRIPTION_HANDLER_H
#include <windows.h>
#include <fileapi.h>
#include <tchar.h>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "dbg.h" 

#define DESCRIPTION_FILE _T("descript.ion")

typedef std::list< std::basic_string<TCHAR> > string_list;

// Forward declaration of the interface
class  CDescriptionHandler
{
private:
	std::unordered_map<std::basic_string<CHAR>, std::basic_string<CHAR>> comments_map;
public:
	bool LoadFile(LPCTSTR filename);
	bool LoadPath(LPCTSTR path); //calls LoadFile("%path%\\descript.ion")
	bool SaveChanges();
	bool ReadComment(LPCTSTR filename, /*out*/ LPTSTR comment);
	bool IsCommented(LPCTSTR filename);
	bool AddChangeComment(LPCTSTR filename, LPCTSTR comment);
	bool FilesWithComments(/*out*/ string_list* files); //returns files that have comments 
//TODO: Broken comments for non-existing files handling
//TODO: Codepage maybe DOS OEM filenames maybe in dos format too
//TODO: Hide descript.ion file
	bool LoadFileToMap(LPCTSTR &filePath);
};


#endif