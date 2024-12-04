#ifndef PROPERTY_SHEET_H
#define PROPERTY_SHEET_H
#include <objbase.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlguid.h>
#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "dbg.h" 
#include "shellext_init.h"

//#define WINVER 0x0600 // For Windows Vista and later
//#define _WIN32_WINNT 0x0600

extern HINSTANCE g_dll_hInstance;
extern UINT g_cActiveComponents; 

// Define a GUID for the COM class
//{ed730b9b-5779-4692-9e36-f7d9a4e86e06}
//{5629ff98-e953-466d-8480-3dd3c554ab09}
const GUID IID_IShellPropSheetExtComClass = { 0xed730b9b, 0x5779, 0x4692, { 0x9e, 0x36, 0xf7, 0xd9, 0xa4, 0xe8, 0x6e, 0x06 } };
const GUID CLSID_ShellPropSheetExtComClass = { 0x5629FF98, 0xE953, 0x466D, { 0x84, 0x80, 0x3D, 0xD3, 0xC5, 0x54, 0xAB, 0x09 } };

// Forward declaration of the interface
class  IShellPropSheetExtComClass :
	public ShellExtInitComClass,
	public  IShellPropSheetExt
 {
public:
		// IContextMenu methods
	virtual HRESULT __stdcall AddPages(LPFNADDPROPSHEETPAGE, LPARAM) override = 0;;
	virtual HRESULT __stdcall ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM) override
	  { return E_NOTIMPL; }
};


// Implement the COM Class
class ShellPropSheetExtComClass : public IShellPropSheetExtComClass {
protected:
	TCHAR m_szFile[MAX_PATH];
public:
	ShellPropSheetExtComClass() : refCount(0) {
		DEBUG_LOG(L"ShellPropSheetExtComClass", "object created") 
	}

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) override;

	ULONG __stdcall AddRef() override ;

	ULONG __stdcall Release() override ;

		// IContextMenu methods
	//Adds items to the menu
	HRESULT __stdcall AddPages ( LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam ) override ;
private:
	LONG refCount;
};


//Funictions for handling winapi callbacks
INT_PTR CALLBACK PropPageDlgProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
UINT CALLBACK PropPageCallbackProc ( HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp );
//BOOL OnInitDialog ( HWND hwnd, LPARAM lParam );
//BOOL OnApply ( HWND hwnd, PSHNOTIFY* phdr );

#endif