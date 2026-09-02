#include "errors_and_settings.h"
#include "reg_helper.h"
#include <string>

void CErrorsAndSettings::ShowError(const wchar_t* kind, const wchar_t* msg){
	MessageBoxW(NULL, msg, kind, MB_OK | MB_ICONINFORMATION);
}

    // Static method to get the instance of the Singleton
CErrorsAndSettings& CErrorsAndSettings::getInstance() {
	//TODO: some funky thread unsafe staff can happen here in c++ < 11, fix that for visual c++ 6.0 while porting
	return instance;
}

bool CErrorsAndSettings::IOError(DWORD error){
	switch (error) {
		case ERROR_ACCESS_DENIED:
			ShowError(L"Error opening file", ERROR_ACCESS_DENIED_MSG);
			break;
		case ERROR_FILE_NOT_FOUND:
			ShowError(L"Error opening file",L"File not found.\n");
			break;
		case ERROR_PATH_NOT_FOUND:
			ShowError(L"Error opening file",L"Path not found.\n");
			break;
		case ERROR_INVALID_PARAMETER:
			ShowError(L"Error opening file",L"Invalid parameter.\n");
			break;
		case ERROR_ALREADY_EXISTS:
			ShowError(L"Error opening file",L"File already exists.\n");
			break;
		case ERROR_DISK_FULL:
			ShowError(L"Error opening file", ERROR_DISK_FULL_MSG);
			break;
		case ERROR_SHARING_VIOLATION:
			ShowError(L"Error opening file",L"Sharing violation.\n");
			break;
		case ERROR_INVALID_HANDLE:
			ShowError(L"Error opening file",L"Invalid handle.\n");
			break;
		case ERROR_NOT_ENOUGH_MEMORY:
			ShowError(L"Error opening file",L"Not enough memory.\n");
			break;
		case ERROR_IO_DEVICE:
			ShowError(L"Error opening file", ERROR_IO_DEVICE_MSG);
			break;
		default:
			ShowError(L"Error opening file",L"An unknown error occurred.\n");
			break;
	}
	return false;
}
	
bool CErrorsAndSettings::ConvertionError(DWORD error){
	return false;
}


const wchar_t* CErrorsAndSettings::ERROR_ACCESS_DENIED_MSG = L"Access denied.\n";
const wchar_t* CErrorsAndSettings::ERROR_DISK_FULL_MSG = L"Disk is full.\n";
const wchar_t* CErrorsAndSettings::ERROR_IO_DEVICE_MSG = L"I/O device error.\n";
CErrorsAndSettings CErrorsAndSettings::instance;


const wchar_t* reg_sittings_key = L"Software\\ResE\\descript.ion-shellex";


void CErrorsAndSettings::setSettings(const CSettings &_settings){
	EnterCriticalSection(&cs);
	settings = _settings;
	LeaveCriticalSection(&cs);
}
CSettings CErrorsAndSettings::getSettings(){
	CSettings eRet;
	EnterCriticalSection(&cs);
	eRet = settings;
	LeaveCriticalSection(&cs);
	return eRet;
}

bool CErrorsAndSettings::saveSettingsToReg(){
	bool result = true;
	EnterCriticalSection(&cs);
	if (!Reg::WriteDword(HKEY_CURRENT_USER, reg_sittings_key, L"cp", settings.cp))
		result = false;
	if (!Reg::WriteDword(HKEY_CURRENT_USER, reg_sittings_key, L"bom", settings.bom))
		result = false;
	if (!Reg::WriteDword(HKEY_CURRENT_USER, reg_sittings_key, L"MultiLineStyle",
														 settings.MultiLineStyle))
		result = false;
	LeaveCriticalSection(&cs);
	return result;
}

bool CErrorsAndSettings::loadSettingsFromReg(){
	bool result = true;
	DWORD temp = 0;
	EnterCriticalSection(&cs);
	if (Reg::ReadDword(HKEY_CURRENT_USER, reg_sittings_key, L"cp", temp))
		settings.cp = temp;
	else
		result = false;
	if (Reg::ReadDword(HKEY_CURRENT_USER, reg_sittings_key, L"bom", temp))
		settings.bom = temp;
	else
		result = false;
	if (Reg::ReadDword(HKEY_CURRENT_USER, reg_sittings_key, L"MultiLineStyle", temp))
		settings.MultiLineStyle = (MultiLineStyle) temp;
	else
		result = false;
	LeaveCriticalSection(&cs);
	return result;
}
