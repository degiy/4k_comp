#include "ttl_table.h"

// calculate bitmap and 3 bytes hashtable
TTLEntry::TTLEntry(u32 id,void *ad) : id_page_(id)
{
	// bitmap calculation
	bitmap_.ParseBloc(ad);
	// 3B hashtable for LZ77
	ht_3b_.ProcessBloc(ad);
}

TTLTable::TTLTable(u32 nb_max,u32 ttl,u32 bonus) : nb_max_entries_{nb_max}, default_ttl_{ttl},
		water_mark_{0}
{

}

void TTLTable::Add(const TTLEntry &ent)
{
	// find the best matching bloc inside the TTL table
	const TTLEntry& match=BestMatch(ent);

	// we have a full table, we need to remove the weak link
	if (size()>=nb_max_entries_)
	{
		erase(begin());
	}
	// insert into multimap
	pair<u32,TTLEntry> p(default_ttl_+water_mark_,ent);
	insert(p);
}

const TTLEntry& TTLTable::BestMatch(const TTLEntry &ent) const
{
	u16 max=0;
	auto pmax=begin();

	for(auto pt=begin();pt!=end();++pt)
	{
		u16 nb=ent.bitmap_.Compare((*pt).second.bitmap_);
		if (nb>max)
		{
			max=nb;
			pmax=pt;
		}
	}
	return (*pmax).second;
}
