#include "description_handler.h"

bool CDescriptionHandler::LoadFile(LPCTSTR filename)
{
	DEBUG_LOG( "CDescriptionHandler::LoadFile", filename);
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
	return true;
}

bool CDescriptionHandler::ReadComment(LPCTSTR filename, /*out*/ LPTSTR comment)
{
	return true;
}

bool CDescriptionHandler::IsCommented(LPCTSTR filename)
{
	return true;
}

bool CDescriptionHandler::AddChangeComment(LPCTSTR filename, LPCTSTR comment)
{
	return true;
}

bool CDescriptionHandler::FilesWithComments(/*out*/ string_list* files) //returns files that have comments
{
	return true;
}