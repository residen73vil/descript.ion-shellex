#include "description_handler.h"
#include "description_file_rw.h"

bool CDescriptionHandler::LoadFile(LPCTSTR filename)
{
	DEBUG_LOG( "CDescriptionHandler::LoadFile", filename);
	m_filename = std::wstring(filename); //saving file name for latter use
	LoadFileToMap(filename);
	return true;
}

bool CDescriptionHandler::LoadPath(LPCTSTR path) //calls LoadFile("%path%/descript.ion")
{
	TCHAR filename[MAX_PATH];
	_tcscpy(filename, path);
	_tcscat( filename, _T("\\") );
	_tcscat( filename, DESCRIPTION_FILE );
	LoadFile( filename );
	return true;
}

bool CDescriptionHandler::SaveChanges()
{
	if (m_mChanges.size()>0){
		CDescriptionFileRW file_reader;
		file_reader.LoadFile(m_filename.c_str());
		file_reader.FindLines();
		for (std::unordered_map<int, std::wstring>::iterator it = m_mChanges.begin(); it != m_mChanges.end(); ++it){
			file_reader.ChangeLine( it->first, &(it->second) );
		}
		file_reader.ConvertAndSaveChanges(CP_UTF8);
	}
	return true;
}

bool CDescriptionHandler::ReadComment(LPCTSTR filename, /*out*/ std::basic_string<TCHAR>& comment)
{
	std::wstring key( filename ); 
	if (!IsCommented(filename)){
		return false;
	}else{
		std::wstring comment_str = m_comments_map[key];
		comment = comment_str;
		return true;
	}
}

bool CDescriptionHandler::IsCommented(LPCTSTR filename)
{
	std::wstring key( filename ); 
	if (m_comments_map.count(key) > 0)
		return true;
	else
		return false;
}

bool CDescriptionHandler::AddChangeComment(LPCTSTR filename, LPCTSTR comment)
{
	if ( IsCommented(filename) ){
		int line_num = m_linenumber_map[filename];
		std::wstring line = std::wstring(filename) + std::wstring(L" ") + std::wstring(comment);
		m_mChanges[line_num] = line;
	} else {
		std::wstring line = std::wstring(filename) + std::wstring(L" ") + std::wstring(comment);
		m_mChanges[-(++m_nCommentsAdded)] = line;
	}
	return true;
}

bool CDescriptionHandler::FilesWithComments(/*out*/ string_list* files) //returns files that have comments
{
	return true;
}



bool CDescriptionHandler::LoadFileToMap(LPCTSTR &filePath) {
	CDescriptionFileRW file_reader;
	file_reader.LoadFile(filePath);
	size_t number_of_lines = file_reader.FindLines();
	std::wstring line;
	
	for ( int i = 0; i < number_of_lines; i++){
		file_reader.GetConvertedLine(i, &line);

		size_t pos = 0;
		if (line[0] == L'\"'){
			pos = line.find_first_of(L'\"', 1);
			if (pos != std::basic_string<TCHAR>::npos && pos != 2){
				line.erase(0,1); //get rid of first \"
				pos = pos - 1; //correct to accommodate deletions 
				line.erase(pos,1); //get rid of last \"
			} else {
				DEBUG_LOG("LoadFileToMap:error in descript.ion file", 
								"filename is too small/large");
				continue; //skip broken filename
			}
		} else {
			pos = line.find_first_of(L" \t");
		}
		if (pos != std::basic_string<TCHAR>::npos && pos != 0) {
			std::basic_string<TCHAR> key = line.substr(0, pos);
			std::basic_string<TCHAR> value = line.substr(pos + 1);
			pos = value.find_last_of(L'\r');
			if ( pos != std::basic_string<TCHAR>::npos){
				value.erase(pos,1); //get rid of \r at the end
			}
			value.erase(value.find_last_not_of(L" \t")+1); //strip spaces at the beginning
			if (m_comments_map.count(key) > 0) {
				DEBUG_LOG("LoadFileToMap:error in descript.ion filename mentioned several times", key );
				//old value will be overwritten!
				//TODO: does nothing on non-debug builds, so add something here!
			}
			m_linenumber_map[key] = i;
			m_comments_map[key] = value;
		}
	}

	return true;
}