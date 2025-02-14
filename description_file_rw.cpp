#include "description_file_rw.h"

bool CDescriptionFileRW::LoadFile(LPCTSTR filename) {

	m_nFileSize = m_file_io.LoadFileIntoBuffer(filename, &m_lpcFileBuffer);
	m_sFilename = filename;
	if (m_nFileSize > 0){
		LookForBomInBuffer();
		return true;
	} else {
		return false;
	}
}

size_t CDescriptionFileRW::FindLines() {
	
	m_vLines.clear();
	char* start = m_lpcFileBuffer;
	char* end = m_lpcFileBuffer;

	for ( ; end < m_lpcFileBuffer + m_nFileSize; end++) {
		if (*end == '\n') {
			//in case of big endian utf16 we have to compensate for \0 before \n by subtracting 1 from line end pointer
			m_vLines.emplace_back(start, end -((m_nCodepage == CP_UTF16BE) ? 1 : 0) );
			start = end + 1 ;
			if (*start == '\0' && m_nCodepage == CP_UTF16LE){
				start=start+1;} //compensate for \0 after \n in little endian utf16
		} 
	}
	//TODO: last line seems to sometimes contain garbage, fix it
	if (start < end) { //add last line
		m_vLines.emplace_back(start, end);
	}

	return m_vLines.size();
}

UINT CDescriptionFileRW::LookForBomInBuffer(){
	//WARNIGN changing m_lpcFileBuffer pointer causes error upon its deletion, use real pointer instead!!!
	if (m_lpcFileBuffer[0] == BOM_UTF8[0] && 
			m_lpcFileBuffer[1] == BOM_UTF8[1] &&
			m_lpcFileBuffer[2] == BOM_UTF8[2]
		)
	{
		m_lpcFileBuffer = m_lpcFileBuffer + 3; //skip BOM if found
		m_nBitOrder = BOM_UTF8_MODE;
		m_nCodepage = CP_UTF8;
		return  m_nBitOrder;
	}
	
	if (m_lpcFileBuffer[0] == BOM_UTF16_LE[0] && 
			m_lpcFileBuffer[1] == BOM_UTF16_LE[1]
		)
	{
		m_lpcFileBuffer = m_lpcFileBuffer + 2; //skip BOM if found
		m_nCodepage = CP_UTF16LE;
		m_nBitOrder = BOM_UTF16_LE_MODE;
		return  m_nBitOrder;
	}

	if (m_lpcFileBuffer[0] == BOM_UTF16_BE[0] && 
			m_lpcFileBuffer[1] == BOM_UTF16_BE[1]
		)
	{
		m_lpcFileBuffer = m_lpcFileBuffer + 2; //skip BOM if found
		m_nCodepage = CP_UTF16BE;
		m_nBitOrder = BOM_UTF16_BE_MODE;
		return  m_nBitOrder;
	}
	m_nBitOrder = BOM_NONE_MODE;
	return m_nBitOrder;
}

