#ifndef PROP_STORE_PROVIDER_H
#define PROP_STORE_PROVIDER_H
#include <propsys.h>
#include <propkey.h>
#include <string>
#include "compiler_compatability.h"
#include "dbg.h"

// Define a GUID for the COM class
//{ed730b9b-5779-4692-9e36-f7d9a4e86e06}
//{5629ff98-e953-466d-8480-3dd3c554ab09}
const GUID IID_IPropStoreProviderComClass = { 0xed730b9b, 0x5779, 0x4692, { 0x9e, 0x36, 0xf7, 0xd9, 0xa4, 0xe8, 0x6e, 0x07 } };
//const GUID CLSID_ContextMenuClass = { 0x5629FF98, 0xE953, 0x466D, { 0x84, 0x80, 0x3D, 0xD3, 0xC5, 0x54, 0xAB, 0x09 } };
// GUID for IColumnProvider: E8025004-1C42-11d2-BE2C-00A0C9A83DA1  (not set in comdef.h)
//const GUID IID_IColumnProvider = { 0xE8025004, 0x1C42, 0x11d2, { 0xBE, 0x2C, 0x00, 0xA0, 0xC9, 0xA8, 0x3D, 0xA1} };
// Forward declaration of the interface
class IPropStoreProviderComClass : 
	public IPropertyStore,
	public IInitializeWithFile
 {
public:
		// IColumnProvider methods
	virtual HRESULT __stdcall Initialize(LPCWSTR pszFilePath, DWORD grfMode) =0;
	virtual HRESULT __stdcall GetCount(DWORD* cProps) =0;
	virtual HRESULT __stdcall GetAt(DWORD iProp, PROPERTYKEY* pkey) =0;
	virtual HRESULT __stdcall GetValue(REFPROPERTYKEY key, PROPVARIANT* pv) =0;
	virtual HRESULT __stdcall SetValue(REFPROPERTYKEY key, REFPROPVARIANT propvar) =0;
	virtual HRESULT __stdcall Commit() =0;
};


// Implement the COM Class
class PropStoreProviderComClass : public IPropStoreProviderComClass {
public:
	PropStoreProviderComClass() : refCount(0) {
		DEBUG_LOG(L"CPropStoreProviderComClass", "object created");
	}
	
	
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv);

	ULONG __stdcall AddRef();

	ULONG __stdcall Release();

	HRESULT __stdcall Initialize(LPCWSTR pszFilePath, DWORD grfMode);
	HRESULT __stdcall GetCount(DWORD* cProps);
	HRESULT __stdcall GetAt(DWORD iProp, PROPERTYKEY* pkey);
	HRESULT __stdcall GetValue(REFPROPERTYKEY key, PROPVARIANT* pv);
	HRESULT __stdcall SetValue(REFPROPERTYKEY key, REFPROPVARIANT propvar);
	HRESULT __stdcall Commit();
private:
	LONG refCount;
	const wchar_t* m_szFilePath;
};

#endif
