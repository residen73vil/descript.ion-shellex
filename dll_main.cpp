#include <windows.h>
#include <objbase.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlguid.h>
#include <iostream>
#include "context_menu.h"
#include "infotip.h"

//Class Factory, part of COM standart
class ClassFactory : public IClassFactory {
public:
	// IUnknown methods
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) override {
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
		WCHAR buffer[39]; // A GUID string representation is 38 chars + null terminator
		StringFromGUID2(riid, buffer, sizeof(buffer) / sizeof(WCHAR));
		MessageBox(NULL, buffer, L"Info", MB_OK);
		if (pUnkOuter != nullptr) {
			return CLASS_E_NOAGGREGATION;
		}
		if ( riid == IID_IQueryInfo || riid == IID_IPersistFile){
			InfoTipClass *pClass = new InfoTipClass();
			return pClass->QueryInterface(riid, ppv);
		}
		if ( riid == IID_IContextMenu || riid == IID_IShellExtInit){
			ContextMenuComClass *pClass = new ContextMenuComClass();
			return pClass->QueryInterface(riid, ppv);
		}
		return E_NOINTERFACE;
	}

	HRESULT __stdcall LockServer(BOOL fLock) override {
		if (fLock) {
			InterlockedIncrement(&lockCount);
		} else {
			InterlockedDecrement(&lockCount);
		}
		return S_OK;
	}

private:
	LONG refCount = 1;
	static LONG lockCount;	// Lock count for the class factory
};

LONG ClassFactory::lockCount = 0; // Initialize static member

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved) {
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