int CDescriptionFileRW::GetConvertedLine(int number, /*out*/ std::wstring* line){
	std::pair<char*,char*> linebonds =  m_vLines[number];
	if (m_nCodepage == CP_UTF16LE){
		size_t wstr_len = (linebonds.second - linebonds.first)/2;
		*line = std::wstring(reinterpret_cast<wchar_t*>(linebonds.first), wstr_len);
		return wstr_len;
	//just swat bytes if BIG ENDIAN
	}else if (m_nCodepage == CP_UTF16BE){
		char* multiByteStr = linebonds.first;
		size_t length_in_bytes = linebonds.second - linebonds.first;
		//TODO: check mod(length_in_bytes, 2) to make sure it is multiple of 2
		char* swap_buffer = new char[length_in_bytes+2];
		for (int i = 0; i < length_in_bytes; i+=2){
			swap_buffer[i] = multiByteStr[i+1];
			swap_buffer[i+1] = multiByteStr[i];
		}
		swap_buffer[length_in_bytes] = '\0'; //null terminate the string
		swap_buffer[length_in_bytes+1] = '\0';
		
		*line = std::wstring(reinterpret_cast<wchar_t*>(swap_buffer), length_in_bytes/2+1);
		delete[] swap_buffer;
		return length_in_bytes/2;
	//converting non utf16 files   
	} else {
		size_t multi_byte_str_size = linebonds.second - linebonds.first;
		char* multiByteStr = linebonds.first;
		int wideCharSize = MultiByteToWideChar(m_nCodepage, 0, multiByteStr, multi_byte_str_size, NULL, 0);
		if (wideCharSize <= 0) {
			DEBUG_LOG( "Error getting wide char size", GetLastError() );
			return 0;
		}
	
		// Allocate buffer for wide character string
		wchar_t* wideStr = new wchar_t[wideCharSize + 1];
	
		// Perform the conversion
		int result = MultiByteToWideChar(m_nCodepage, 0, multiByteStr, multi_byte_str_size,
											 wideStr, wideCharSize);
		if (result <= 0) {
			DEBUG_LOG( "Error converting to wide char", GetLastError() );
			delete[] wideStr; // Clean up allocated memory
			return 0;
		}
		wideStr[wideCharSize] = '\0'; //zero terminate the string 
		*line = std::wstring(wideStr);
		delete[] wideStr;
		return wideCharSize;
	}
}

bool CDescriptionFileRW::ChangeLine(int number, std::wstring* line){
	m_mChanges[number] = *line;
	return true;
}

bool CDescriptionFileRW::ConvertChangesToCodePage(UINT codepage, std::map<int, tuple_2_sizes_and_ptr>& changes_cvonverted){
	for (std::map<int, std::wstring>::iterator it = m_mChanges.begin(); it != m_mChanges.end(); ++it) {
		DEBUG_LOG("change line", it->second + L" (in line) " + std::to_wstring(it->first));
		int line_n = it->first;
		std::wstring* source_str = &(it->second);
		int size = 0;
		char* str_to_write;
		if (codepage == CP_UTF16LE){
			size = source_str->size()*2;
			str_to_write = new char[size + 2];
			memcpy(str_to_write, reinterpret_cast<const char*>(source_str->c_str()),size);
			str_to_write[size] = '\0'; //null terminate the string
			str_to_write[size+1] = '\0';
		} else if (codepage == CP_UTF16BE){ //swap bytes if big endian
			size = source_str->size()*2;
			str_to_write = new char[size + 2];
			const char* source =  reinterpret_cast<const char*>(source_str->c_str());
			for (int i = 0; i < size; i+=2){
				str_to_write[i] = source[i+1];
				str_to_write[i+1] = source[i];
			}
			str_to_write[size] = '\0'; //null terminate the string
			str_to_write[size+1] = '\0';
		} else {
			size = WideCharToMultiByte(codepage, 0, source_str->c_str(), source_str->size(), NULL, 0, NULL, NULL);
			if (size <= 0) {
				DEBUG_LOG( "Error getting str size", GetLastError() );
				return false;
			}
			str_to_write = new char[size + 1];
			int result = WideCharToMultiByte(codepage, 0, source_str->c_str(), source_str->size(), 
														str_to_write, size, NULL, NULL);
			if (result <= 0) {
				DEBUG_LOG( "Error converting from wide char", GetLastError() );
				delete[] str_to_write; // Clean up allocated memory
				return false;
			}
			str_to_write[size]= '\0';
		}
			int old_size = 0;
			if (line_n >= 0){ //if line number is less then 0 then it's a new line added at the end of the file
				old_size = m_vLines[line_n].second - m_vLines[line_n].first;
			} else { //we need a place to add \n when dealing with new lines so add 1 (of 2 if utf16) bytes
				//TODO: new page size seem to be a bit short causing overflow in the heap 
				//		adding 4 and 2 seems to fix the problem but adds an unwanted symbol at the end
				//		further investigation and fix are needed    
				size += (codepage == CP_UTF16LE || codepage == CP_UTF16BE) ? 2 : 1; 
			}
			changes_cvonverted[line_n] = tuple_2_sizes_and_ptr(old_size, size, str_to_write);
	}
	return true;
}


