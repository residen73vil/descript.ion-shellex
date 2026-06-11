#ifndef COLUMN_PROVIDER_H
#define COLUMN_PROVIDER_H
#include <objbase.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlguid.h>
#include "shellext_init.h"

// Define a GUID for the COM class
//{ed730b9b-5779-4692-9e36-f7d9a4e86e06}
//{5629ff98-e953-466d-8480-3dd3c554ab09}
const GUID IID_IColumnProviderComClass = { 0xed730b9b, 0x5779, 0x4692, { 0x9e, 0x36, 0xf7, 0xd9, 0xa4, 0xe8, 0x6e, 0x07 } };
//const GUID CLSID_ContextMenuClass = { 0x5629FF98, 0xE953, 0x466D, { 0x84, 0x80, 0x3D, 0xD3, 0xC5, 0x54, 0xAB, 0x09 } };
// GUID for IColumnProvider: E8025004-1C42-11d2-BE2C-00A0C9A83DA1  (not set in comdef.h)
//const GUID IID_IColumnProvider = { 0xE8025004, 0x1C42, 0x11d2, { 0xBE, 0x2C, 0x00, 0xA0, 0xC9, 0xA8, 0x3D, 0xA1} };
// Forward declaration of the interface
class IColumnProviderComClass : 
	public IColumnProvider,
	public ShellExtInitComClass
 {
public:
		// IColumnProvider methods
	virtual HRESULT __stdcall Initialize(LPCSHCOLUMNINIT psci) override=0;
	virtual HRESULT __stdcall GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO* psci) override=0;
	virtual HRESULT __stdcall GetItemData(LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, VARIANT* pvarData) override=0;
};


// Implement the COM Class
class ColumnProviderComClass : public IColumnProviderComClass {
//protected:
//	TCHAR m_szFile[MAX_PATH];
public:
	ColumnProviderComClass() : refCount(0) {
		DEBUG_LOG(L"ColumnProviderComClass", "object created") 
	}
	
	
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) override;

	ULONG __stdcall AddRef() override ;

	ULONG __stdcall Release() override ;

		// IColumnProvider methods
	// Initialisation will set the folder for which the extension show columns
	HRESULT __stdcall Initialize(LPCSHCOLUMNINIT psci) override;
	// Sets which columns the extension provides.
	HRESULT __stdcall GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO* psci) override;
	// Provides column data on demand.
	HRESULT __stdcall GetItemData(LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, /*out*/ VARIANT* pvarData) override;
private:
	LONG refCount;
};

#endif
