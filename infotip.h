#ifndef INFOTYP_H
#define INFOTIP_H
#include <objbase.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlguid.h>


// Forward declaration of the interface
class IInfoTipComClass : 
	public IPersistFile,
	public IQueryInfo
 {
public:
		// IPersistFile methods
    virtual HRESULT __stdcall Load ( LPCOLESTR wszFilename, DWORD dwMode ) override = 0;
		// IQueryInfo methods
    virtual HRESULT __stdcall GetInfoTip ( DWORD dwFlags, LPWSTR* ppwszTip ) override = 0;



};


// Implement the COM Class
class InfoTipClass : public IInfoTipComClass {
protected:
	LPCOLESTR file;
public:
	InfoTipClass() : refCount(1) {}
	
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) override;

	ULONG __stdcall AddRef() override ;

	ULONG __stdcall Release() override ;

		// IPersistFile methods
	HRESULT __stdcall Load ( LPCOLESTR wszFilename, DWORD dwMode ) override ;
	HRESULT __stdcall GetClassID(LPCLSID)		override	{ return E_NOTIMPL; } 
	HRESULT __stdcall IsDirty()					override	{ return E_NOTIMPL; }
	HRESULT __stdcall Save(LPCOLESTR, BOOL)		override	{ return E_NOTIMPL; }
	HRESULT __stdcall SaveCompleted(LPCOLESTR)	override	{ return E_NOTIMPL; }
	HRESULT __stdcall GetCurFile(LPOLESTR*)		override	{ return E_NOTIMPL; }
		// IQueryInfo methods
	HRESULT __stdcall GetInfoTip ( DWORD dwFlags, LPWSTR* ppwszTip )  override ;
	HRESULT __stdcall GetInfoFlags(DWORD*)		override	{ return E_NOTIMPL; }
private:
	LONG refCount;
};

#endif