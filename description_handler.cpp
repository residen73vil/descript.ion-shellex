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
		file_reader.ConvertAndSaveChanges(file_reader.m_nCodepage, CErrorsAndSettings::getInstance().getSettings().bom);
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


////////////////////////////////////////////////////////////////////////////////
// Helper constants (UTF‑16 literals)
// NOTE: 0x00A0 = non‑breaking space
//		 0x0004 = the marker of program data in the comment
//		 0x00C2 = program id, marks that totalcmd multi line comment format is used
static const wchar_t NBSP   = L'\x00A0';	// doublecmd uses none breaking spare
static const wchar_t PROG_DATA = L'\x0004';	// mark of program data
static const wchar_t TOTALCMD_ID = L'\x00C2';	// program id (totalcmd new line)

////////////////////////////////////////////////////////////////////////////////
// Detect which mode a string is using (AUTO)
static MultiLineStyle DetectMode(const std::wstring& s)
{
	// Look for the TOTALCMD marker first - it is the most specific.
	//TODO: This method of search for the mark is not robust should use another one
	if (s.find(L"\\n") != std::wstring::npos &&
		s.find(PROG_DATA) != std::wstring::npos &&
		s.find(TOTALCMD_ID) != std::wstring::npos)
		return TOTALCMD;

	// DOUBLECMD uses a non‑breaking space instead of a normal space.
	if (s.find(NBSP) != std::wstring::npos )
		return DOUBLECMD;

	// No special sequences.
	return NONE;
}

//TODO: Properly save Program Data and add it back in unchanged.
//		Maybe even show it in the program some how.

//==========================================================================
// Demultilinefy - turn the encoded form into a real newline string
//==========================================================================
MultiLineStyle CDescriptionHandler::Demultilinefy(const std::wstring& lineIn,
										/*out*/ std::wstring& lineOut,
										/*out*/ std::wstring& commentProgData,
												MultiLineStyle mode)
{
	// Auto‑detect if requested
	if (mode == AUTO)
		mode = DetectMode(lineIn);

	lineOut.clear();
	commentProgData.clear();
	size_t i = 0;
	switch (mode) {
	case DOUBLECMD: {
		// Replace every NBSP with a real newline.
		lineOut.reserve(lineIn.size());
		for (i = 0; i < lineIn.size(); ++i) {
			if (i + 1 < lineIn.size() &&
				lineIn[i] == NBSP) {
				lineOut.push_back(L'\r');
				lineOut.push_back(L'\n');
			} else {
				if (lineIn[i] == PROG_DATA){
					break; // stop copping if prog data is reached
				}
				lineOut.push_back(lineIn[i]);
			}
		}
		break;
	}

	case TOTALCMD: {
		// Convert the literal string "\\n" to a real newline,
		// but **only** when the TOTALCMD terminator (\x04\xc2) is present.
		if (lineIn.find(PROG_DATA) != std::wstring::npos &&
			lineIn.find(TOTALCMD_ID) != std::wstring::npos) {

			lineOut.reserve(lineIn.size());
			for (i = 0; i < lineIn.size(); ++i) {
				// Detect the two‑character sequence \"\\n\"
				if (i + 1 < lineIn.size() &&
					lineIn[i] == L'\\' && lineIn[i + 1] == L'n') {
					lineOut.push_back(L'\r');
					lineOut.push_back(L'\n');
					++i; // skip the 'n'
				} else {
					if (lineIn[i] == PROG_DATA){
						break; // stop copping if prog data is reached
					}
					lineOut.push_back(lineIn[i]);
				}
			}
			break;
		} 
		// If no totalcmd multiline mark present, proceed father to plain copy
	}

	case NONE:
	case AUTO:	// AUTO will have been resolved above, fall‑through is safe
	default:
		for (i = 0; i < lineIn.size(); ++i) {
			if (lineIn[i] == PROG_DATA){
				break; // stop copping if prog data is reached
			}
			lineOut.push_back(lineIn[i]);
		}
		break;
	}

	// The rest of the line is prog data (if any), copy it as well.
	if ( i < lineIn.size() && lineIn[i] == PROG_DATA){
		for (; i < lineIn.size(); i++){

			if (i + 1 < lineIn.size() &&
					lineIn[i] == PROG_DATA && lineIn[i + 1] == TOTALCMD_ID){
				++i; // don't copy totalcmd multi line mark
			}else{
				commentProgData.push_back(lineIn[i]);}
		}
	}
	return mode;
}

//==========================================================================
// Multilinefy - turn a real newline string into the encoded form
//==========================================================================
MultiLineStyle CDescriptionHandler::Multilinefy(const std::wstring& lineIn,
										/*out*/	std::wstring& lineOut,
										const	std::wstring& commentProgData,
												MultiLineStyle mode)
{
	// Auto‑detect if requested (based on the *input* string, which already
	// contains real newlines)
	if (mode == AUTO)
		mode = TOTALCMD;

	lineOut.clear();

	switch (mode) {
	case DOUBLECMD: {
		// Replace every real newline with NBSP.
		lineOut.reserve(lineIn.size());
		for (size_t i = 0; i < lineIn.size(); ++i) {
			if (i + 1 < lineIn.size() &&
					lineIn[i] == L'\r' && lineIn[i+1] == L'\n'){
				lineOut.push_back(NBSP);
				++i; //skip \n
			}else{
				lineOut.push_back(lineIn[i]);
			}
		}
		break;
	}

	case TOTALCMD: {
		// Replace each real newline with the literal sequence \"\\n\".
		// Append the required terminator (\x04\xc2) at the end.
		bool bMultiLine = false;
		lineOut.reserve(lineIn.size());
		for (size_t i = 0; i < lineIn.size(); ++i) {
			if (i + 1 < lineIn.size() &&
					lineIn[i] == L'\r' && lineIn[i+1] == L'\n'){
				lineOut.append(L"\\n");
				bMultiLine = true;
				++i; //skip \n
			}else{
				lineOut.push_back(lineIn[i]);
			}
		}
		if (bMultiLine){ // multi line mark is only written if there are new lines
			lineOut.push_back(PROG_DATA);
			lineOut.push_back(TOTALCMD_ID);
		}
		break;
	}

	case NONE:{
		// Replace each real newline with two spaces.
		lineOut.reserve(lineIn.size());
		for (size_t i = 0; i < lineIn.size(); ++i) {
			if (i + 1 < lineIn.size() &&
					lineIn[i] == L'\r' && lineIn[i+1] == L'\n'){
				lineOut.append(L"  ");
				++i; //skip \n
			}else{
				lineOut.push_back(lineIn[i]);
			}
		}
		break;
	}
	case AUTO:	// AUTO resolved already
	default:
		lineOut = lineIn;	// plain copy
		break;
	}
	// Add prog data back in
	lineOut.append(commentProgData);
	return mode;
}
