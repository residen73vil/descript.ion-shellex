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
		file_reader.ConvertAndSaveChanges(file_reader.m_nCodepage);
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

bool CDescriptionHandler::ReadCommentWithChanges(LPCTSTR filename, /*out*/ std::basic_string<TCHAR>& comment)
{
	std::wstring key( filename );
	//try to read the comment normally first
	bool bRet = ReadComment(filename, comment);
	//check if changes are present
	//firstly in m_mChanges
	if (m_linenumber_map.count(key)){
		int line_num = m_linenumber_map[key];
		if (m_mChanges.count(line_num))
			{
				std::basic_string<TCHAR> extractedFname;
				std::basic_string<TCHAR> extractedComment;
				SeparateCommentAndFileName(m_mChanges[line_num], extractedFname, extractedComment);
				comment = extractedComment;
				bRet = true;
			}
	//then in m_mNewLines
	}else if (!m_mNewLines.empty())
	{
		for (std::unordered_map<std::wstring, int>::iterator it = m_mNewLines.begin(); it != m_mNewLines.end(); ++it){
			if (it->first == key){
				std::basic_string<TCHAR> extractedFname;
				std::basic_string<TCHAR> extractedComment;
				SeparateCommentAndFileName(m_mChanges[-it->second], extractedFname, extractedComment);
				comment = extractedComment;
				bRet = true;
			}
		}
	}
	return bRet;
}
bool CDescriptionHandler::SeparateCommentAndFileName(std::basic_string<TCHAR> line, /*out*/ std::basic_string<TCHAR>& filename,
												 /*out*/ std::basic_string<TCHAR>& comment)
{
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
			return false; //skip broken filename
		}
	} else {
		pos = line.find_first_of(L" \t");
	}
	if (pos != std::basic_string<TCHAR>::npos && pos != 0) {
		filename = line.substr(0, pos);
		comment = line.substr(pos + 1);
		pos = comment.find_last_of(L'\r');
		if ( pos != std::basic_string<TCHAR>::npos){
			comment.erase(pos,1); //get rid of \r at the end
		}
		//TODO: end line search must be redone!
		comment.erase(comment.find_last_not_of(L" \t")+1); //strip spaces at the beginning
	}
	return true;
}

bool CDescriptionHandler::IsCommented(LPCTSTR filename)
{
	std::wstring key( filename ); 
	if (m_comments_map.count(key) > 0)
		return true;
	else
		return false;
}

std::wstring AddQuotesIfContainsSpaces(const std::wstring& input) {
    // Check if the string contains a space
    if (input.find(L' ') != std::wstring::npos) {
        // If it does, return the string with quotes
        return L"\"" + input + L"\"";
    }
    // If it doesn't, return the original string
    return input;
}

bool CDescriptionHandler::AddChangeComment(LPCTSTR filename, LPCTSTR comment)
{
	if ( IsCommented(filename) ){
		int line_num = m_linenumber_map[filename];
		std::wstring prepared_filename = AddQuotesIfContainsSpaces(std::wstring(filename));
		std::wstring line = prepared_filename + std::wstring(L" ") + std::wstring(comment);
		m_mChanges[line_num] = line;
	} else {
		std::wstring key = std::wstring(filename);
		std::unordered_map<std::wstring, int>::iterator it = m_mNewLines.end();
		if (!m_mNewLines.empty()){
			it = m_mNewLines.find(key);
		}
		//Add \" to file name if needed
		std::wstring prepared_filename = AddQuotesIfContainsSpaces(std::wstring(filename));
		
		//TODO: possibility for a different separator according to settings in changed comments above too
		std::wstring line = prepared_filename + std::wstring(L" ") + std::wstring(comment);
		if ( it == m_mNewLines.end() ){
			m_mNewLines[key] = ++m_nCommentsAdded;
			m_mChanges[-m_nCommentsAdded] = line;
			DEBUG_LOG(L"new comment", line);
		}else{
			m_mChanges[-(it->second)] = line;
		}
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

		std::basic_string<TCHAR> extractedFname;
		std::basic_string<TCHAR> extractedComment;
		SeparateCommentAndFileName(line, extractedFname, extractedComment);
		if (m_comments_map.count(extractedFname) > 0) {
			DEBUG_LOG("LoadFileToMap:error in descript.ion filename mentioned several times", extractedFname );
			//old value will be overwritten!
			//TODO: does nothing on non-debug builds, so add something here!
		}
		m_linenumber_map[extractedFname] = i;
		m_comments_map[extractedFname] = extractedComment;
	}

	return true;
}
