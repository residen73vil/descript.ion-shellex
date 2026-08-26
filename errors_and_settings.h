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

struct CSettings{
	MultiLineStyle MultiLineStyle;
	UINT cp;
	bool bom;

	CSettings(): MultiLineStyle(AUTO), cp(CP_UTF8), bom(true) {};
};

class CErrorsAndSettings

{
	static const wchar_t* ERROR_ACCESS_DENIED_MSG;
	static const wchar_t* ERROR_DISK_FULL_MSG;
	static const wchar_t* ERROR_IO_DEVICE_MSG;

	static CErrorsAndSettings instance;
	CRITICAL_SECTION cs;
	CSettings settings;

	DWORD m_last_io_error;
	static void ShowError(const wchar_t* kind, const wchar_t* msg);
	CErrorsAndSettings():	new_file_default_codepage(CP_UTF8),
							new_file_default_BOM(BOM_UTF8_MODE),
							m_last_io_error(0)
	{
		InitializeCriticalSection(&cs);
		//TODO: Should initialize settings from registry.
	}
	// Delete copy constructor and assignment operator
	CErrorsAndSettings(const CErrorsAndSettings&);
	CErrorsAndSettings& operator=(const CErrorsAndSettings&);
public:
	~CErrorsAndSettings(){
		DeleteCriticalSection(&cs);
	}
	// Static method to get the instance of the Singleton
	static CErrorsAndSettings& getInstance();
	// Settings
	void setSettings(const CSettings &settings);
	CSettings getSettings();

	UINT new_file_default_codepage;
	UINT new_file_default_BOM;
	static bool IOError(DWORD error);
	
	bool ConvertionError(DWORD error);
};

#endif
