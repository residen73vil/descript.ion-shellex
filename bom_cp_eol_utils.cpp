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

#define SPLESH_0		0b11
#define SPLESH_n		0b10
#define SPLESH_r		0b01

#define EOL_0r0n	SPLESH_0<<6 | SPLESH_r<<4 | SPLESH_0<<2 | SPLESH_n
#define EOL_r0n0	SPLESH_r<<6 | SPLESH_0<<4 | SPLESH_n<<2 | SPLESH_0
#define EOL_rn		SPLESH_r<<2 | SPLESH_n
#define EOL_0n		SPLESH_0<<2 | SPLESH_n
#define EOL_n0		SPLESH_n<<2 | SPLESH_0
#define EOL_n		SPLESH_n
#define EOL_0r		SPLESH_0<<2 | SPLESH_r
#define EOL_r0		SPLESH_r<<2 | SPLESH_0
#define EOL_r		SPLESH_r

size_t is_eol(char* place, char* limit){
//TODO: a utf16 symbol U+0D0A ("Malayalam Letter Lla") can be mistaken for \r\n, fix that 
	unsigned char flag_representation = 0;
	//if approaching limit, check less then 4 next bytes to prevent buffer overshot 
	size_t check_n = ( (limit - place) < 4 ) ? limit - place : 4;
	for (size_t i = 0; i < check_n; i++){
		switch (place[i]) 
		{
			case '\0':
				flag_representation <<= 2;
				flag_representation |= SPLESH_0;
				//if first byte in sequence is \0 but not in an even position it is tailing \0 of LE symbol not leading of BE hence stop checking 
				if (0 == i && ( (limit - place) % 2 )) {
					 return 0;
				}
				continue;
			case '\n':
				flag_representation <<= 2;
				flag_representation |= SPLESH_n;
				continue;
			case '\r':
				flag_representation <<= 2;
				flag_representation |= SPLESH_r;
				continue;
		}
		break; //if different symbol stop search

	}
	switch (flag_representation) 
	{
		case EOL_0r0n:
		case EOL_r0n0:
			return 4;
		case EOL_rn:
		case EOL_0n:
		case EOL_n0:
		case EOL_r0:
		case EOL_0r:
			return 2;
		case EOL_n:
		case EOL_r:
			return 1;
		default:
			return 0;
	}
}