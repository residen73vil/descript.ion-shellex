#ifndef BOM_CP_EOL_UTILS_H
#define BOM_CP_EOL_UTILS_H


#define BOM_UTF8 "\xEF\xBB\xBF"
#define BOM_UTF16_LE "\xFF\xFE"
#define BOM_UTF16_BE "\xFE\xFF"

#define BOM_NONE_MODE 0
#define BOM_UTF8_MODE 1
#define BOM_UTF16_LE_MODE 2
#define BOM_UTF16_BE_MODE 3
#define BOM_SKIP_MODE -1 //signals that no bom actions should be performed 


#define NEWLINE_WIN_MODE 0
#define NEWLINE_LIN_MODE 1
#define NEWLINE_MAC_MODE 2

#define NEWLINE_WIN "\r\n"
#define NEWLINE_LIN "\n"
#define NEWLINE_MAC "\r"

//are not defined in windows.h file, so defining them by hand
#define CP_UTF16LE 1200
#define CP_UTF16BE 1201

/**
 * @brief Swaps bytes (converting UTF16 BE to LE and vice versa) 
 *
 * Copyes "length_in_bytes" bytes "from" from "to" to swapping them in the process
 *   
 * @param to Buffer where to copy swapped bytes
 * @param from Buffer to copy and swap from
 * @param length_in_bytes How much bytes to swap
 *
 * @return Number of bytes swapped
 *
 * @note If length_in_bytes is not multiple of 2 does not perform a swap
 */
size_t swap_bytes(char* to, char* from, size_t length_in_bytes);

/**
 * @brief Adds End Of Line or Bit Order Mark depending on mode, codepage and bom mode
 *
 * Writes End Of Line to "to" depending on mode in corresponding codepage or writes 
 * a Bit Order Mark depending on bom mode
 *   
 * @param to Buffer to write to
 * @param mode Sets new line mode ( NEWLINE_WIN, NEWLINE_LIN, NEWLINE_MAC )
 * @param codepage Codepage in which to write eol
 * @param is_bom Writes bom instead of eof if set
 *
 * @return Number of bytes written
 */
size_t add_eol_or_bom(char* to, UINT mode, UINT codepage, INT is_bom = BOM_SKIP_MODE);

/**
 * @brief Returns size of End Of Line or Bit Order Mark depending on mode, codepage and bom mode
 *
 * Returns the size of End Of Line in bytes depending on mode and corresponding codepage  
 * or size of a Bit Order Mark depending on bom mode
 *   
 * @param mode Sets new line mode ( NEWLINE_WIN, NEWLINE_LIN, NEWLINE_MAC )
 * @param codepage Codepage in which eol must be later written
 * @param is_bom Return the size of bom instead of eof if set
 *
 * @return Number of bytes to be written
 */
size_t eol_size(UINT mode, UINT codepage, INT is_bom = BOM_SKIP_MODE);

char* look_for_eol(char* to, UINT mode, UINT codepage);

#endif