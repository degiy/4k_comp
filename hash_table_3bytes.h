#ifndef HASH_TABLE_3BYTES_H_
#define HASH_TABLE_3BYTES_H_

#include "common.h"
#include <vector>

using namespace std;

// building bloc for 3 consecutive bytes hash table (LZ77)

class Entry3B
{
public:
	u16 next; 		// next entry : just 12 lsb bits
	u8 pos;			// position / offset in 4k bloc (from 0 to 4093) 4 bits from next + those 8 => 12 bits
	u8 a,b,c;		// the 3 following bytes
};

class HashTable3B
{
public:
	HashTable3B();
	void ProcessBloc(void* ad_bloc);

private:
	u16 CalcHash(u8 *pt);			// hash on next 3 bytes => return a 24 bits hash (half the size of key)
	vector<u16> the_hash_table_;	// 12 bits index, so 4096*2 bytes = 8KB
	vector<Entry3B> table_entries_;	// 6 bytes per entry, so 4096*6 = 24KB
};

// Extreme cases : 4k bloc filled with the same byte (4094 entries on the same linked list,
//                   with the other entries void)
//                 4k bloc with arbitrary data : all linked list will be with only one entry

#endif /* HASH_TABLE_3BYTES_H_ */
