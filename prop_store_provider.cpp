#include "prop_store_provider.h"
#include "description_handler.h"
#include "shellext_init.h"
#include <propvarutil.h>
#include "dbg.h"

std::wstring GetComment(const wchar_t* filePath);
EXTERN_C const PROPERTYKEY PKEY_CommentColumn =
{
    {
        0x7f5a2b41,
        0x9b61,
        0x4c1c,
        { 0x91, 0x84, 0x42, 0x3c, 0x11, 0x7a, 0x08, 0x55 }
    },
    2
};
//DEFINE_PROPERTYKEY(
//	PKEY_CommentColumn,
//	0x7f5a2b41, 0x9b61, 0x4c1c,
//	0x91, 0x84, 0x42, 0x3c, 0x11, 0x7a, 0x08, 0x55,
//	2);
HRESULT __stdcall PropStoreProviderComClass::QueryInterface(REFIID riid, void **ppv) {
	DEBUG_LOG_RIID(L"PropStoreProviderComClass", riid) ;

	if (riid == IID_IUnknown || riid == IID_IPropStoreProviderComClass) {
		*ppv = static_cast<IPropStoreProviderComClass*>(this);
		AddRef();
		return NOERROR;
	}
	if ( riid == IID_IPropertyStore) {
		*ppv = static_cast<IPropertyStore*>(this);
			AddRef();
			return NOERROR;
		}
	if ( riid == IID_IInitializeWithFile) {
		DEBUG_LOG(L"PropStoreProviderComClass", "return IInitializeWithFile") ;
		*ppv = static_cast<IInitializeWithFile*>(this);
			AddRef();
			return NOERROR;
		}
		*ppv = NULL;
		return E_NOINTERFACE;
}

ULONG __stdcall PropStoreProviderComClass::AddRef() {
	DEBUG_LOG(L"PropStoreProviderComClass", "refcount incremented") 
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall PropStoreProviderComClass::Release() {
	DEBUG_LOG(L"PropStoreProviderComClass", "refcount decremented") 
	ULONG count = InterlockedDecrement(&refCount);
	if (count == 0) {
		DEBUG_LOG(L"PropStoreProviderComClass", "object deleted ")
		delete this;
	}
	return count;
}

	// IColumnProvider methods
// Initialisation will set the folder for which the extension show columns
HRESULT __stdcall PropStoreProviderComClass::Initialize(LPCWSTR pszFilePath, DWORD grfMode){
	if (!pszFilePath)
		return E_INVALIDARG;

	// This provider is read-only and does not write to the file.
	if (grfMode & STGM_WRITE)
		return STG_E_ACCESSDENIED;

	m_szFilePath = pszFilePath;
	return S_OK;
};


HRESULT __stdcall PropStoreProviderComClass::GetCount(DWORD* cProps){
	if (!cProps)
		return E_POINTER;

	*cProps = 1;
	return S_OK;
};
HRESULT __stdcall PropStoreProviderComClass::GetAt(DWORD iProp, PROPERTYKEY* pkey){
	if (!pkey)
		return E_POINTER;

	if (iProp != 0)
		return E_INVALIDARG;

	*pkey = PKEY_CommentColumn;
	return S_OK;
};
HRESULT __stdcall PropStoreProviderComClass::SetValue(REFPROPERTYKEY key, REFPROPVARIANT propvar){
	return STG_E_ACCESSDENIED;
};
HRESULT __stdcall PropStoreProviderComClass::Commit(){
	return S_OK;
};


HRESULT __stdcall PropStoreProviderComClass::GetValue(REFPROPERTYKEY key, PROPVARIANT* pv){
	if (!pv)
		return E_POINTER;

	PropVariantInit(pv);

	//if (key == PKEY_CommentColumn)
	{
		std::wstring value = GetComment(m_szFilePath);

		return InitPropVariantFromString(
			value.c_str(),
			pv);
	}
};



std::wstring GetComment(const wchar_t* filePath){
	TCHAR	 szFileName[MAX_PATH];
	TCHAR	 szFilePath[MAX_PATH];
	std::basic_string<TCHAR> comment;
	std::basic_string<TCHAR> commentWithNewLines;
	std::basic_string<TCHAR> commentPropData;
	CDescriptionHandler description;

	GetFileNameAndPath(filePath, szFileName, szFilePath, false);
	description.LoadPath(szFilePath);
	if ( description.IsCommented(szFileName) ){
		description.ReadComment(szFileName, comment);
		description.Demultilinefy(comment, commentWithNewLines, commentPropData,
									CErrorsAndSettings::getInstance().getSettings().MultiLineStyle);
		// Multiline is only partially supported by column provider, so we better get rid of it all together.
		description.Multilinefy(commentWithNewLines, comment, L"", NONE);

		return comment;
	} else {
		return L"";
	}
};
