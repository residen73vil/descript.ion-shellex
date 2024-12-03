#include "property_sheet.h"

//TODO: Process several files not just one


HRESULT __stdcall ShellPropSheetExtComClass::Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID) {
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

HRESULT __stdcall ShellPropSheetExtComClass::QueryInterface(REFIID riid, void **ppv) {
	DEBUG_LOG_RIID(L"ObjectQueryPropertySheet:", riid) ;

	if (riid == IID_IUnknown || riid == IID_IShellPropSheetExtComClass) {
		*ppv = static_cast<IShellPropSheetExtComClass*>(this);
		AddRef();
		return NOERROR;
	}
	if ( riid == IID_IShellPropSheetExt) {
		*ppv = static_cast<IShellPropSheetExt*>(this);
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

ULONG __stdcall ShellPropSheetExtComClass::AddRef() {
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall ShellPropSheetExtComClass::Release() {
	ULONG count = InterlockedDecrement(&refCount);
	if (count == 0) {
		delete this;
	}
	return count;
}

// IShellPropSheetExt methods
//Adds page to the property sheets window
HRESULT __stdcall ShellPropSheetExtComClass::AddPages ( LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam ) {
	DEBUG_LOG(L"property_sheet", L"test");
	PROPSHEETPAGE  psp;
	HPROPSHEETPAGE hPage;


	psp.dwSize	  = sizeof(PROPSHEETPAGE);
	psp.dwFlags	 = PSP_USEREFPARENT | PSP_USETITLE |
						PSP_USEICONID | PSP_USECALLBACK;
	psp.hInstance   = g_dll_hInstance;
	psp.pszTemplate = MAKEINTRESOURCE(IDD_COMMENT_PROPPAGE);
	psp.pszIcon	 = MAKEINTRESOURCE(IDI_TAB_ICON);
	psp.pszTitle	= L"test";
	psp.pfnDlgProc  = PropPageDlgProc;
	psp.lParam	  = (LPARAM) m_szFile;
	psp.pfnCallback = PropPageCallbackProc;
	psp.pcRefParent = (UINT*) &g_cActiveComponents;
	hPage = CreatePropertySheetPage ( &psp );


	if ( NULL != hPage )
	  {
	  // Call the shell's callback function, so it adds the page to

	  // the property sheet.

	  if ( !lpfnAddPageProc ( hPage, lParam ) )
	  	DestroyPropertySheetPage ( hPage );
	  }

	
  return S_OK;
}

//Callbacks for the page in the property sheets window

INT_PTR CALLBACK PropPageDlgProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){
	BOOL bRet = FALSE;

	switch ( uMsg )
		{
		case WM_INITDIALOG:
			bRet = FALSE;//OnInitDialog ( hwnd, lParam );
		break;

		case WM_NOTIFY:
			{
			NMHDR* phdr = (NMHDR*) lParam;

			switch ( phdr->code )
				{
				case PSN_APPLY:
					bRet = TRUE;// OnApply ( hwnd, (PSHNOTIFY*) phdr );
				break;

				//TODO: Activatin apply button doesn't work, needs to be fixed
				case WM_COMMAND:
					// Handle command messages, including notifications from controls
					if (HIWORD(wParam) == EN_CHANGE)
						// If the user changes any of the DTP controls, enable
						// the Apply button.
						SendMessage ( GetParent(hwnd), PSM_CHANGED, (WPARAM) hwnd, 0 ); //TODO: doesn't work, 
				break;
				}
			}
		break;
		}

	return bRet;
}

UINT CALLBACK PropPageCallbackProc ( HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp ){
		//if ( PSPCB_RELEASE == uMsg )
		//free ( (void*) ppsp->lParam ); // should contain file names, and should be freed upon exit!!!

	return 1;   // used for PSPCB_CREATE - let the page be created
}
