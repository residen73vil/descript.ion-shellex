#include "column_provider.h"
#include "dbg.h"

HRESULT __stdcall ColumnProviderComClass::QueryInterface(REFIID riid, void **ppv) {
	DEBUG_LOG_RIID(L"ColumnProviderComClass", riid) ;

	if (riid == IID_IUnknown || riid == IID_IColumnProviderComClass) {
		*ppv = static_cast<IColumnProviderComClass*>(this);
		AddRef();
		return NOERROR;
	}
	if ( riid == IID_IColumnProvider) {
		*ppv = static_cast<IColumnProvider*>(this);
			AddRef();
			return NOERROR;
		}
	if ( riid == IID_IShellExtInit) {
		DEBUG_LOG(L"ContextMenuComClass", "return IShellExtInit") ;
		*ppv = static_cast<ShellExtInitComClass*>(this);
			AddRef();
			return NOERROR;
		}
		*ppv = nullptr;
		return E_NOINTERFACE;
}

ULONG __stdcall ColumnProviderComClass::AddRef() {
	DEBUG_LOG(L"ColumnProviderComClass", "refcount incremented") 
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall ColumnProviderComClass::Release() {
	DEBUG_LOG(L"ColumnProviderComClass", "refcount decremented") 
	ULONG count = InterlockedDecrement(&refCount);
	if (count == 0) {
		DEBUG_LOG(L"ColumnProviderComClass", "object deleted ")
		delete this;
	}
	return count;
}

	// IColumnProvider methods
// Initialisation will set the folder for which the extension show columns
HRESULT __stdcall ColumnProviderComClass::Initialize(LPCSHCOLUMNINIT psci){
	return S_OK;
};
// Sets which columns the extension provides.
HRESULT __stdcall ColumnProviderComClass::GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO* psci){
	if ( dwIndex >= 1 )
		return S_FALSE;

	psci->scid.fmtid	= FMTID_SummaryInformation;	// predefined FMTID
	psci->scid.pid		= PIDSI_COMMENTS;			// Predefined - comment
	psci->vt			= VT_LPSTR;					// We'll return the data as a string
	psci->fmt			= LVCFMT_LEFT;				// Text will be left-aligned
	psci->csFlags		= SHCOLSTATE_TYPE_STR;		// Data should be sorted as strings
	psci->cChars		= 32;						// Default col width in chars
	return S_OK;
};
// Provides column data on demand.
HRESULT __stdcall ColumnProviderComClass::GetItemData(LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, /*out*/ VARIANT* pvarData){
	VariantInit(pvarData);
	BSTR b = SysAllocString(L"Sample text");
	if (!b) return E_OUTOFMEMORY;	// allocation failed

	pvarData->vt = VT_BSTR;
	pvarData->bstrVal = b;
	return S_OK;
};
