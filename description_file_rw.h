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

#define BOM_UTF8 "\xEF\xBB\xBF"
#define BOM_UTF16_LE "\xFF\xFE"
#define BOM_UTF16_BE "\xFE\xFF"

#define BOM_NONE_MODE 0
#define BOM_UTF8_MODE 1
#define BOM_UTF16_LE_MODE 2
#define BOM_UTF16_BE_MODE 3

//are not defined in library files, so defining them by hand
#define CP_UTF16LE 1200
#define CP_UTF16BE 1201


#define DESCRIPTION_FILE _T("descript.ion")

typedef std::list< std::basic_string<TCHAR> > string_list;

// Forward declaration of the interface
class  CDescriptionFileRW
{
private:
	char* m_lpcFileBuffer;
	char* m_lpcFileBuffer_copy; // unchanged m_lpcFileBuffer for delete[]
	UINT m_nFileSize = 0;
	std::vector< std::pair<char*,char*> > m_vLines;
public:
	UINT m_nCodepage = CP_ACP;
	UINT m_nBitOrder = BOM_NONE_MODE;
	UINT m_nTargetCodepage = CP_ACP;
	
	bool LoadFile(LPCTSTR filename);
	
	/**
	* @brief Looks for BOM (Byte Order Mark) in the open file 
	*
	* If BOM is found sets appropriate m_nCodepage and m_nBitOrder
	*
	* @return m_nBitOrder 
	*/
	UINT LookForBomInBuffer();
	
	bool ChangeLine(int number, std::wstring* line);
	bool SaveChanges();
	bool ConvertAndSaveChanges(UINT codepage);
	
	/**
	* @brief Finds and counts the number of lines in a description file.
	*
	* This method scans the description file associated with the 
	* CDescriptionFileRW object, finds '\n's and 
	* adds corresponding line-bonds into m_vLines
	*
	* @return size_t The number of lines found in the description file.
	*/
	size_t FindLines();
	
	/**
	* @brief Retrieves and converts a line based on the specified line number.
	*
	* This function takes a line number as input and retrieves the corresponding
	* line from a data source. The line is then converted to a wide string format
	* according to m_nCodepage and stored in the provided output parameter.
	*
	* @param number The line number to retrieve and convert.
	* @param line Pointer to a wide string where the converted line will be stored.
	*			  This parameter must not be null.
	* 
	* @return int line length
	* 
	* @note The output parameter `line` will be modified to contain the converted
	*	   line if the function succeeds.
	* 
	* @warning Ensure that the `line` pointer is valid and points to a writable
	*		  memory location before calling this function.
	*/
	int GetConvertedLine(int number, /*out*/ std::wstring* line);


	~CDescriptionFileRW();
};


#endif