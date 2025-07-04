                          JP Software Inc.
                           P.O. Box 328
                    Chestertown, MD  21620, USA

                           410-810-8818
                         fax 410-810-0026
                       Email sales@jpsoft.com
                     Web http://www.jpsoft.com/

# Technical Note -- Using DESCRIPT.ION

4NT and Take Command use the file DESCRIPT.ION to store file descriptions. This file is created as a hidden file in each subdirectory which has descriptions, and deleted when all descriptions are removed or when all files with descriptions are deleted. If you remove the hidden attribute from the file, it will not be hidden again.

Your programs can access DESCRIPT.ION to create, retrieve, or modify file descriptions, and to store other information. DESCRIPT.ION has one line per file, and is unsorted. Each line is in the following format:

   filename.ext Description[*%%<ID>Other program info]...<CR><LF>%%

There is normally one space between the description and filename but additional spaces may be used in future versions. The characters after the description allow extension of the description format for use by other programs. They are as follows:

	is an ASCII Ctrl-D (04), and marks the end of the description text and the beginning of information for a program other than 4NT or Take Command. This symbol can appear multiple times on each line; each occurrence marks the beginning of information for another program.

<ID> is an identification byte for the program which is using this area of the particular line. If you are writing a program which will store information in DESCRIPT.ION, test it using an ID byte of your own choosing. When you are ready to release the program contact JP Software and we will provide you with an ID by that is not in use by others to the best of our knowledge.

   Other program info is any text the program wishes to store in its
   area of the line.  The text should relate specifically to the file
   named on the line.  It may not contain the Ctrl-D character,
   carriage returns, line feeds, or nulls (ASCII 0s).

4NT and Take Command will copy, delete, or move all the information on a line in DESCRIPT.ION, including information owned by other programs, when performing the same action on the corresponding file. They will also change the name if a file is renamed. To support DESCRIPT.ION properly, your program must do the same if it copies, deletes, moves, or renames files. Take care not to remove information which does not belong to your program, or delete lines which contain information for other programs. Your program should be able to handle a line terminated by a CR or LF alone, a CR/LF pair, an EOF (ASCII 26), or the physical end of the file. The lines it creates should be terminated with CR / LF. The line length limit is 4096 bytes; exceeding this limit will cause unpredictable results.

# Reallife use cases:
Total Commander uses ID C2 that just signals that sequence "\n" in the comment should be treated as a next line 
    Filename This is the first line\\nSecond line\\nLast line\x04\xc2
in Double Commander \xa0 (non-breaking space, \xc2\xa0 in utf8, \x00\xa0 in utf16) signals line breaks in multi-line comments.
