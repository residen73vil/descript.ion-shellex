#include "shellext_init.h"

HRESULT __stdcall ShellExtInitComClass::Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID) {
	DEBUG_LOG(L"ShellExtInitComClass", "Init called") ;
	// Initialize your shell extension - store relevant info if needed
	UINT	uNumFiles;
	TCHAR	 szFile[MAX_PATH];
	TCHAR	 szFileName[MAX_PATH];
	TCHAR	 szFilePath[MAX_PATH];
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP	hDrop;
	bool	bSkipPathOptimization = false;

	// Look for CF_HDROP data in the data object. If there

	// is no such data, return an error back to Explorer.

	if ( FAILED( pDataObj->GetData ( &fmt, &stg ) ))
		return E_INVALIDARG;
	

	// Get a pointer to the actual data.
	hDrop = (HDROP) GlobalLock ( stg.hGlobal );
	
	// Make sure it worked.
	if ( NULL == hDrop )
		return E_INVALIDARG;

	uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 );
	HRESULT hr = S_OK;
	if ( 0 == uNumFiles )
	{
		GlobalUnlock ( stg.hGlobal );
		ReleaseStgMedium ( &stg );
		return E_INVALIDARG;
	}
 
	// Get the names of the file and store it in our
	for ( UINT uFile = 0; uFile < uNumFiles; uFile++ )
	{
		// Get the next filename.
		
		if ( 0 == DragQueryFile ( hDrop, uFile, szFile, MAX_PATH ) )
			continue;
		
		
		if ( GetFileNameAndPath(szFile, szFileName, szFilePath, bSkipPathOptimization) )
		{
			if (!bSkipPathOptimization) //if not skipping the path 
			{
				//make a copy of the path
				wcsncpy(m_szPath, szFilePath, MAX_PATH);
				DEBUG_LOG(L"ShellExtInitComClass::Initialize selected filepath", szFilePath);
				bSkipPathOptimization = true; //and start skipping in next iterations 
			}
			DEBUG_LOG(L"ShellExtInitComClass::Initialize selected filename", szFileName);
			// Add the filename to our list of files to act on.
			m_lsFiles.push_back ( szFileName );
		}
		
	}   // end for

	// Release resources.
	GlobalUnlock ( stg.hGlobal );
	ReleaseStgMedium ( &stg );
	
	// Check how many files were selected.  If the number is greater than the
	// maximum number of property pages, truncate our list.
	if ( m_lsFiles.size() > MAXPROPPAGES )
		m_lsFiles.resize ( MAXPROPPAGES );

	// If we found any files we can work with, return S_OK.  Otherwise,
	// return E_FAIL so we don't get called again for this right-click
	// operation. 

	return (m_lsFiles.size() > 0) ? S_OK : E_FAIL;

}

//Returns file name and path of an existing file
bool GetFileNameAndPath(LPCTSTR szFile, TCHAR* lpFileName, TCHAR* lpFilePath, bool bSkipPathOptimization) {
    // Pointer to hold the file part
    TCHAR fullPath[MAX_PATH];
	TCHAR *lpFilePart;
    // Get the full path
    DWORD result = GetFullPathName(szFile, MAX_PATH, fullPath, &lpFilePart);

    if (result > 0 && result < MAX_PATH) {
        // Copy the file name
        if (lpFilePart != nullptr) {
			wcsncpy(lpFileName, lpFilePart, MAX_PATH);
			// Null-terminate the path
			if (!bSkipPathOptimization)
			{
				*lpFilePart = '\0';
				wcsncpy(lpFilePath, fullPath, MAX_PATH);
			} else {
				*lpFilePath = '\0'; //skip path, set path to ""
			} 
		}
        return true; // Success
    } else {
        // Handle error
        return false; // Failure
    }
}