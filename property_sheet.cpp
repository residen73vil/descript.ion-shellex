#include "property_sheet.h"

//TODO: Process several files not just one

CDescriptionHandler* description;


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
	DEBUG_LOG(L"ShellPropSheetExtComClass", "refcount incremented") 
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall ShellPropSheetExtComClass::Release() {
	DEBUG_LOG(L"ShellPropSheetExtComClass", "refcount decremented") 
	ULONG count = InterlockedDecrement(&refCount);
	if (count == 0) {
		DEBUG_LOG(L"ShellPropSheetExtComClass", "object deleted") 
		delete this;
	}
	return count;
}

// IShellPropSheetExt methods
//Adds page to the property sheets window
HRESULT __stdcall ShellPropSheetExtComClass::AddPages ( LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam ) {
	
	bool only_one_file_selected = (m_lsFiles.size() == 1) ? true : false;
	if ( m_lsFiles.size() < 1)
		return S_FALSE;
	description = new CDescriptionHandler();
	description->LoadPath(m_szPath);
	
	
	string_list::iterator it = m_lsFiles.begin();
	for (string_list::iterator it =  m_lsFiles.begin(); it !=  m_lsFiles.end(); ++it) 
	{
		DEBUG_LOG(L"property_sheet file", *it);
		
		PROPSHEETPAGE  psp;
		HPROPSHEETPAGE hPage;
		
	
		psp.dwSize	  = sizeof(PROPSHEETPAGE);
		psp.dwFlags	 = PSP_USEREFPARENT | PSP_USETITLE |
							PSP_USEICONID | PSP_USECALLBACK;
		psp.hInstance   = g_dll_hInstance;
		psp.pszTemplate = MAKEINTRESOURCE(IDD_COMMENT_PROPPAGE);
		psp.pszIcon	 = MAKEINTRESOURCE(IDI_TAB_ICON);
		
		std::basic_string<TCHAR> *fname	= new std::basic_string<TCHAR>(*it);
		

		psp.pszTitle	= only_one_file_selected ? _T("Comment") : static_cast<LPCWSTR>(fname->c_str());
		psp.pfnDlgProc  = PropPageDlgProc;
		psp.lParam	  = (LPARAM) fname;
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
	}
	
  return S_OK;
}

//Callbacks for the page in the property sheets window

INT_PTR CALLBACK PropPageDlgProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){
	BOOL bRet = FALSE;
	DEBUG_LOG("PropPageDlgProc MSG", uMsg)
	switch ( uMsg )
		{
		case WM_INITDIALOG:
			bRet = OnInitDialog( hwnd, lParam ); 
		break;
		case WM_COMMAND:
			DEBUG_LOG("	WM_COMMAND", HIWORD(wParam))
			DEBUG_LOG("	WM_COMMAND LOW", LOWORD(wParam))
			if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == IDC_TEXT)
				SendMessage ( GetParent(hwnd), PSM_CHANGED, (WPARAM) hwnd, 0 );
		break;
		case WM_NOTIFY:
			{
			NMHDR* phdr = (NMHDR*) lParam;
			DEBUG_LOG("		PropPageDlgProc WM_NOTIFY code", phdr->code)
			DEBUG_LOG("		PropPageDlgProc WM_NOTIFY wParam", HIWORD(wParam))

			switch ( phdr->code )
				{
				case PSN_APPLY:
					DEBUG_LOG("	WM_NOTIFY", "Apply")
					bRet = TRUE;// OnApply ( hwnd, (PSHNOTIFY*) phdr );
				break;
				}
			}
		break;
		}

	return bRet;
}

UINT CALLBACK PropPageCallbackProc ( HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp ){
		DEBUG_LOG("PropPageCallbackProc:", uMsg);
		if ( PSPCB_RELEASE == uMsg ){
			DEBUG_LOG("PropPageDlgProc name is freed", *(reinterpret_cast<std::basic_string<TCHAR>*>(ppsp->lParam)));
			delete ( reinterpret_cast<std::basic_string<TCHAR>*>(ppsp->lParam) ); // a string containing file name, is freed upon exit
			delete description;
		}

	return 1;   // used for PSPCB_CREATE - let the page be created
}
BOOL OnInitDialog ( HWND hwnd, LPARAM lParam ){
		HWND hEditControl = GetDlgItem(hwnd, IDC_TEXT);
		std::basic_string<TCHAR> comment;
		PROPSHEETPAGE* psp = (PROPSHEETPAGE*)lParam;
		std::basic_string<TCHAR> *fname = (std::basic_string<TCHAR>*)(psp->lParam);
		if ( description->ReadComment( fname->c_str(), comment ) ){
			SetWindowText(hEditControl, comment.c_str());
		}else{
			SetWindowText(hEditControl, L"");
		}
		SetFocus(hEditControl);
		return TRUE; //don't override focus with default
}
