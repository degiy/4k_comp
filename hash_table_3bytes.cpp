#include "hash_table_3bytes.h"

HashTable3B::HashTable3B(): table_entries_(4096,{0,0,0,0,0}), the_hash_table_(4096,0)
{
}

void HashTable3B::ProcessBloc(void *ad_bloc)
{
	u8 *bl=(u8*)ad_bloc;

	// we start at 1 as 0 would means no entry
	// (so entry 0 is not used to avoid substracting one between next and table entry)
	for (u16 i=1;i<=4094;i++)
	{
		// struct feeding
		Entry3B *pabc=&table_entries_[i];
		pabc->a=bl[i-1];
		pabc->b=bl[i];
		pabc->c=bl[i+1];
		pabc->pos=(i-1)&255;
		pabc->next=((i+1)&0x700)<<8;

		// index work
		u16 hash=CalcHash(bl);
		if (the_hash_table_[hash])
		{
			// we need to insert the new (a,b,c) triplet at beginning of linked list
			pabc->next|=the_hash_table_[hash];
		}
		// else or anyway we need to place the index of pabc in the hash table index
		the_hash_table_[hash]=i;	// right on pabc
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
