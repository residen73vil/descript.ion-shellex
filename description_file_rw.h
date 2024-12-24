#ifndef DESCRIPTION_FILE_RW_H
#define DESCRIPTION_FILE_RW_H
#include <windows.h>
#include <fileapi.h>
#include <tchar.h>
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include "dbg.h" 

#define DESCRIPTION_FILE _T("descript.ion")

typedef std::list< std::basic_string<TCHAR> > string_list;

// Forward declaration of the interface
class  CDescriptionFileRW
{
private:
	char* m_lpcFileBuffer;
	UINT m_nFileSize = 0;
	std::vector< std::pair<char*,char*> > lines;
public:
	UINT m_nCodepage = 0;
	UINT m_nBitOrder = 0;
	UINT m_nTargetCodepage = 0;
	
	bool LoadFile(LPCTSTR filename);
	UINT CheckBitOrderSing();
	
	bool ChangeLine(int number, std::wstring* line);
	bool SaveChanges();
	bool ConvertAndSaveChanges(UINT codepage);

	bool FindLines();
	bool GetConvertedLine(int number, /*out*/ std::wstring* line);

	~CDescriptionFileRW();
};


#endif