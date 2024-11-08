#ifndef CONTEXT_MENU_H
#define CONTEXT_MENU_H
#include <objbase.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlguid.h>

// Define a GUID for the COM class
//{ed730b9b-5779-4692-9e36-f7d9a4e86e06}
//{5629ff98-e953-466d-8480-3dd3c554ab09}
const GUID IID_IContextMenuComClass = { 0xed730b9b, 0x5779, 0x4692, { 0x9e, 0x36, 0xf7, 0xd9, 0xa4, 0xe8, 0x6e, 0x06 } };
const GUID CLSID_ContextMenuClass = { 0x5629FF98, 0xE953, 0x466D, { 0x84, 0x80, 0x3D, 0xD3, 0xC5, 0x54, 0xAB, 0x09 } };

// Forward declaration of the interface
class IContextMenuComClass : 
	public IShellExtInit,
	public IContextMenu
 {
public:
        // IContextMenu methods
    virtual HRESULT __stdcall QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst,
                                        UINT idCmdLast, UINT uFlags) override = 0;

    virtual HRESULT __stdcall InvokeCommand(LPCMINVOKECOMMANDINFO pici) override = 0;

    virtual HRESULT __stdcall GetCommandString(UINT_PTR idCmd, UINT uType, UINT *pwResv, LPSTR pszName, UINT cchMax) override = 0;
};


// Implement the COM Class
class ContextMenuComClass : public IContextMenuComClass {
protected:
	TCHAR m_szFile[MAX_PATH];
public:
	ContextMenuComClass() : refCount(1) {}
	
	HRESULT __stdcall Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID) override ;
	
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) override;

	ULONG __stdcall AddRef() override ;

	ULONG __stdcall Release() override ;

		// IContextMenu methods
	//Adds items to the menu
	HRESULT __stdcall QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) override ;
	//Invoked when the item is clicked
	HRESULT __stdcall InvokeCommand(LPCMINVOKECOMMANDINFO pici) override ;
	//Shows a comment for the menu item in the status bar
	HRESULT __stdcall GetCommandString(UINT_PTR idCmd, UINT uType, UINT *pwResv, LPSTR pszName, UINT cchMax) override ;
private:
	LONG refCount;
};

#endif