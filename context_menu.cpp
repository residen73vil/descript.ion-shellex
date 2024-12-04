#include "context_menu.h"
#include "dbg.h"

HRESULT __stdcall ContextMenuComClass::QueryInterface(REFIID riid, void **ppv) {
	DEBUG_LOG_RIID(L"ContextMenuComClass", riid) ;

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
		DEBUG_LOG(L"ContextMenuComClass", "return IShellExtInit") ;
		*ppv = static_cast<ShellExtInitComClass*>(this);
			AddRef();
			return NOERROR;
		}
		*ppv = nullptr;
		return E_NOINTERFACE;
}

ULONG __stdcall ContextMenuComClass::AddRef() {
	DEBUG_LOG(L"ContextMenuComClass", "refcount incremented") 
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall ContextMenuComClass::Release() {
	DEBUG_LOG(L"ContextMenuComClass", "refcount decremented") 
	ULONG count = InterlockedDecrement(&refCount);
	if (count == 0) {
		DEBUG_LOG(L"ContextMenuComClass", "object deleted ")
		delete this;
	}
	return count;
}

	// IContextMenu methods
//Adds items to the menu
HRESULT __stdcall ContextMenuComClass::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst,
									UINT idCmdLast, UINT uFlags) {
	DEBUG_LOG(L"ContextMenuComClass", "insert elements into the menu") ;
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
