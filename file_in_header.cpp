#include "file_in_header.h"
#include <string.h>

Header *FileInHeader::p_the_header_=nullptr;
char *FileInHeader::p_loc_=nullptr;

void FileInHeader::SetHeaderAddress(Header *pth, char *ptc)
{
    p_the_header_=pth;
    p_loc_=ptc;
    p_the_header_->nb_of_files=0;
}

FileInHeader::FileInHeader()
{
    // future ? as we alp_loc_ate file_in_heaer through an array, nothing to be done here
}

void FileInHeader::AddFile(const char* file_name)
{
    file_name_size=strlen(file_name);
    file_name_offset=p_loc_-(const char*)p_the_header_;
    strncpy(p_loc_,file_name,file_name_size);
    p_loc_+=file_name_size;
    p_the_header_->nb_of_files++;
}

char* FileInHeader::FixPadding()
{
    // pad with 0 until next 8 bytes align boundary
    while ((((u64)p_loc_)&7)!=0)
    {
        *p_loc_++=0;
    }
    // fix the header size
    p_the_header_->header_size=(u64)p_loc_-(u64)p_the_header_;
    return p_loc_;
}
