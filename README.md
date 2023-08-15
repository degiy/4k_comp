# 4k_comp
a file compression tool to zip multiple vm datafile focused on 4k same blocks compression

compression is done in several steps :
1/ all files in input are split in 4K blocs (like on disk)
   - each block is ranked from 0 to n (0 being the first bloc of first file, and so on)
   - when a file does not finish on a 4K boundary, the remaining bytes are zeroed (mmap function)
2/ each block is processed through a hash to be caracterized and its reference is placed into a hashmap
3/ identical blocks (like they can be found in multiples vms) are identified through hashmap and byte to byte comparison
4/ all non identical blocs are caracterized through a specific algorithm (using bitmap of occurence of bytes above a given threshold in the bloc)
   - 