bool CDescriptionFileRW::ConvertAndSaveChanges(UINT codepage){
	std::map<int, tuple_2_sizes_and_ptr> changes_cvonverted; //holds converted lines and their sizes 
															//plus old size of corresponding line in file
	//populating changes_cvonverted
	ConvertChangesToCodePage(codepage, changes_cvonverted);

	// calculating new size of the file
	int lines_sizes_in_file = 0;
	int new_lines_sizes = 0;
	for (std::map<int, tuple_2_sizes_and_ptr>::iterator it = changes_cvonverted.begin();
				 it != changes_cvonverted.end(); ++it) {
		lines_sizes_in_file += std::get<0>(it->second);
		new_lines_sizes += std::get<1>(it->second);	
	}
	int new_file_size = m_nFileSize - lines_sizes_in_file + new_lines_sizes;

	char* buffer_to_write = new char[new_file_size];
	char*  copy_from = m_file_io.m_lpcFileBuffer;
	char*  copy_to = buffer_to_write;
	//copping data before changed line and its contents into buffer_to_write
	for (std::map<int, tuple_2_sizes_and_ptr>::iterator it = changes_cvonverted.begin();
				 it != changes_cvonverted.end(); ++it) {
		if (it->first >= 0){ 
			int copy_count = m_vLines[it->first].first - copy_from;
			memcpy(copy_to, copy_from, copy_count);
			copy_to += copy_count;
			memcpy(copy_to, std::get<2>(it->second), std::get<1>(it->second));
			copy_to += std::get<1>(it->second);
			copy_from = m_vLines[it->first].second;
		}
	}
	//copping data after the last changed line
	int copy_count = m_file_io.m_lpcFileBuffer + m_nFileSize - copy_from;
	memcpy(copy_to, copy_from, copy_count);

	//adding new lines
	copy_to += copy_count;
	for (std::map<int, tuple_2_sizes_and_ptr>::iterator it = changes_cvonverted.begin();
				 it != changes_cvonverted.end(); ++it) {
//TODO: do not add \n at the beginning of a new file 
		if (it->first < 0){
			*copy_to = '\n';
			copy_to += 1;
			if (codepage == CP_UTF16LE){
				*copy_to = '\0';
				copy_to += 1;
			}
			if (codepage == CP_UTF16BE){
				*copy_to = '\n';
				*(copy_to-1) = '\0';
				copy_to += 1;
			}
			char* line_to_add = std::get<2>(it->second);
			size_t line_to_add_length = std::get<1>(it->second);
			//compensate for \0 at the end of the string and set it to be replaced by \n
			line_to_add_length -= ( codepage == CP_UTF16LE or codepage == CP_UTF16BE ) ? 2 : 1;
			memcpy(copy_to, line_to_add, line_to_add_length);
			copy_to += line_to_add_length;
		}
//TODO: another way of handling ends of the line is needed 
	}
	
	//freeing memory (deleting converted lines)
	for (std::map<int, tuple_2_sizes_and_ptr>::iterator it = changes_cvonverted.begin();
				 it != changes_cvonverted.end(); ++it) {
		delete[] std::get<2>(it->second);
	}
	
	//writing data
	DWORD bytesWritten = 0;
	bytesWritten = m_file_io.WriteBufferIntoFile(m_sFilename.c_str(), buffer_to_write, new_file_size);
	
	delete[] buffer_to_write;
	return true;
}

CDescriptionFileRW::~CDescriptionFileRW(){

	m_file_io.CleanupBuffersMemory();
}