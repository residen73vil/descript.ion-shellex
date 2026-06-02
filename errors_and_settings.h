#ifndef ERRORS_AND_SETTINGS_H
#define ERRORS_AND_SETTINGS_H

#include <windows.h>
#include "bom_cp_eol_utils.h"

enum MultiLineStyle{
	TOTALCMD,
	DOUBLECMD,
	AUTO,
	NONE
};

class CErrorsAndSettings

{
	static const wchar_t* ERROR_ACCESS_DENIED_MSG;
	static const wchar_t* ERROR_DISK_FULL_MSG;
	static const wchar_t* ERROR_IO_DEVICE_MSG;

	static CErrorsAndSettings instance;
	CRITICAL_SECTION cs;

	DWORD m_last_io_error = 0;
	MultiLineStyle m_eMultiLineStyle = AUTO;
	static void ShowError(const wchar_t* kind, const wchar_t* msg);
	CErrorsAndSettings():	new_file_default_codepage(CP_UTF8),
							new_file_default_BOM(BOM_UTF8_MODE),
							m_eMultiLineStyle(AUTO)
	{
		InitializeCriticalSection(&cs);
	}
	~CErrorsAndSettings(){
		DeleteCriticalSection(&cs);
	}
public:
	// Delete copy constructor and assignment operator
	CErrorsAndSettings(const CErrorsAndSettings&) = delete;
	CErrorsAndSettings& operator=(const CErrorsAndSettings&) = delete;

	// Static method to get the instance of the Singleton
	static CErrorsAndSettings& getInstance();
	void setMultiLineStyle(MultiLineStyle mode);
	MultiLineStyle getMultiLineStyle();

	UINT new_file_default_codepage = CP_UTF8;
	UINT new_file_default_BOM = BOM_UTF8_MODE;
	static bool IOError(DWORD error);
	
	bool ConvertionError(DWORD error);


};

#endif
