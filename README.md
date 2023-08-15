# 4k_comp

## A file compression tool to zip multiple vm datafile focused on 4k same blocks compression

### compression is done in several steps :

1. all files in input are split in 4K blocs (like on disk) :
   - each block is ranked from 0 to n (0 being the first bloc of first file, and so on)
   - when a file does not finish on a 4K boundary, the remaining bytes are zeroed (mmap function)
   - so every file begin on a 4k boundary in the mapped memory of the process (that's why we need a 64bits cpu)
2. each 4k block is processed through a hash to be caracterized and its reference is placed into a hashmap
3. identical blocks (like they can be found in multiples vms) are identified through the hashmap and byte to byte comparison (when hash match)
4. all non identical blocs are caracterized through a specific algorithm (using bitmap of occurence of bytes above a given threshold in the bloc) :
   - their byte affinity is compared to a set of other frequently used 4k blocks, ranged in a tree
   - the block with the maximum similarity with currend block is reward by an increase of its time to live (TTL) inside the tree
   - the tree is composed of the lastly read blocks and the one who happen to match frequently
   - the tree size being limited, when it is full, a cleaning is done removing the least used block (the one with the lower TTL)
   - along the bitmap of byte occurences that caracterize a given 4k block, is store a hashmap of all 3 consecutive bytes from the block
   - the current block is compressed through a LZ77 like algorithm using its own 4k data and the one of the more similar block

### Specific technical points :

#### MEMORY :

You need specific amount of memory to process to compression :

- **12 bytes** for each bloc to compute hash (murmur based) : For each 4k bloc, we need 12 bytes. So you will be limited by your availlable RAM to read and hash all files. You won't be able to compress more than 340 times your amount of RAM. So with 192GB of RAM, you will be limited to 64TB of files. With 1GB it will be around 340GB.
- **~25KB** per block kept in the cache
  - 20KB of 3 consecutive bytes hashtable (LZ77) by 4kB bloc week in cache in the tree
  - 32B per bit mask per block in cache in the tree
  - the tree management
  - 4KB of the block by itself if you want the mmap to remind cached on RAM
- so you will need to manage the tree settings for both performance and overall memory footprint. Ok it's only several MB for hundred of blocks, but you will want to have the amount on cache
      
#### PERFORMANCE :

- the compression algorithm map all the files into memory (process memory). Depending on the RAM availlable, files will be on physical memory or virtual (not).
- the first pass, will read everything to compute hashs. In case a block matches a hash, the full 4KB comparison will be perform, so it will reload from disk to memory ancient blocks (if not anymore in physical memory). This phase can be painfull on traditionnal HDD, if a lot of blocks are the same, and RAM is not able to cache enough. Cache will work its way, but it will mostly work on computers with a lot of memory.
- the second pass will only focus on non identical blocks to compute bitmap and LZ77 hashmap. So the more identical blocks found in phase 1, the less work will be needed on second pass. Both bitmap calculation and LZ77 3 bytes hashmap will be performed on the block on cache, so it will be only one read.
- 4k_comp is basic, it would probably need more complexity to bring better compression. Right now it's more focused on speed. 
