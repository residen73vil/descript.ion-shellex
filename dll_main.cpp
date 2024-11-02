#include <windows.h>
#include <objbase.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlguid.h>
#include <iostream>



// Define a GUID for the COM class
//{ed730b9b-5779-4692-9e36-f7d9a4e86e06}
//{5629ff98-e953-466d-8480-3dd3c554ab09}
const GUID IID_ISimpleComClass = { 0xed730b9b, 0x5779, 0x4692, { 0x9e, 0x36, 0xf7, 0xd9, 0xa4, 0xe8, 0x6e, 0x06 } };
const GUID CLSID_SimpleComClass = { 0x5629FF98, 0xE953, 0x466D, { 0x84, 0x80, 0x3D, 0xD3, 0xC5, 0x54, 0xAB, 0x09 } };

// Forward declaration of the interface
class ISimpleComClass : 
		public IShellExtInit,
		public IContextMenu
 {
public:
    virtual HRESULT __stdcall SayHello() = 0;  // A sample method
        // IContextMenu methods
    virtual HRESULT __stdcall QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst,
                                        UINT idCmdLast, UINT uFlags) override = 0;

    virtual HRESULT __stdcall InvokeCommand(LPCMINVOKECOMMANDINFO pici) override = 0;

    virtual HRESULT __stdcall GetCommandString(UINT_PTR idCmd, UINT uType, UINT *pwResv, LPSTR pszName, UINT cchMax) override = 0;
};


// Implement the COM Class
class SimpleComClass : public ISimpleComClass {
public:
    SimpleComClass() : refCount(1) {}
    
 HRESULT __stdcall Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID) override {
        // Initialize your shell extension - store relevant info if needed
	return S_OK;
    }
    HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) override {


        if (riid == IID_IUnknown || riid == IID_ISimpleComClass) {
            *ppv = static_cast<ISimpleComClass*>(this);
            AddRef();
            return NOERROR;
        }
	if ( riid == IID_IContextMenu) {
		*ppv = static_cast<IContextMenu*>(this);
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

    HRESULT __stdcall SayHello() override {
        std::cout << "Hello from COM!" << std::endl;
        return S_OK;
    }

        // IContextMenu methods
    HRESULT __stdcall QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst,
                                        UINT idCmdLast, UINT uFlags) override 
    {
        InsertMenu(hMenu, indexMenu, MF_BYPOSITION, idCmdFirst, L"My Extension");
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 1);
    }

    HRESULT __stdcall InvokeCommand(LPCMINVOKECOMMANDINFO pici) override 
    {
        MessageBox(NULL, L"My Extension Clicked!", L"Info", MB_OK);
        return S_OK;
    }

    HRESULT __stdcall GetCommandString(UINT_PTR idCmd, UINT uType, UINT *pwResv, LPSTR pszName, UINT cchMax) override 
    {
        return S_OK;
    }
private:
    LONG refCount;
};


class MyClassFactory : public IClassFactory {
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
        if (pUnkOuter != nullptr) {
            return CLASS_E_NOAGGREGATION;
        }
        SimpleComClass *pMyClass = new SimpleComClass();

        return pMyClass->QueryInterface(riid, ppv);
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
    static LONG lockCount;  // Lock count for the class factory
};

LONG MyClassFactory::lockCount = 0; // Initialize static member

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
extern "C" __declspec(dllexport) HRESULT CreateSimpleComClass(ISimpleComClass** ppv) {
    *ppv = new SimpleComClass();
    return S_OK;
}

// DllGetClassObject implementation
extern "C" __declspec(dllexport)  HRESULT  DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv) {
    if (rclsid == CLSID_SimpleComClass) {
        MyClassFactory* factory = new MyClassFactory();
        return factory->QueryInterface(riid, ppv);
    }
    
    return CLASS_E_CLASSNOTAVAILABLE;
}