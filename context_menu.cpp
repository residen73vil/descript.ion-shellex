#include "context_menu.h"
#include "dbg.h"
HRESULT __stdcall ContextMenuComClass::Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID) {
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

HRESULT __stdcall ContextMenuComClass::QueryInterface(REFIID riid, void **ppv) {
	DEBUG_LOG_RIID(L"ObjectQuery:", riid) ;

	if (riid == IID_IUnknown || riid == IID_IContextMenuComClass) {
		*ppv = static_cast<IContextMenuComClass*>(this);
		AddRef();
		return NOERROR;
	}
	if ( riid == IID_IContextMenu) {
		*ppv = static_cast<IContextMenu*>(this);
		 	AddRef();
			return NOERROR;
		}
	if ( riid == IID_IShellExtInit) {
		*ppv = static_cast<IShellExtInit*>(this);
			AddRef();
			return NOERROR;
		}
		*ppv = nullptr;
		return E_NOINTERFACE;
}

ULONG __stdcall ContextMenuComClass::AddRef() {
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall ContextMenuComClass::Release() {
	ULONG count = InterlockedDecrement(&refCount);
	if (count == 0) {
		delete this;
	}
	return count;
}

	// IContextMenu methods
//Adds items to the menu
HRESULT __stdcall ContextMenuComClass::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst,
									UINT idCmdLast, UINT uFlags) {
	InsertMenu(hMenu, indexMenu, MF_BYPOSITION, idCmdFirst, L"My Extension");
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 1);
}

//Invoked when the item is clicked
HRESULT __stdcall ContextMenuComClass::InvokeCommand(LPCMINVOKECOMMANDINFO pici){
	if (!pici)
	{
		return E_INVALIDARG;
	}

	if (IS_INTRESOURCE(pici->lpVerb)){ //checking whether lpVerb is a menu offset or a canonical verb (RunAs/Open/Preview etc)
		MessageBox(NULL, this->m_szFile, L"Info", MB_OK);
		return S_OK;
	} else {
		return E_NOTIMPL; //if lpVerb contains canonical verb, return not implemented
	}
}

//Shows a comment for the menu item in the status bar
HRESULT __stdcall ContextMenuComClass::GetCommandString(UINT_PTR idCmd, UINT uType, UINT *pwResv, LPSTR pszName, UINT cchMax) {
	//todo: implement if menu item persists
	return S_OK;
}
