#ifndef FILE_H_
#define FILE_H_

#include <string>
#include "common.h"

using namespace std;

// class to represent a file mapped into memory to be placed into a tree to be indexed by 4k overall offset
// as file are all mapped into 64bits process virtual memory and we reference their 4k blocks by 4k offset
// like they were all glued one to another (with just eventual padding in the last 4k block)
class File
{
    public:
    File();
    ~File();

  void *LoadFile(string &file_name, u32 file_id);  // Map file into memory
    void UnloadFile();

    u64 size;                   // in bytes
    u32 nb4k;                   // nb of 4k blocs (including padding)
    u32 bytes_in_last_block;    // <=4096
    u32 id;                     // id of file in list
    void* addr;                 // pt on mapped file

    string name;                // file name

    private:
    void HandleError(const char* hint);

    int fd_;
};


#endif
