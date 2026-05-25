#include "property_sheet.h"
//TODO: Process several files not just one

CDescriptionHandler* description = nullptr;
bool are_changes_to_applay_present = false;


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
	if (nullptr == description){ //create only one CDescriptionHandler
		description = new CDescriptionHandler();
		description->LoadPath(m_szPath);
	}
	
	string_list* file_names = new string_list(m_lsFiles);
	DEBUG_LOG("\t\t\tPropSheet add pages - file names array:",file_names);
	PROPSHEETPAGE  psp;
	HPROPSHEETPAGE hPage;
	

	psp.dwSize	  = sizeof(PROPSHEETPAGE);
	psp.dwFlags	 = PSP_USEREFPARENT | PSP_USETITLE |
						PSP_USEICONID | PSP_USECALLBACK;
	psp.hInstance   = g_dll_hInstance;
	psp.pszTemplate = MAKEINTRESOURCE(IDD_COMMENT_PROPPAGE);
	psp.pszIcon	 = MAKEINTRESOURCE(IDI_TAB_ICON);
	

	psp.pszTitle = _T("Comments");
	psp.pfnDlgProc  = PropPageDlgProc;
	psp.lParam = (LPARAM) file_names;
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
	DEBUG_LOG("PropPageDlgProc MSG", uMsg)
	switch ( uMsg )
		{
		case WM_INITDIALOG:
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lParam); //save PROPSHEETPAGE pointer
			bRet = OnInitDialog( hwnd, lParam ); 
		break;
		case WM_COMMAND:
			DEBUG_LOG("	WM_COMMAND", HIWORD(wParam))
			DEBUG_LOG("	WM_COMMAND LOW", LOWORD(wParam))
			if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == IDC_TEXT){
				// Get the handle to the edit control
				HWND hEdit = (HWND)lParam; // lParam contains the handle to the control that sent the message

				// Retrieve the PROPSHEETPAGE pointer 
				PROPSHEETPAGE* psp = (PROPSHEETPAGE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
				std::basic_string<TCHAR> *fname = (std::basic_string<TCHAR>*)(psp->lParam);
				
				// Get the length of the text in the edit control
				int length = GetWindowTextLength(hEdit);

				// Allocate a buffer to hold the text
				wchar_t* buffer = new wchar_t[length + 1]; // +1 for the null terminator

				// Retrieve the text from the edit control
				GetWindowText(hEdit, buffer, length + 1);

				//add changes
				description->AddChangeComment(fname->c_str(),buffer);
				delete[] buffer;

				//enable apply button
				SendMessage ( GetParent(hwnd), PSM_CHANGED, (WPARAM) hwnd, 0 );
				are_changes_to_applay_present = true;

				DEBUG_LOG("\t\t\tdesctiption changed","");
			}
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
					if (are_changes_to_applay_present){
					//press of OK doesn't causes second apply due to are_changes_to_applay_present flag check but
					//TODO: several property dialogs cause override of description, so whole architecture should be changed
					//to accommodate for that!
						description->SaveChanges();
						are_changes_to_applay_present = false;
					}
					bRet = TRUE;// OnApply ( hwnd, (PSHNOTIFY*) phdr );
				break;
				case TCN_SELCHANGE:
					int iSel = TabCtrl_GetCurSel(phdr->hwndFrom);
					ShowTabPage(iSel, hwnd);
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
			DEBUG_LOG("PropPageDlgProc name list is freed", ppsp->lParam);
			delete ( reinterpret_cast<string_list*>(ppsp->lParam) ); // the vector containing file names, is freed upon exit
			if (nullptr != description){
				//delete only one description instance
				delete description;
				description = nullptr;
			}
		}

	return 1; // used for PSPCB_CREATE - let the page be created
}

INT_PTR CALLBACK TabControlDlgProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){
	BOOL bRet = FALSE;
	DEBUG_LOG("\tTabControl MSG", uMsg)
	switch ( uMsg )
		{
		case WM_COMMAND:
			DEBUG_LOG("tubControl	WM_COMMAND", HIWORD(wParam))
			DEBUG_LOG("tubControl	WM_COMMAND LOW", LOWORD(wParam))
			if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == IDC_TEXT){
				// Get the handle to the edit control
				HWND hEdit = (HWND)lParam; // lParam contains the handle to the control that sent the message
				HWND hPropertySheet = GetParent(hwnd);
				HWND hTabControl = GetDlgItem(hPropertySheet, IDC_TABCTRL);

				DEBUG_LOG("Changes in the edit tab handler", hTabControl);
				int iSel = TabCtrl_GetCurSel(hTabControl);

				string_list* file_names = (string_list*) GetWindowLongPtr(hPropertySheet, GWLP_USERDATA);
				string_list::iterator itFileName = file_names->begin();
				std::advance(itFileName, iSel);

				// Get the length of the text in the edit control
				int length = GetWindowTextLength(hEdit);

				// Allocate a buffer to hold the text
				wchar_t* buffer = new wchar_t[length + 1]; // +1 for the null terminator

				// Retrieve the text from the edit control
				GetWindowText(hEdit, buffer, length + 1);

				//add changes
				description->AddChangeComment(itFileName->c_str(),buffer);
				DEBUG_LOG("Changes in the edit happened", buffer);
				delete[] buffer;

				//enable apply button
				HWND prop_sheet_that_changed = GetParent(hwnd);
				HWND parent_that_prop_sheet = GetParent(prop_sheet_that_changed );
				SendMessage ( parent_that_prop_sheet, PSM_CHANGED, (WPARAM) prop_sheet_that_changed, 0 );
				are_changes_to_applay_present = true;

			}
		break;
		}

	return bRet;
}

