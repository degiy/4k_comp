#ifndef HEADER_H_
#define HEADER_H_

#include "common.h"

// header of compressed file : singleton
// reference the list of files taken into account for compression
// most of the code to manipulate header is located in the file_in_header class

class Header
{
public:
    Header();

    u32 type;
    u32 version;
    u32 header_size;
    u32 nb_of_files;
};

#endif
