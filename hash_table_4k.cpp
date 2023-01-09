#include "hash_table_4k.h"
#include <iostream>
#include <stdlib.h>

HashTable4k::HashTable4k(u32 nb_of_entries, Compressor &comp) :
	the_hash_table_(3<<24), table_entries_(nb_of_entries),
	nb_bloc_read_(0), nb_entries_(0), ref_comp_(&comp)
{
}

// TBD : see what to do with return
u32 HashTable4k::ProcessBloc(u32 id_bloc)
{
	void *ad_bloc=ref_comp_->AddressByGlobalBlocId(id_bloc);

	// get the 64 bits hash
	u64 hash64=CalcHash(ad_bloc,4096);

	// hash64 is on the form A-B-C-D-E-F-G-H bytes
	// we xor CDE and FGH for hashtable index (24 bits)
	// and we keep the AB 16 bits word ad additional discrimination in linked list

	u32 key24=hash64&((1<<24)-1);
		key24^=(hash64>>24)&((1<<24)-1);
		//key24&=3; for tests on 4 lines
	u16 key16=hash64>>48;

	TableEntry *pe=nullptr;

	u32 *next=&(the_hash_table_.at(key24));
	if (*next==0)
	{
		// line is empty, we will intialise hashtable line with a new linked list
		if (debug>1) cout<<"bloc "<<id_bloc<<" new hash line "<<key24<<endl;
	}
	else
	{
		// we run the linked list to find equal hash, if not we add
		// and element at the end of the list
		do
		{
			TableEntry *pe=&(table_entries_.at(*next));
			if (pe->end_key==key16)
			{
				// we have a hash match, need to compare both blocs to be sure
				if (ref_comp_->CompareBlocs(id_bloc,pe->bloc_id))
				{
					// ok it matches
					pe->hits++;
					if (debug) cout<<"bloc "<<id_bloc<<" matches same bloc "<<pe->bloc_id<<endl;
					return pe->bloc_id;
				}
			}
			next=&(pe->next);
		}
		while (*next);
		// we reached the end of linked list
	}
	//  we initiate a new element in linked list
	// (coming from hashtable or last element)
	*next=++nb_entries_;
	pe=&(table_entries_.at(nb_entries_));
	pe->next=0;
	pe->bloc_id=id_bloc;
	pe->hits=0;
	pe->end_key=key16;
	if (debug>1) cout<<"bloc "<<id_bloc<<" has no match"<<endl;

	return 0xffffffff;
}

inline u64 rotl64 ( u64 x, u8 r )
{
  return (x << r) | (x >> (64 - r));
}

u64 HashTable4k::fmix64 (u64 k )
{
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccd;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53;
	k ^= k >> 33;

	return k;
}

u64 HashTable4k::CalcHash(void *ad_bloc, u64 bloc_size)
{
	if (((u64)ad_bloc)&15)
	{
		cerr<<"Error : getting unaligned block at address "<<hex<<(u64)(ad_bloc)<<endl;
		exit(-1);
	}
	u64 *p64 = (u64 *)(ad_bloc);
	u64 *pend= (u64*)((u64)(ad_bloc)+bloc_size);

	u64 h1 = 0;
	u64 h2 = 0;

	const u64 c1 = 0x87c37b91114253d5;
	const u64 c2 = 0x4cf5ad432745937f;

	while(p64<pend)
	{
		u64 k1 = *p64++;
		u64 k2 = *p64++;

		k1 *= c1; k1  = rotl64(k1,31); k1 *= c2; h1 ^= k1;
		h1 = rotl64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

		k2 *= c2; k2  = rotl64(k2,33); k2 *= c1; h2 ^= k2;
		h2 = rotl64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
	}

	h1 += h2;
	h2 = fmix64(h1);

	return h2;
}

