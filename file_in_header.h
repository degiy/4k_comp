#ifndef FILE_IN_HEADER_H_
#define FILE_IN_HEADER_H_

#include "common.h"
#include "header.h"

// entries of files in header
// files names are kept at the end of header
// they are located thanks to offset (from begining of header) and size
// real data begin at header offset (8 bytes alignement)
// So to sum up we have
// - header (with offset allowing to jump to compressed date)
// - first file_in_header
// - ...
// - last file_in_header struct
// - strings of file name (located thanks to offset/size in each file_in_header struct)
// - some pading to fall on 8 byte boundary
// - compressed data

class FileInHeader
{
public:
	FileInHeader();
	void AddFile(const char* file_name);       // to process a file
	static void SetHeaderAddress(Header *pth, char* ptc);   // to initialise address of header struct for file names offsets calculations
	static char* FixPadding();                 // to call when last file to finalise header

	u64 file_size;          // size of file in bytes
	u32 offset_4k_of_file;  // begining of this file in the concatenation of all files to compress
	u32 nb_4k_blocs;        // nb of 4k blocs used by file (last one can be not fullfilled)
	u32 file_name_offset;   // location of file name in char* space after FileInHeader structs
	u32 file_name_size;     // size of the file name char string (without 0 at the end)

private:
	static Header* p_the_header_;
	static char* p_loc_;       // to manage file name buffer
};




#endif
