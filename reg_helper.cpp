#include "reg_helper.h"

namespace Reg {

	bool CreateKey(
		HKEY root,
		const std::wstring& subKey
	) {
		HKEY key = NULL;

		LONG result = RegCreateKeyExW(
			root,
			subKey.c_str(),
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_QUERY_VALUE | KEY_SET_VALUE,
			NULL,
			&key,
			NULL
		);

		if (result == ERROR_SUCCESS) {
			RegCloseKey(key);
			return true;
		}

		return false;
	}

	bool WriteString(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		const std::wstring& value
	) {
		HKEY key = NULL;

		LONG result = RegCreateKeyExW(
			root,
			subKey.c_str(),
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_SET_VALUE,
			NULL,
			&key,
			NULL
		);

		if (result != ERROR_SUCCESS) {
			return false;
		}

		const DWORD byteCount = static_cast<DWORD>(
			(value.size() + 1) * sizeof(wchar_t)
		);

		result = RegSetValueExW(
			key,
			valueName.c_str(),
			0,
			REG_SZ,
			reinterpret_cast<const BYTE*>(value.c_str()),
			byteCount
		);

		RegCloseKey(key);
		return result == ERROR_SUCCESS;
	}

	bool ReadString(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		std::wstring& value
	) {
		HKEY key = NULL;

		LONG result = RegOpenKeyExW(
			root,
			subKey.c_str(),
			0,
			KEY_QUERY_VALUE,
			&key
		);

		if (result != ERROR_SUCCESS) {
			return false;
		}

		DWORD type = 0;
		DWORD byteCount = 0;

		// First query the required buffer size.
		result = RegQueryValueExW(
			key,
			valueName.c_str(),
			NULL,
			&type,
			NULL,
			&byteCount
		);

		if (result != ERROR_SUCCESS || type != REG_SZ) {
			RegCloseKey(key);
			return false;
		}

		wchar_t* buffer = new wchar_t[byteCount / sizeof(wchar_t)];

		result = RegQueryValueExW(
			key,
			valueName.c_str(),
			NULL,
			NULL,
			reinterpret_cast<BYTE*>(buffer),
			&byteCount
		);

		RegCloseKey(key);

		if (result != ERROR_SUCCESS) {
			return false;
		}

		value = buffer;
		return true;
	}

	bool WriteDword(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		DWORD value
	) {
		HKEY key = NULL;

		LONG result = RegCreateKeyExW(
			root,
			subKey.c_str(),
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_SET_VALUE,
			NULL,
			&key,
			NULL
		);

		if (result != ERROR_SUCCESS) {
			return false;
		}

		result = RegSetValueExW(
			key,
			valueName.c_str(),
			0,
			REG_DWORD,
			reinterpret_cast<const BYTE*>(&value),
			sizeof(value)
		);

		RegCloseKey(key);
		return result == ERROR_SUCCESS;
	}

	bool ReadDword(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		DWORD& value
	) {
		HKEY key = NULL;

		LONG result = RegOpenKeyExW(
			root,
			subKey.c_str(),
			0,
			KEY_QUERY_VALUE,
			&key
		);

		if (result != ERROR_SUCCESS) {
			return false;
		}

		DWORD type = 0;
		DWORD byteCount = sizeof(DWORD);

		result = RegQueryValueExW(
			key,
			valueName.c_str(),
			NULL,
			&type,
			reinterpret_cast<BYTE*>(&value),
			&byteCount
		);

		RegCloseKey(key);

		return result == ERROR_SUCCESS &&
				type == REG_DWORD &&
				byteCount == sizeof(DWORD);
	}

	bool KeyExists(
		HKEY root,
		const std::wstring& subKey
	) {
		HKEY key = NULL;

		LONG result = RegOpenKeyExW(
			root,
			subKey.c_str(),
			0,
			KEY_READ,
			&key
		);

		if (result == ERROR_SUCCESS) {
			RegCloseKey(key);
			return true;
		}

		return false;
	}

	bool ValueExists(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName
	) {
		HKEY key = NULL;

		LONG result = RegOpenKeyExW(
			root,
			subKey.c_str(),
			0,
			KEY_QUERY_VALUE,
			&key
		);

		if (result != ERROR_SUCCESS) {
			return false;
		}

		result = RegQueryValueExW(
			key,
			valueName.c_str(),
			NULL,
			NULL,
			NULL,
			NULL
		);

		RegCloseKey(key);
		return result == ERROR_SUCCESS;
	}

	bool DeleteValue(
		HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName
	) {
		HKEY key = NULL;

		LONG result = RegOpenKeyExW(
			root,
			subKey.c_str(),
			0,
			KEY_SET_VALUE,
			&key
		);

		if (result != ERROR_SUCCESS) {
			return false;
		}

		result = RegDeleteValueW(
			key,
			valueName.c_str()
		);

		RegCloseKey(key);

		return result == ERROR_SUCCESS;
	}

	bool DeleteKey(  // works recursively
		HKEY root,
		const std::wstring& subKey
	) {
		HKEY key = NULL;

		LONG result = RegOpenKeyExW(
			root,
			subKey.c_str(),
			0,
			KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
			&key
		);

		if (result == ERROR_FILE_NOT_FOUND) {
			// Already deleted or never existed.
			return true;
		}

		if (result != ERROR_SUCCESS) {
			return false;
		}

		wchar_t childName[256];
		DWORD childNameLength;

		// Always enumerate index 0 because deleting a child shifts
		// the remaining children down.
		for (;;) {
			childNameLength = ARRAYSIZE(childName);

			result = RegEnumKeyExW(
				key,
				0,
				childName,
				&childNameLength,
				NULL,
				NULL,
				NULL,
				NULL
			);

			if (result == ERROR_NO_MORE_ITEMS) {
				break;
			}

			if (result != ERROR_SUCCESS) {
				RegCloseKey(key);
				return false;
			}

			std::wstring childPath = subKey;
			childPath += L'\\';
			childPath.append(childName, childNameLength);

			if (!DeleteKey(root, childPath)) {
				RegCloseKey(key);
				return false;
			}
		}

		RegCloseKey(key);

		// At this point, the key should be empty, so RegDeleteKeyW
		// can delete it.
		result = RegDeleteKeyW(
			root,
			subKey.c_str()
		);

		return result == ERROR_SUCCESS ||
				result == ERROR_FILE_NOT_FOUND;
	}
}
