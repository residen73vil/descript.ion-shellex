#ifndef ERRORS_AND_SETTINGS_H
#define ERRORS_AND_SETTINGS_H

#include <windows.h>


class CErrorsAndSettings

{
	static const wchar_t* ERROR_ACCESS_DENIED_MSG;
	static const wchar_t* ERROR_DISK_FULL_MSG;
	static const wchar_t* ERROR_IO_DEVICE_MSG;
	
	static CErrorsAndSettings instance;

	DWORD m_last_io_error = 0;
	static void ShowError(const wchar_t* kind, const wchar_t* msg);
	CErrorsAndSettings(): new_file_default_codepage(CP_UTF8) {
        // Initialization code (if needed)
    }
public:
	    // Delete copy constructor and assignment operator
    CErrorsAndSettings(const CErrorsAndSettings&) = delete;
    CErrorsAndSettings& operator=(const CErrorsAndSettings&) = delete;

    // Static method to get the instance of the Singleton
    static CErrorsAndSettings& getInstance();


	UINT new_file_default_codepage = CP_UTF8;
	static bool IOError(DWORD error);
	
	bool ConvertionError(DWORD error);

	
};

#endif