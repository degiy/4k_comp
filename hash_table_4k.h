#ifndef HASH_TABLE_4K_H_
#define HASH_TABLE_4K_H_

#include "common.h"
#include "compressor.h"
#include <vector>

using namespace std;

// building bloc for 4k blocs hash table
//
// the first level of hash is the entry of the 24 bits index (3 bytes) of the key
// the second level is the end_key 16 bits field inside the structure of the chained list
// "next" starts at 1 (for the first bloc), 0 meaning no entry

class TableEntry
{
public:

	u32 next;		// next entry as index in the table : 0 if none
	u32 bloc_id;	// ref to block id (0 : for first 4k block of first file,...)
	u16 hits;		// how many time the 4k block is referenced (need explicit check between current 4k bloc and ref)
	u16 end_key;	// last 2 bytes of the 5 bytes keys (the first 3 are used as index in the_hash_table
};

class HashTable4k
{
public:
	HashTable4k(u32 nb_of_entries,class Compressor &comp);
	u32 ProcessBloc(u32 id_bloc); // return -1 if bloc has no sibling, else the "same bloc" id

private :
	u64 CalcHash(void *ad_bloc,u64 bloc_size);	// hash is based on murmur hash 3 (with no tail as we operate on 4k blocs)
	u64 fmix64(u64 k);							// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
	void SpaceNeeded();					// Table is full, we need to free some entries and rebuild table : TBD
										// v1.0 hypothesis is : we won't need to clean the table, everything is inside the table
										// (we'll have 1/300th of total disk file size as available RAM)

	vector<u32> the_hash_table_;			// limited to 24 bits (16M) entries of u32 = 67MB : very fast for 100GB-1TB of data
	vector<TableEntry> table_entries_;	// 12 bytes per entry, so the main memory consumer (12 bytes per 4k bloc)
										// if we use all entries (32 bits), it's 48GB RAM needed (but for 17TB of files)
	u32 nb_bloc_read_;	// total number of 4k blocs read
	u32 nb_entries_;	// total number of entries (or current one while processing blocks)
						// so normally less than nb_bloc_read_ because of same blocks find
	Compressor *ref_comp_;
};

// to clean table, we could drop one entry per 24 bits hash (per line, or linked-list associated to a hash)
// need to be based on hits and age (current bloc id distance)


#endif /* HASH_TABLE_4K_H_ */
