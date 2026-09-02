#include <windows.h>
#include <objbase.h>
#include <shlobj.h>
#include <shlguid.h>
#include <iostream>
#include "context_menu.h"
#include "property_sheet.h"
#include "column_provider.h"
#include "dbg.h"
#include "compiler_compatability.h"
#include "reg_helper.h"
HINSTANCE g_dll_hInstance;
UINT g_cActiveComponents = 0; //counts additional noncom components of the dll that are in use
wchar_t appName[] = L"descript.ion-shellex";

//TODO: Check for memory leakage!!!
//TODO: Com works wrong, fix it


//Class Factory, part of COM standart
class ClassFactory : public IClassFactory {
public:
	// IUnknown methods
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv){
		DEBUG_LOG_RIID( L"FactoryQuery:", riid)
		if (riid == IID_IUnknown || riid == IID_IClassFactory) {
			*ppv = static_cast<IClassFactory*>(this);
			AddRef();
			return S_OK;
		}
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef(){
		return InterlockedIncrement(&refCount);
	}

	ULONG __stdcall Release(){
		ULONG count = InterlockedDecrement(&refCount);
		if (count == 0) {
			delete this;
		}
		return count;
	}

	// IClassFactory methods
	HRESULT __stdcall CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv){
		DEBUG_LOG_RIID( L"FactoryInstance:", riid)
		if (pUnkOuter != NULL) {
			return CLASS_E_NOAGGREGATION;
		}
		if ( riid == CLSID_ContextMenuClass  || riid == IID_IUnknown){
			ContextMenuComClass *pClass = static_cast<ContextMenuComClass*>(new ShellPropSheetExtComClass());
			return pClass->QueryInterface(riid, ppv);
		}
		if ( riid == IID_IContextMenu || riid == IID_IShellExtInit){
			ContextMenuComClass *pClass = static_cast<ContextMenuComClass*>(new ShellPropSheetExtComClass());
			return pClass->QueryInterface(riid, ppv);
		}
		if ( riid == IID_IShellPropSheetExt){
			ShellPropSheetExtComClass *pClass = new ShellPropSheetExtComClass();
			return pClass->QueryInterface(riid, ppv);
		}
		if ( riid == IID_IColumnProvider){
			ColumnProviderComClass *pClass = new ColumnProviderComClass();
			return pClass->QueryInterface(riid, ppv);
		}
		return E_NOINTERFACE;
	}

	HRESULT __stdcall LockServer(BOOL fLock){
		DEBUG_LOG( L"LockServer", L"lockserver called");
		if (fLock) {
			InterlockedIncrement(&lockCount);
		} else {
			InterlockedDecrement(&lockCount);
		}
		return S_OK;
	}
	
	static LONG get_lockCount(){
		return lockCount;
	}

	ClassFactory(): refCount(1){};
private:
	LONG refCount;
	static LONG lockCount;	// Lock count for the class factory
};

LONG ClassFactory::lockCount = 0; // Initialize static member

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		// Set file to output debug log into
		g_dll_hInstance = hInstance;
		OutputDebugString(L"test");
		//DEBUG_INIT("c:\\Logs\\dbg.log");
		DEBUG_LOG( L"dllmain", L"dll loaded");
		break;
	case DLL_PROCESS_DETACH:
		DEBUG_LOG( L"dllmain", L"dll unloaded");
		DEBUG_CLOSE
		break;
	case DLL_THREAD_ATTACH:
		DEBUG_LOG( L"dllmain", L"dll thread loaded");
		break;
	case DLL_THREAD_DETACH:
		DEBUG_LOG( L"dllmain", L"dll thread unloaded");
		break;
	}
	return TRUE;
}

// DllRegisterServer function
extern "C" __declspec(dllexport) HRESULT DllRegisterServer() {
	// This function should register the COM class
	// You may use Windows Registry functions to add CLSID and interface entries here
	wchar_t guid[40];
	wchar_t dll_path[MAX_PATH];
	std::wstring reg_path = L"";

	if ( StringFromGUID2(CLSID_ContextMenuClass, guid, 40) == 0 )
		return E_FAIL;
	DWORD length = GetModuleFileNameW(
				g_dll_hInstance,
				dll_path,
				ARRAYSIZE(dll_path)
			);
	if (length == 0)
		return E_FAIL;

	// Register com object
	reg_path = std::wstring(L"CLSID\\") + guid;
	if (!Reg::WriteString(HKEY_CLASSES_ROOT, reg_path, L"", appName))
		return E_FAIL;
	reg_path = std::wstring(L"CLSID\\") + guid + L"\\InProcServer32";
	if (!Reg::WriteString(HKEY_CLASSES_ROOT, reg_path, L"", dll_path))
		return E_FAIL;
	reg_path = std::wstring(L"CLSID\\") + guid + L"\\InProcServer32";
	if (!Reg::WriteString(HKEY_CLASSES_ROOT, reg_path, L"ThreadingModel", L"Apartment"))
		return E_FAIL;

	// Register property sheet handler
	reg_path = std::wstring(L"*\\shellex\\PropertySheetHandlers\\descript.ion-shellex");
	if (!Reg::WriteString(HKEY_CLASSES_ROOT, reg_path, L"", guid))
		return E_FAIL;
	reg_path = std::wstring(L"Directory\\shellex\\PropertySheetHandlers\\descript.ion-shellex");
	if (!Reg::WriteString(HKEY_CLASSES_ROOT, reg_path, L"", guid))
		return E_FAIL;
	// Register column handler ( Pre vista only)
	reg_path = std::wstring(L"*\\shellex\\ColumnHandlers\\descript.ion-shellex");
	if (!Reg::WriteString(HKEY_CLASSES_ROOT, reg_path, L"", guid))
		return E_FAIL;
	reg_path = std::wstring(L"Directory\\shellex\\ColumnHandlers\\descript.ion-shellex");
	if (!Reg::WriteString(HKEY_CLASSES_ROOT, reg_path, L"", guid))
		return E_FAIL;
	return S_OK;
}

