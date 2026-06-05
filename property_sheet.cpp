#include "property_sheet.h"

// Structure of state of current property sheet
struct PropSheetAttachments{
	CDescriptionHandler description;
	string_list file_names;
	bool are_changes_to_apply_present;
	bool are_settings_to_apply_present;
	std::wstring hidden_cashed_prog_data;
	MultiLineStyle current_line_multiline_style;
	CSettings settings;
};

// One entry shown in the combo box
struct CodePageItem
{
    UINT    cp;          // numeric code‑page identifier (e.g. 1252)
    std::wstring name;   // what the user sees
};
void FillCodePageCombo(HWND hDlg, int comboId);
void PopulateCodePageList();
BOOL CALLBACK CPEnumProc(LPTSTR lpCodePageString);
void SelectCodePageCombo(HWND hDlg, int comboId,int cpId );
static std::vector<CodePageItem> g_codePages;   // global or member of a Settings class
// Com object stuff
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
	PropSheetAttachments* pAttachments = new PropSheetAttachments();
	pAttachments->description.LoadPath(m_szPath);
	
	pAttachments->file_names = string_list(m_lsFiles);
	DEBUG_LOG("\t\t\tPropSheet add pages - file names array:",&(pAttachments->file_names));
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
	psp.lParam = (LPARAM) pAttachments;
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
			//SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lParam); //save PROPSHEETPAGE pointer
			bRet = OnInitDialog( hwnd, lParam ); 
		break;
		case WM_NOTIFY:
			{
			NMHDR* phdr = (NMHDR*) lParam;
			DEBUG_LOG("		PropPageDlgProc WM_NOTIFY code", phdr->code)
			DEBUG_LOG("		PropPageDlgProc WM_NOTIFY wParam", HIWORD(wParam))

			switch ( phdr->code )
				{
				case PSN_APPLY:{
					DEBUG_LOG("	WM_NOTIFY", "Apply")
					PropSheetAttachments *pAttachments = (PropSheetAttachments*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
					if (pAttachments->are_settings_to_apply_present){
						CErrorsAndSettings::getInstance().setSettings(pAttachments->settings);
						pAttachments->are_settings_to_apply_present = false;
					}
					if (pAttachments->are_changes_to_apply_present){
						pAttachments->description.SaveChanges();
						pAttachments->are_changes_to_apply_present = false;
					}
					bRet = TRUE;// OnApply ( hwnd, (PSHNOTIFY*) phdr );
				}
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
			DEBUG_LOG("PropPageDlgProc freeing pAttachments", ppsp->lParam);
			delete ( reinterpret_cast<PropSheetAttachments*>(ppsp->lParam) );
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
			if (HIWORD(wParam) == BN_CLICKED){
				HWND hPropertySheet = GetParent(hwnd);
				PropSheetAttachments* pAttachments = (PropSheetAttachments*) GetWindowLongPtr(hPropertySheet, GWLP_USERDATA);
				switch (LOWORD(wParam)){
				case IDC_RADIO_NL_AUTO:{
					pAttachments->settings.MultiLineStyle = AUTO;
				break;}
				case IDC_RADIO_NL_DBL:{
					pAttachments->settings.MultiLineStyle = DOUBLECMD;
				break;}
				case IDC_RADIO_NL_TC:{
					pAttachments->settings.MultiLineStyle = TOTALCMD;
				break;}
				case IDC_RADIO_NL_NONE:{
					pAttachments->settings.MultiLineStyle = NONE;
				break;}
				case IDC_CHK_BOM:{
					bool isChecked = IsDlgButtonChecked(hwnd, IDC_CHK_BOM) == BST_CHECKED;
					pAttachments->settings.bom = isChecked;
				break;}
				}
				//enable apply button
				if (LOWORD(wParam) >= IDC_CHK_BOM && LOWORD(wParam) <= IDC_RADIO_NL_NONE){
					HWND hParentOfPropSheet = GetParent(hPropertySheet);
					SendMessage ( hParentOfPropSheet, PSM_CHANGED, (WPARAM) hPropertySheet, 0 );
					pAttachments->are_settings_to_apply_present = true;
				}
			}
			if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == IDC_COMBO_CP){
				HWND hCombo = GetDlgItem(hwnd, IDC_COMBO_CP);
				int sel = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
				if (sel != CB_ERR){
					HWND hPropertySheet = GetParent(hwnd);
					PropSheetAttachments* pAttachments = (PropSheetAttachments*) GetWindowLongPtr(hPropertySheet, GWLP_USERDATA);
					pAttachments->settings.cp = g_codePages[sel].cp;
					HWND prop_sheet_that_changed = GetParent(hwnd);
					HWND parent_that_prop_sheet = GetParent(prop_sheet_that_changed );
					SendMessage ( parent_that_prop_sheet, PSM_CHANGED, (WPARAM) prop_sheet_that_changed, 0 );
					pAttachments->are_settings_to_apply_present = true;
				}
			}
			if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == IDC_TEXT){
				// Get the handle to the edit control
				HWND hEdit = (HWND)lParam; // lParam contains the handle to the control that sent the message
				HWND hPropertySheet = GetParent(hwnd);
				HWND hTabControl = GetDlgItem(hPropertySheet, IDC_TABCTRL);

				DEBUG_LOG("Changes in the edit tab handler", hTabControl);
				int iSel = TabCtrl_GetCurSel(hTabControl);

				PropSheetAttachments* pAttachments = (PropSheetAttachments*) GetWindowLongPtr(hPropertySheet, GWLP_USERDATA);
				string_list* file_names = &pAttachments->file_names;
				string_list::iterator itFileName = file_names->begin();
				std::advance(itFileName, iSel);

				// Get the length of the text in the edit control
				int length = GetWindowTextLength(hEdit);

				// Allocate a buffer to hold the text
				wchar_t* buffer = new wchar_t[length + 1]; // +1 for the null terminator

				// Retrieve the text from the edit control
				GetWindowText(hEdit, buffer, length + 1);

				//treat new lines
				MultiLineStyle mode;
				if (pAttachments->settings.MultiLineStyle == AUTO){
					mode = pAttachments->current_line_multiline_style;
				}else{
					mode = pAttachments->settings.MultiLineStyle;
				}
				std::basic_string<TCHAR> commentWithNewLines = buffer;
				std::basic_string<TCHAR> comment;

				pAttachments->description.Multilinefy(commentWithNewLines, comment,
													pAttachments->hidden_cashed_prog_data,
													mode);

				//add changes
				pAttachments->description.AddChangeComment(itFileName->c_str(),comment.c_str());
				DEBUG_LOG("Changes in the edit happened", buffer);
				delete[] buffer;

				//enable apply button
				HWND prop_sheet_that_changed = GetParent(hwnd);
				HWND parent_that_prop_sheet = GetParent(prop_sheet_that_changed );
				SendMessage ( parent_that_prop_sheet, PSM_CHANGED, (WPARAM) prop_sheet_that_changed, 0 );
				pAttachments->are_changes_to_apply_present = true;

			}
		break;
		}

	return bRet;
}

