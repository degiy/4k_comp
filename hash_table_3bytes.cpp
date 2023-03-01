#include "hash_table_3bytes.h"

HashTable3B::HashTable3B(): the_hash_table_(4096,0), table_entries_(4096,{0,0,0})
{
}

void HashTable3B::ProcessBloc(void *ad_bloc)
{
	u8 *bl=(u8*)ad_bloc;

	// we start at 1 as 0 would means no entry
	// (so entry 0 is not used to avoid substracting one between next and table entry)
	for (u16 i=1;i<=4094;i++)
	{
		u16 next=0;		// by default we will be at the end of list, so no next

		// index work
		u16 hash=CalcHash(bl+i-1);
		if (the_hash_table_[hash])
		{
			// as we need to take the place as first entry of a current entry
			// we need to reference it as next
			next=the_hash_table_[hash];
		}
		// anyway we need to place the new entry in the hash table (as first entry for index)
		the_hash_table_[hash]=i;	// right on pabc

		// put next and pos on a 3 bytes word
		// struct feeding
		Entry3B *pabc=&table_entries_[i];
		pabc->set_pos_and_next(i-1,next);
	}
}

u16 HashTable3B::CalcHash(u8 *pt)
{
	// we keep one nimble (quartet = 4bits) per byte by xoring the 4 MSb and the 4 LSb
	// for the 3 successive bytes
	u16 q3=(*pt&0xf)^(*pt>>4);
	q3<<=4;
	pt++;
	q3|=(*pt&0xf)^(*pt>>4);
	q3<<=4;
	pt++;
	q3|=(*pt&0xf)^(*pt>>4);
	return q3;
}
