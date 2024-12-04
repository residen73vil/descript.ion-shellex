#include "shellext_init.h"

//TODO: Process several files not just one


HRESULT __stdcall ShellExtInitComClass::Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID) {
	DEBUG_LOG(L"ShellExtInitComClass", "Init called") ;
	// Initialize your shell extension - store relevant info if needed
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP	hDrop;

	// Look for CF_HDROP data in the data object. If there

	// is no such data, return an error back to Explorer.

	if ( FAILED( pDataObj->GetData ( &fmt, &stg ) ))
		return E_INVALIDARG;
 
	// Get a pointer to the actual data.

	hDrop = (HDROP) GlobalLock ( stg.hGlobal );
 
	// Make sure it worked.

	if ( NULL == hDrop )
		return E_INVALIDARG;
	UINT uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 );
	HRESULT hr = S_OK;
	
	if ( 0 == uNumFiles )
		{
		GlobalUnlock ( stg.hGlobal );
		ReleaseStgMedium ( &stg );
		return E_INVALIDARG;
		}
 
	// Get the name of the first file and store it in our

	// member variable m_szFile.

	if ( 0 == DragQueryFile ( hDrop, 0, m_szFile, MAX_PATH ) )
		hr = E_INVALIDARG;
 
	GlobalUnlock ( stg.hGlobal );
	ReleaseStgMedium ( &stg );
 
	return hr;
}