BOOL OnInitDialog ( HWND hwnd, LPARAM lParam ){
		HWND hTabControl = GetDlgItem(hwnd, IDC_TABCTRL);
		string_list* file_names = (string_list*) ((LPPROPSHEETPAGE) lParam)->lParam;
		DEBUG_LOG("\t\t\tCreation of tab pages - file names array:",file_names);

		TCITEM tie = {0};
		tie.mask = TCIF_TEXT;
		HWND hCommentTab = CreateDialog(g_dll_hInstance, MAKEINTRESOURCE(IDD_COMMENT), hwnd, TabControlDlgProc);
		SetWindowLongPtr(hCommentTab, GWLP_ID, static_cast<LONG_PTR>(IDD_COMMENT_TAB));
		HWND hSettingsTab = CreateDialog(g_dll_hInstance, MAKEINTRESOURCE(IDD_SETTINGS), hwnd, TabControlDlgProc);
		SetWindowLongPtr(hSettingsTab, GWLP_ID, static_cast<LONG_PTR>(IDD_SETTINGS_TAB));

		DEBUG_LOG("\t\t\tCreation of tab pages","");
		//creating pages in tab
		bool only_one_file_selected = (file_names->size() == 1) ? true : false;
		UINT nTab = 0;
		for (string_list::iterator it =  file_names->begin(); it !=  file_names->end(); ++it)
		{
			DEBUG_LOG(L"Init dialog file:", *it);
			TCHAR fname[MAX_TAB_LENGTH] = {0};
			UINT str_length = it->size() >= MAX_TAB_LENGTH ? MAX_TAB_LENGTH-1 : it->size();
			std::wmemcpy(fname, it->data(), str_length);
			if (it->size() >= MAX_TAB_LENGTH)
				{ std::wmemcpy(&fname[MAX_TAB_LENGTH-4], _T("..."), 3); }
			tie.pszText = only_one_file_selected ? (LPWSTR) _T("Comment") : (LPWSTR) fname;
			TabCtrl_InsertItem(hTabControl, nTab, &tie);
			nTab++;
			if (nTab > MAX_TAB_NUMBER) break;
		}
		tie.pszText = _T("Settings");
		TabCtrl_InsertItem(hTabControl, MAX_TAB_NUMBER+1, &tie);

		// Attach number of pages to the tab control
		SetWindowLongPtr(hTabControl, GWLP_USERDATA, (LONG_PTR)nTab);
		// Attach array of file names to the property sheet
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)file_names);
		// -------------------------------------------------
		// 3. Position them inside the tab’s client rectangle
		// -------------------------------------------------
		RECT rcTab;
		GetClientRect(hTabControl, &rcTab);				// tab’s client area (no border)
		MapWindowPoints(hTabControl, hwnd, (POINT*)&rcTab, 2); // to parent‑dialog coords
		TabCtrl_AdjustRect(hTabControl, FALSE, &rcTab);
		// Size each page to fill that rect
		SetWindowPos(hCommentTab, nullptr,
					 rcTab.left, rcTab.top,
					 rcTab.right - rcTab.left,
					 rcTab.bottom - rcTab.top,
					 SWP_SHOWWINDOW);
		SetWindowPos(hSettingsTab, nullptr,
					 rcTab.left, rcTab.top,
					 rcTab.right - rcTab.left,
					 rcTab.bottom - rcTab.top,
					 SWP_SHOWWINDOW);

		// Show only the first page initially
		ShowWindow(hCommentTab, SW_SHOW);
		ShowWindow(hSettingsTab, SW_HIDE);

		HWND hEditControl = GetDlgItem(hCommentTab, IDC_TEXT);
		std::basic_string<TCHAR> comment;
		std::basic_string<TCHAR> *fname = &file_names->front();
		if ( description->ReadComment( fname->c_str(), comment ) ){
			SetWindowText(hEditControl, comment.c_str());
		}else{
			SetWindowText(hEditControl, L"");
		}
		SetFocus(hEditControl);
		return TRUE; //don't override focus with default
}

void ShowTabPage(int iSel, HWND hwnd){
	HWND hEditTab = GetDlgItem(hwnd, IDD_COMMENT_TAB);
	HWND hEditControl = GetDlgItem(hEditTab, IDC_TEXT);
	HWND hSettingsTab = GetDlgItem(hwnd, IDD_SETTINGS_TAB);
	HWND hTabControl = GetDlgItem(hwnd, IDC_TABCTRL);
	int tabCount = TabCtrl_GetItemCount(hTabControl);
	ShowWindow(hSettingsTab, (iSel == tabCount-1) ? SW_SHOW : SW_HIDE);
	ShowWindow(hEditTab, (iSel != tabCount-1) ? SW_SHOW : SW_HIDE);
	if (iSel != tabCount-1){
		string_list* file_names = (string_list*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		string_list::iterator it = file_names->begin();
		std::advance(it, iSel);
		DEBUG_LOG("change tab to",*it);
		std::basic_string<TCHAR> comment;
		if ( description->ReadCommentWithChanges( it->c_str(), comment ) ){
			SetWindowText(hEditControl, comment.c_str());
		}else{
			SetWindowText(hEditControl, L"");
		}
	}
}