// DllUnregisterServer function
extern "C" __declspec(dllexport) HRESULT DllUnregisterServer() {
	// This function should unregister the COM class
	// Clean up registry entries for CLSID and interface here
	wchar_t guid[40];
	std::wstring reg_path = L"";
	if ( StringFromGUID2(CLSID_ContextMenuClass, guid, 40) == 0 )
		return E_FAIL;
	// Deleting com class
	reg_path = std::wstring(L"CLSID\\") + guid;
	if (!Reg::DeleteKey(HKEY_CLASSES_ROOT, reg_path))
		return E_FAIL;
	// Deleting property handler
	reg_path = std::wstring(L"*\\shellex\\PropertySheetHandlers\\descript.ion-shellex");
	if (!Reg::DeleteKey(HKEY_CLASSES_ROOT, reg_path))
		return E_FAIL;
	reg_path = std::wstring(L"Directory\\shellex\\PropertySheetHandlers\\descript.ion-shellex");
	if (!Reg::DeleteKey(HKEY_CLASSES_ROOT, reg_path))
		return E_FAIL;
	// Deleting column handler
	reg_path = std::wstring(L"*\\shellex\\ColumnHandlers\\descript.ion-shellex");
	if (!Reg::DeleteKey(HKEY_CLASSES_ROOT, reg_path))
		return E_FAIL;
	reg_path = std::wstring(L"Directory\\shellex\\ColumnHandlers\\descript.ion-shellex");
	if (!Reg::DeleteKey(HKEY_CLASSES_ROOT, reg_path))
		return E_FAIL;
	// Deleting application settings
	reg_path = std::wstring(L"SOFTWARE\\ResE\\descript.ion-shellex");
	if (!Reg::DeleteKey(HKEY_CURRENT_USER, reg_path))
		return E_FAIL;
	return S_OK;
}

// Create instance function
extern "C" __declspec(dllexport) HRESULT CreateContextMenuComClass(IContextMenuComClass** ppv) {
	*ppv = new ContextMenuComClass();
	return S_OK;
}

//#pragma comment(linker, "/export:DllGetClassObject=_DllGetClassObject@12")
// DllGetClassObject implementation
extern "C" HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID FAR*ppv) {
	DEBUG_LOG_RIID( L"DllGetCalassObject", riid)
	if (rclsid == CLSID_ContextMenuClass) {
		ClassFactory* factory = new ClassFactory();
		return factory->QueryInterface(riid, ppv);
	}
	
	return CLASS_E_CLASSNOTAVAILABLE;
}

//Is it safe to unload the dll
//TODO: Doesn't seem to actually work, fix that.
extern "C" __declspec(dllexport) STDAPI DllCanUnloadNow() {
	// Check if the server is locked
	if (ClassFactory::get_lockCount() == 0 && g_cActiveComponents == 0) {
		DEBUG_LOG(L"DllCanUnloadNow", L"Can be unloaded");
		return S_OK; // Safe to unload
	}
	DEBUG_LOG(L"DllCanUnloadNow", L"Not allowed to unload");
	return S_FALSE; // Not safe to unload
}

//look for other classes dll implements
//TODO: Add "this" and make it the only function converting classes or something
//WARNING: Broken for now!
HRESULT LookForAnotherImplementedClass(REFIID riid, LPVOID FAR*ppv) {
	DEBUG_LOG_RIID( L"LookForAnotherImplementedClass", riid)
	static ShellPropSheetExtComClass *shellextInit = NULL;
	/*if ( riid == IID_IContextMenu ){
		ContextMenuComClass *pClass = new ContextMenuComClass();
		return pClass->QueryInterface(riid, ppv);
	}
	if (riid == IID_IShellExtInit){
		ShellExtInitComClass *pClass = NULL;
		if (shellextInit != NULL)
			pClass = (ShellExtInitComClass*) shellextInit;
		else{
			pClass = (ShellExtInitComClass*) new ShellPropSheetExtComClass();
			pClass->AddRef();
			shellextInit = (ShellPropSheetExtComClass*) pClass;
		}
		//__asm { int 3 };
		return pClass->QueryInterface(riid, ppv);
	}
	if ( riid == IID_IShellPropSheetExt ){
		ShellPropSheetExtComClass *pClass = NULL;
		if (shellextInit != NULL)
			pClass = shellextInit;
		else{
			pClass = new ShellPropSheetExtComClass();
			pClass->AddRef();
			shellextInit = pClass;
		}
		//__asm { int 3 };
		return pClass->QueryInterface(riid, ppv);
	}
	*ppv = NULL;*/
	return E_NOINTERFACE;
}