BOOL OnInitDialog ( HWND hwnd, LPARAM lParam ){
	HWND hTabControl = GetDlgItem(hwnd, IDC_TABCTRL);
	PropSheetAttachments* pAttachments = (PropSheetAttachments*) ((LPPROPSHEETPAGE) lParam)->lParam;
	string_list* file_names = &(pAttachments->file_names);
	DEBUG_LOG("\t\t\tCreation of tab pages - file names array:",file_names);

	TCITEM tie = {0};
	tie.mask = TCIF_TEXT;
	HWND hCommentTab = CreateDialog(g_dll_hInstance, MAKEINTRESOURCE(IDD_COMMENT), hwnd, TabControlDlgProc);
	SetWindowLongPtr(hCommentTab, GWLP_ID, static_cast<LONG_PTR>(IDD_COMMENT_TAB));
	HWND hSettingsTab = CreateDialog(g_dll_hInstance, MAKEINTRESOURCE(IDD_SETTINGS), hwnd, TabControlDlgProc);
	SetWindowLongPtr(hSettingsTab, GWLP_ID, static_cast<LONG_PTR>(IDD_SETTINGS_TAB));

	DEBUG_LOG("\t\t\tCreation of tab pages","");
	// -------------------------------------------------
	// Creating pages in tab
	// -------------------------------------------------
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

	// -------------------------------------------------
	// Creating settings tab
	// -------------------------------------------------
	tie.pszText = _T("Settings");
	TabCtrl_InsertItem(hTabControl, MAX_TAB_NUMBER+1, &tie);
	//loading settings
	pAttachments->settings = CErrorsAndSettings::getInstance().getSettings();
	// Setting default ratio button in multiline style setting
	switch (pAttachments->settings.MultiLineStyle){
		case DOUBLECMD:
			CheckDlgButton(hSettingsTab, IDC_RADIO_NL_DBL, BST_CHECKED);
		break;
		case TOTALCMD:
			CheckDlgButton(hSettingsTab, IDC_RADIO_NL_TC, BST_CHECKED);
		break;
		case NONE:
			CheckDlgButton(hSettingsTab, IDC_RADIO_NL_NONE, BST_CHECKED);
		break;
		case AUTO:
		default:
			CheckDlgButton(hSettingsTab, IDC_RADIO_NL_AUTO, BST_CHECKED);
		break;
	}
	// Set bom checkbox
	CheckDlgButton(hSettingsTab, IDC_CHK_BOM, pAttachments->settings.bom ? BST_CHECKED : BST_UNCHECKED);
	// Fill up codepage combobox
	PopulateCodePageList();
	FillCodePageCombo(hSettingsTab, IDC_COMBO_CP);
	SelectCodePageCombo(hSettingsTab, IDC_COMBO_CP, pAttachments->settings.cp);
	// -------------------------------------------------
	// Attach attach all necessary data to the property sheet
	// -------------------------------------------------
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pAttachments);
	// -------------------------------------------------
	// Position them inside the tab’s client rectangle
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
	std::basic_string<TCHAR> commentWithNewLines;
	std::basic_string<TCHAR> *fname = &file_names->front();
	if ( pAttachments->description.ReadComment( fname->c_str(), comment ) ){
		pAttachments->current_line_multiline_style =
				pAttachments->description.Demultilinefy(comment, commentWithNewLines,
										pAttachments->hidden_cashed_prog_data,
										pAttachments->settings.MultiLineStyle);
		SetWindowText(hEditControl, commentWithNewLines.c_str());
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
	// If anything but settings selected show editor tab
	ShowWindow(hSettingsTab, (iSel == tabCount-1) ? SW_SHOW : SW_HIDE);
	ShowWindow(hEditTab, (iSel != tabCount-1) ? SW_SHOW : SW_HIDE);
	if (iSel != tabCount-1){
		PropSheetAttachments* pAttachments = (PropSheetAttachments*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		string_list* file_names = &pAttachments->file_names;
		string_list::iterator it = file_names->begin();
		std::advance(it, iSel);
		DEBUG_LOG("change tab to",*it);
		std::basic_string<TCHAR> comment;
		std::basic_string<TCHAR> commentWithNewLines;
		if ( pAttachments->description.ReadCommentWithChanges( it->c_str(), comment ) ){
			pAttachments->current_line_multiline_style =
					pAttachments->description.Demultilinefy(comment, commentWithNewLines,
											pAttachments->hidden_cashed_prog_data,
											pAttachments->settings.MultiLineStyle);
			SetWindowText(hEditControl, commentWithNewLines.c_str());
		}else{
			SetWindowText(hEditControl, L"");
		}
	}
}

//======================================================================================
// Function to fill up possible code page selections
//======================================================================================
BOOL CALLBACK CPEnumProc(LPTSTR lpCodePageString)
{
	UINT cp = static_cast<UINT>(_ttoi(lpCodePageString));

	// Build a readable description:
	// - get the code‑page name via GetLocaleInfoEx (Unicode) or GetLocaleInfoA (ANSI)
	// - fall back to just the number if the name lookup fails.
	wchar_t cpName[64] = L"";

#if defined(UNICODE)
	// Try to get the native description (e.g. "Western European (Windows)")
	int ret = GetLocaleInfoEx(L"en-US", LOCALE_IDEFAULTANSICODEPAGE, nullptr, 0);
	// The LOCALE_IDEFAULTANSICODEPAGE flag does **not** give the name,
	// so we use GetCPInfoEx which returns the code‑page description.
	CPINFOEXW info;
	if (GetCPInfoExW(cp, 0, &info))
		swprintf_s(cpName, L"%s", info.CodePageName);
	else
		swprintf_s(cpName, L"%u", cp);
#else
	// ANSI version (same logic, just the W‑suffix functions are absent)
	CPINFOEXA info;
	if (GetCPInfoExA(cp, 0, &info))
		swprintf_s(cpName, L"%S", info.CodePageName);
	else
		swprintf_s(cpName, L"%u", cp);
#endif

	g_codePages.push_back({ cp, cpName });
	return TRUE;	// continue enumeration
}

void PopulateCodePageList()
{
	g_codePages.clear();
	EnumSystemCodePages(CPEnumProc, CP_INSTALLED);
}

void FillCodePageCombo(HWND hDlg, int comboId)
{
	HWND hCombo = GetDlgItem(hDlg, comboId);
	SendMessage(hCombo, CB_RESETCONTENT, 0, 0);

	for (size_t i = 0; i < g_codePages.size(); ++i)
	{
		LRESULT idx = SendMessage(hCombo, CB_ADDSTRING, 0,
								(LPARAM)g_codePages[i].name.c_str());
		SendMessage(hCombo, CB_SETITEMDATA, idx, (LPARAM)g_codePages[i].cp);
	}
}

void SelectCodePageCombo(HWND hDlg, int comboId,int cpId ){
	HWND hCombo = GetDlgItem(hDlg, comboId);
	for (int i = 0; i< g_codePages.size(); i++){
		if (cpId == g_codePages[i].cp)
			SendMessage(hCombo, CB_SETCURSEL, i, 0);
	}
}
