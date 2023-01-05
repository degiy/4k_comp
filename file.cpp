#include "file.h"
#include "common.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

File::File() : size{0}, nb4k{0}, bytes_in_last_block{0}, id{0}, addr{MAP_FAILED}, fd_{-1}
{
}

File::~File()
{
	UnloadFile();
}

void File::UnloadFile()
{
	if (addr!=MAP_FAILED)
	{
		munmap(addr, size);
		size = 0;
		nb4k = 0;
		bytes_in_last_block = 0;
		id = 0;
		addr=MAP_FAILED;
		name = "";
	}
	if (fd_!=-1)
	{
		close(fd_);
		fd_ = -1;
	}
}

void *File::LoadFile(string &file_name,u32 file_id)
{
	struct stat sb;

	fd_ = open(file_name.c_str(), O_RDONLY);
	if (fd_ == -1)
		HandleError("open");

	if (fstat(fd_, &sb) == -1) /* To obtain file size */
		HandleError("fstat");

	size=sb.st_size;
	nb4k=size>>12;
	bytes_in_last_block=size&((1<<12)-1);
	if (bytes_in_last_block)
	{
		nb4k++;
	}

	addr = mmap(NULL, nb4k<<12, PROT_READ, MAP_PRIVATE, fd_, 0);
	if (addr == MAP_FAILED)
		HandleError("mmap");

	id=file_id;
	name=file_name;
	if (debug)
		cout<<"File "<<name<<" loaded and mapped at "<<hex<<(u64)addr
		<<", size : "<<dec<<size<<", nb 4k blocs : "<<nb4k
		<<", remain : "<<bytes_in_last_block<<" bytes, id : "<<id<<endl;

	return addr;
}

void File::HandleError(const char* hint)
{
	cerr << hint << " error on file " << name << endl;
	exit(-1);
}
