#ifndef SHELLEXT_INIT_H
#define SHELLEXT_INIT_H
#include <objbase.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlguid.h>
#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "dbg.h" 

// Forward declaration of the interface
class  IShellExtInitComClass :
	public IShellExtInit
 {
public:
	virtual HRESULT __stdcall Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID) override = 0;
};


// Implement the COM Class
class ShellExtInitComClass : public IShellExtInitComClass {
protected:
	TCHAR m_szFile[MAX_PATH];
public:

	HRESULT __stdcall Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID) override ;

};

#endif