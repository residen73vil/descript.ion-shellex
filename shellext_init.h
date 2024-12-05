#ifndef SHELLEXT_INIT_H
#define SHELLEXT_INIT_H
#include <objbase.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlguid.h>
#include <windows.h>
#include <fileapi.h>
#include <commctrl.h>
#include <string>
#include <list>
#include "resource.h"
#include "dbg.h" 

typedef std::list< std::basic_string<TCHAR> > string_list;

// Forward declaration of the interface
class  IShellExtInitComClass :
	public IShellExtInit
 {
public:
	virtual HRESULT __stdcall Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID) override = 0;
};


// Implement the COM Class
class ShellExtInitComClass : public IShellExtInitComClass {
protected:
	string_list m_lsFiles;
	TCHAR m_szPath[MAX_PATH];
public:

	HRESULT __stdcall Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID) override ;

};

/**
 * @brief Retrieves the file name and path from a given file string.
 *
 * This function extracts the file name and the file path from the specified
 * file string. Depending on the value of the `bSkipPath` parameter, it can
 * either return just the file name or both the file name and the path.
 *
 * @param szFile [in] The input file string from which to extract the file name
 *                   and path. This should be a valid file path.
 * @param lpFileName [out] A pointer to a buffer that receives the file name.
 *                         This buffer should be large enough to hold the file name.
 * @param lpFilePath [out] A pointer to a buffer that receives the file path.
 *                         This buffer should be large enough to hold the file path.
 * @param bSkipPath [in] A boolean flag that determines whether to skip the path.
 *                       If true, only the file name will be returned. If false,
 *                       both the file name and path will be returned.
 *
 * @return Returns true if the operation was successful, false otherwise.
 *
 * @note Ensure that the buffers pointed to by lpFileName and lpFilePath are
 *       properly allocated and large enough to hold the expected results.
 *
 * @warning This function does not perform any validation on the input file string.
 *          Ensure that szFile is a valid and accessible file path.
 */
bool GetFileNameAndPath(LPCTSTR szFile, TCHAR* lpFileName, TCHAR* lpFilePath, bool bSkipPath);

#endif