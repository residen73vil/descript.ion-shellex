#ifndef DESCRIPTION_FILE_RW_H
#define DESCRIPTION_FILE_RW_H
#include <windows.h>
#include <fileapi.h>
#include <tchar.h>
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <map>
#include <tuple>
#include "dbg.h" 
#include "file_io_wrapper.h"
#include "errors_and_settings.h"

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
//holds initial size of a line, size of the line to write and pointer to the line (must be freed after use!)
typedef std::tuple< size_t, size_t, char* > tuple_2_sizes_and_ptr;

// Forward declaration of the interface
class  CDescriptionFileRW
{
private:
	char* m_lpcFileBuffer;
	IOWrapper m_file_io;
	char* m_lpcFileBuffer_to_write; // buuffer with
	UINT m_nFileSize = 0;
	UINT m_nFileSizeWithoutBOM = 0;
	std::vector< std::pair<char*,char*> > m_vLines; //line bonds in m_lpcFileBuffer
	std::map<int,std::wstring> m_mChanges; //lines to be replaced 
public:
	std::wstring m_sFilename = L"";
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
	
	/**
	 * @brief Changes a specific line in descript.ion file.
	 *
	 * This function modifies the line at the specified index 
	 * with the content provided in the `line` parameter.
	 * The function doesn't ensure that the index is valid
	 * before attempting to make the change.
	 *
	 * @param number The index of the line to be changed. This should be
	 *               a zero-based index. If the index is out of bounds,
	 *               the function may return false.
	 * @param line A pointer to a std::wstring containing the new content
	 *             for the specified line. The content will replace the
	 *             existing line at the given index.
	 *
	 * @return true if the line was successfully changed; false otherwise
	 *
	 * @note Ensure that the `line` pointer is not null before calling
	 *       this function, and the index is in bounds. The function 
	 *       may also log errors if the operation fails.
	 */
	bool ChangeLine(int number, std::wstring* line);
	bool SaveChanges();
	
	/**
	 * @brief Converts lines in m_mChanges to the specified code page.
	 *
	 * This function takes a code page identifier and a map of changes, 
	 * converting the changes to the specified code page format. 
	 * The changes are represented as a map where the key is an integer 
	 * and the value is a tuple containing two sizes and a pointer.
	 *
	 * @param codepage The code page to which the changes should be converted.
	 * @param changes_converted A reference to a map that will be populated 
	 *                          with the converted changes. The map's keys 
	 *                          are integers, and the values are tuples 
	 *                          containing two sizes and a pointer.
	 *
	 * @return true if the conversion was successful, false otherwise.
	 */
	bool ConvertChangesToCodePage(UINT codepage, std::map<int, tuple_2_sizes_and_ptr>& changes_cvonverted);
	/**
	 * @brief Converts data to a specified codepage and saves the changes.
	 * 
	 * This function takes a codepage as input, converts the lines added with
	 * ChangeLine converts them to the specified codepage, and saves the changes
	 * to the descript.ion file (only changed lines are overvriten. The conversion 
	 * process may involve encoding transformations, and the function will handle
	 * any necessary memory management.
	 *
	 * @param codepage The codepage to which the data should be converted.
	 *                 A Windows codepage identifier (e.g., CP_UTF8, CP_ACP).
	 *
	 * @return true if the conversion and saving were successful; false otherwise.
	 *
	 * @note Ensure that the data to be converted is properly initialized
	 *       before calling this function. The function may also log errors
	 *       or warnings if the conversion fails.
	 */
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