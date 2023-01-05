#include "header.h"

Header::Header()
{
	type=('4'<<24)+('k'<<16)+('z'<<8)+'p';
	version=1;
	header_size=sizeof(class Header);
	nb_of_files=0;
}
