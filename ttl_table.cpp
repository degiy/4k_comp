#include "ttl_table.h"
#include <iostream>

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

void TTLTable::Add(pTTLEntry p_ent)
{
	// find the best matching bloc inside the TTL table
	const pTTLEntry& p_match=BestMatch(p_ent);

	// we have a full table, we need to remove the weak link
	if (size()>=nb_max_entries_)
	{
		erase(begin());
	}
	// insert into multimap p_ent
	emplace(make_pair(default_ttl_+water_mark_,move(p_ent)));
	// increase water mark
	water_mark_++;
}

const pTTLEntry& TTLTable::BestMatch(const pTTLEntry &p_ent)
{
	u16 max=0;
	auto pmax=end();

	if (verbose)
		cout<<"matches for bloc "<<dec<<p_ent->id_page_<<endl;
	for(auto pt=begin();pt!=end();++pt)
	{
		u16 nb=p_ent->bitmap_.Compare(pt->second->bitmap_);
		if (nb>max)
		{
			max=nb;
			pmax=pt;
			if (verbose)
				cout<<"  - new max with "<<dec<<max<<" matches (- "
				<<(p_ent->id_page_-pt->second->id_page_)<<")"<<endl;
		}
	}
	if (pmax != end())
	{
		// increase credit of best entry
		auto npmax = emplace(make_pair(pmax->first + bonus_, move(pmax->second)));
		erase(pmax);
		return npmax->second;
	}
	else
	{
		// no match (when only one entry in table, so return it)
		return p_ent;
	}
}

void TTLTable::Dump()
{
	cout<<"TTL table is "<<size()<<" elements, water mark at "<<water_mark_<<endl;
	for(auto pt=begin();pt!=end();++pt)
	{
		cout<<" -ttl "<<pt->first<<" for bloc "<<pt->second->id_page_<<endl;
	}
}
