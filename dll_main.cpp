#include <windows.h>
#include <objbase.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlguid.h>
#include <iostream>
#include "context_menu.h"
#include "property_sheet.h"
#include "dbg.h"
UINT g_cActiveComponents = 0; //counts additional noncom components of the dll that are in use
HINSTANCE g_dll_hInstance;
//TODO: Check for memory leakage!!!


//Class Factory, part of COM standart
class ClassFactory : public IClassFactory {
public:
	// IUnknown methods
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) override {
		DEBUG_LOG_RIID( L"FactoryQuery:", riid)
		if (riid == IID_IUnknown || riid == IID_IClassFactory) {
			*ppv = static_cast<IClassFactory*>(this);
			AddRef();
			return S_OK;
		}
		*ppv = nullptr;
		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef() override {
		return InterlockedIncrement(&refCount);
	}

	ULONG __stdcall Release() override {
		ULONG count = InterlockedDecrement(&refCount);
		if (count == 0) {
			delete this;
		}
		return count;
	}

	// IClassFactory methods
	HRESULT __stdcall CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv) override {
		DEBUG_LOG_RIID( L"FactoryInstance:", riid)
		if (pUnkOuter != nullptr) {
			return CLASS_E_NOAGGREGATION;
		}
		if ( riid == CLSID_ContextMenuClass  || riid == IID_IUnknown){
			ContextMenuComClass *pClass = new ContextMenuComClass();
			return pClass->QueryInterface(riid, ppv);
		}
		if ( riid == IID_IContextMenu || riid == IID_IShellExtInit){
			ContextMenuComClass *pClass = new ContextMenuComClass();
			return pClass->QueryInterface(riid, ppv);
		}
		if ( riid == IID_IShellPropSheetExt){
			ShellPropSheetExtComClass *pClass = new ShellPropSheetExtComClass();
			return pClass->QueryInterface(riid, ppv);
		}
		return E_NOINTERFACE;
	}

	HRESULT __stdcall LockServer(BOOL fLock) override {
		DEBUG_LOG( "LockServer", "lockserver called");
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

private:
	LONG refCount = 1;
	static LONG lockCount;	// Lock count for the class factory
};

LONG ClassFactory::lockCount = 0; // Initialize static member

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
		// Set file to output debug log into
		g_dll_hInstance = hInstance;
		DEBUG_INIT("c:\\Logs\\dbg.log");
		DEBUG_LOG( "dllmain", "dll loaded");
		break;
    case DLL_PROCESS_DETACH:
		DEBUG_LOG( "dllmain", "dll unloaded");
		DEBUG_CLOSE
        break;
	case DLL_THREAD_ATTACH:
		DEBUG_LOG( "dllmain", "dll thread loaded");
		break;
    case DLL_THREAD_DETACH:
		DEBUG_LOG( "dllmain", "dll thread unloaded");
		break;
    }
	return TRUE;
}

// DllRegisterServer function
extern "C" __declspec(dllexport) HRESULT DllRegisterServer() {
	// This function should register the COM class
	// You may use Windows Registry functions to add CLSID and interface entries here
	return S_OK;
}

// DllUnregisterServer function
extern "C" __declspec(dllexport) HRESULT DllUnregisterServer() {
	// This function should unregister the COM class
	// Clean up registry entries for CLSID and interface here
	return S_OK;
}

// Create instance function
extern "C" __declspec(dllexport) HRESULT CreateContextMenuComClass(IContextMenuComClass** ppv) {
	*ppv = new ContextMenuComClass();
	return S_OK;
}

// DllGetClassObject implementation
extern "C" __declspec(dllexport)	HRESULT	DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv) {
	if (rclsid == CLSID_ContextMenuClass) {
		ClassFactory* factory = new ClassFactory();
		return factory->QueryInterface(riid, ppv);
	}
	
	return CLASS_E_CLASSNOTAVAILABLE;
}

//Is it safe to unload the dll
STDAPI DllCanUnloadNow() {
    // Check if the server is locked
    if (ClassFactory::get_lockCount() == 0 && g_cActiveComponents == 0) {
		DEBUG_LOG("DllCanUnloadNow", "Can be unloaded");
        return S_OK; // Safe to unload
    }
	DEBUG_LOG("DllCanUnloadNow", "Not allowed to unload");
    return S_FALSE; // Not safe to unload
}