#include <windows.h>
#include "bom_cp_eol_utils.h"


size_t swap_bytes(char* to, char* from, size_t length_in_bytes){
	if ( 0 != length_in_bytes %2) //cant swipe non multiple of 2
		return 0;
	for (int i = 0; i < length_in_bytes; i+=2){
		to[i] = from[i+1];
		to[i+1] = from[i];
	}
	return length_in_bytes;
}

size_t add_eol_or_bom(char* to, UINT mode, UINT codepage, INT is_bom){
	if ( -1 == is_bom ){
		switch (mode) 
		{
			case NEWLINE_WIN_MODE:
				if ( codepage == CP_UTF16LE){
					*to = NEWLINE_WIN[0];
					*(to+1) = '\0';
					*(to+2) = NEWLINE_WIN[1];
					*(to+3) = '\0';
					return 4;
				} else if ( codepage == CP_UTF16BE ){
					*to = '\0';
					*(to+1) = NEWLINE_WIN[0];
					*(to+2) = '\0';
					*(to+3) = NEWLINE_WIN[1];
					return 4;
				} else {
					*to = NEWLINE_WIN[0];
					*(to+1) = NEWLINE_WIN[1];
					return 2;
				}
				break;
			case NEWLINE_LIN_MODE:
				if ( codepage == CP_UTF16LE){
					*to = NEWLINE_LIN[0];
					*(to+1) = '\0';
					return 2;
				} else if ( codepage == CP_UTF16BE ){
					*to = '\0';
					*(to+1) = NEWLINE_LIN[0];
					return 2;
				} else {
					*to = NEWLINE_LIN[0];
					return 1;
				}
				break;
			case NEWLINE_MAC_MODE:
				if ( codepage == CP_UTF16LE){
					*to = NEWLINE_MAC[0];
					*(to+1) = '\0';
					return 2;
				} else if ( codepage == CP_UTF16BE ){
					*to = '\0';
					*(to+1) = NEWLINE_MAC[0];
					return 2;
				} else {
					*to = NEWLINE_MAC[0];
					return 1;
				}
		    	break;
			default:
				return 0;
				break;
		}
	} else {
		switch (is_bom) 
		{
			case BOM_UTF8_MODE:
				*to = BOM_UTF8[0];
				*(to+1) = BOM_UTF8[1];
				*(to+2) = BOM_UTF8[2];
				return 3;
				break;
			case BOM_UTF16_LE_MODE:
				*to = BOM_UTF16_LE[0];
				*(to+1) = BOM_UTF16_LE[1];
				return 2;
				break;
			case BOM_UTF16_BE_MODE:
				*to = BOM_UTF16_BE[0];
				*(to+1) = BOM_UTF16_BE[1];
				return 2;
		    	break;
			case BOM_NONE_MODE:
				return 0;
				break;
			default:
				return 0;
				break;
		} 
	}
}
size_t eol_size(UINT mode, UINT codepage, INT is_bom){
		size_t result = 0;
	if ( mode == NEWLINE_WIN_MODE ) result = 2;
	else result = 1;
	if ( codepage == CP_UTF16LE || codepage == CP_UTF16BE ) result += ( (mode == NEWLINE_WIN_MODE) ? 2 : 1 ) ;
	switch (is_bom) 
	{
		case BOM_UTF8_MODE:
			result = 3;
			break;
		case BOM_UTF16_LE_MODE:
		case BOM_UTF16_BE_MODE:
	    	result = 2;
	    	break;
		case BOM_NONE_MODE:
			result = 0;
			break;
		case -1:
		default:
			break;
	}  
	
	return result;
}

char* look_for_eol(char* to, UINT mode, UINT codepage){
	return to;
}