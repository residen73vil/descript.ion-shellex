#ifndef REG_HELPER_H
#define REG_HELPER_H

#include <windows.h>
#include <string>

namespace Reg {

	// Creates the key if it does not exist.
	bool CreateKey(
		HKEY root,
		const std::wstring& subKey
	);

	bool WriteString(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		const std::wstring& value
	);

	bool ReadString(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		std::wstring& value
	);

	bool WriteDword(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		DWORD value
	);

	bool ReadDword(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		DWORD& value
	);

	bool KeyExists(
		HKEY root,
		const std::wstring& subKey
	);

	bool ValueExists(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName
	);

}
#endif
