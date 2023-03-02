#ifndef HASH_TABLE_3BYTES_H_
#define HASH_TABLE_3BYTES_H_

#include "common.h"
#include <vector>

using namespace std;

// building bloc for 3 consecutive bytes hash table (LZ77)

class Entry3B
{
public:
	inline void get_pos_and_next(u16 &pos,u16 &next) const
	{
		pos=(bytes[0]|(bytes[1]<<8))&0xfff;
		next=(bytes[1]|(bytes[2]<<8))>>4;
	}
	inline void set_pos_and_next(const u16 &pos,const u16 &next)
	{
		u32 l=pos;
		l|=(((u32)next)<<12);
		bytes[0]=l&0xff;
		l>>=8;
		bytes[1]=l&0xff;
		l>>=8;
		bytes[2]=l;
	}

	// 12 bits for next entry, and 12 for position inside 4k bloc (but in 3 bytes)
	u8 bytes[3];
	//	u8 pos_8low;			// position / offset in 4k bloc (from 0 to 4093) 8 lsb bits from pos 12 bits
	//	u8 next_4low_pos_4high;	// 4 msb from pos and 4 lsb from next
	//	u8 next_8hight; 		// next entry : just 8 msb bits from 12 from next
	// remark : as we don't save the 3 exact bytes inside the entry, we may look for a wrong 3 consecutive match
	//          (even 0 byte match), but we want to save memory. As we will have the 4k bloc in memory, better
	//			to check from the source.
};

class HashTable3B
{
public:
	HashTable3B();
	void ProcessBloc(void* ad_bloc);		// parse all 4k bloc to feed hash table

private:
	u16 CalcHash(u8 *pt);			// hash on next 3 bytes => return a 24 bits hash (half the size of key)
	vector<u16> the_hash_table_;	// 12 bits index, so 4096*2 bytes = 8KB
	vector<Entry3B> table_entries_;	// 3 bytes per entry, so 4096*3 bytes = 12KB

	friend class U3BParse;
};

// Extreme cases : 4k bloc filled with the same byte (4094 entries on the same linked list,
//                   with the other entries void)
//                 4k bloc with arbitrary data : all linked list will be with only one entry

// A utility class to parse hash table entries
class U3BParse
{
public:
	U3BParse(HashTable3B* table,void* ad_bloc);

	inline bool FirstEntry(u16 index) // run 3 bytes triplet along the table
	{
		u16 h=table_->CalcHash(bloc_+index);
		u16 e=table_->the_hash_table_[h];
		if (e==0) return 0;
		Entry3B& pe3b=table_->table_entries_[e];
		pe3b.get_pos_and_next(pos_,next_);
		return 1;
	}

	inline bool NextEntry() // get next entry if exist
	{
		if (next_==0) return 0;
		Entry3B& pe3b=table_->table_entries_[next_];
		pe3b.get_pos_and_next(pos_,next_);
		return 1;
	}

	inline u16 Compare(u16 index) // how many bytes match ?
	{
		u8 *p1=bloc_+pos_;
		u8 *p1i=p1;
		u8* p2=bloc_+index;
		while ((p2<p_end_)&&(*p1==*p2)) { p1++;p2++;}
		u16 dist=p1-p1i;
		if (dist<3) dist=0;
		return dist;
		// TODO : replace with a memcpm
	}

	class HashTable3B* table_;
	u8 *bloc_;
	u16 pos_;
	u16 next_;
	u8 *p_end_;

};


#endif /* HASH_TABLE_3BYTES_H_ */
