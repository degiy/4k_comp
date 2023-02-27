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
	// increase water mark
	water_mark_++;
}

const TTLEntry& TTLTable::BestMatch(const TTLEntry &ent)
{
	u16 max=0;
	multimap<u32,TTLEntry>::iterator pmax=end();

	if (verbose)
		cout<<"matches for bloc "<<dec<<ent.id_page_<<endl;
	for(auto pt=begin();pt!=end();++pt)
	{
		u16 nb=ent.bitmap_.Compare(pt->second.bitmap_);
		if (nb>max)
		{
			max=nb;
			pmax=pt;
			if (verbose)
				cout<<"  - new max with "<<dec<<max<<" matches (- "
				<<(ent.id_page_-pt->second.id_page_)<<")"<<endl;
		}
	}
	if (pmax != end())
	{
		// increase credit of best entry (not optimum, because of recopy)
		auto npmax = insert(make_pair(pmax->first + bonus_, pmax->second));
		erase(pmax);
		return npmax->second;
	}
	else
	{
		// return the input value as we don't have other blocs to compare to
		return ent;
	}
}

void TTLTable::Dump()
{
	cout<<"TTL table is "<<size()<<" elements, water mark at "<<water_mark_<<endl;
	for(auto pt=begin();pt!=end();++pt)
	{
		cout<<" -ttl "<<pt->first<<" for bloc "<<pt->second.id_page_<<endl;
	}
}
