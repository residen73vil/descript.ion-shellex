#include "infotip.h"

HRESULT __stdcall InfoTipClass::QueryInterface(REFIID riid, void **ppv) {


	if (riid == IID_IUnknown ) {
		*ppv = static_cast<InfoTipClass*>(this);
		AddRef();
		return NOERROR;
	}
	if ( riid == IID_IQueryInfo) {
		*ppv = static_cast<IQueryInfo*>(this);
		 	AddRef();
			return NOERROR;
		}
	if ( riid == IID_IPersistFile) {
		*ppv = static_cast<IPersistFile*>(this);
			AddRef();
			return NOERROR;
		}
		*ppv = nullptr;
		return E_NOINTERFACE;
}

ULONG __stdcall InfoTipClass::AddRef() {
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall InfoTipClass::Release() {
	ULONG count = InterlockedDecrement(&refCount);
	if (count == 0) {
		delete this;
	}
	return count;
}

HRESULT __stdcall InfoTipClass::Load ( LPCOLESTR wszFilename, DWORD dwMode ) {

	return S_OK;
}

HRESULT __stdcall InfoTipClass::GetInfoTip ( DWORD dwFlags, LPWSTR* ppwszTip ) {
	//todo: implement if menu item persists
	wchar_t* arr = L"test";
	ppwszTip = &arr;
	return S_OK;
}
