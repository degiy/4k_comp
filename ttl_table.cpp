#include "ttl_table.h"
#include <iostream>

// calculate bitmap and 3 bytes hashtable
TTLEntry::TTLEntry(u32 id,void *ad) : id_page_(id), add_(ad)
{
	// bitmap calculation
	bitmap_.ParseBloc(add_);
	// 3B hashtable for LZ77
	ht_3b_.ProcessBloc(add_);
}

void TTLEntry::LZ77()
{
	// Utility init
	U3BParse parser(&ht_3b_,add_);

	// bloc parsing
	u16 i=0;
	goto fail; // because index 0 always fails (nothing before when only a single 4k bloc)
	while (i<4094)
	{
		if (parser.FirstEntry(i))
		{
			// ok we may have some past reference, let's check
			// re-init counters to find best one (largest and nearer in past)
			u16 max_match=0;
			u16 best_pos=0;
			// ok we have an entry, maybe a compression to make
			do
			{
				// first check if ref is after or on index
				if (parser.pos_<i)
				{
					// ok, now check how many bytes match
					u16 match=parser.Compare(i);
					if (match>max_match)
					{
						max_match=match;
						best_pos=parser.pos_;
					}
				}
			}
			while (parser.NextEntry());
			if (max_match==0) goto fail;
			// we have a best match
			// TO DO : output ref
			cout<<"match "<<max_match<<" at "<<best_pos<<" for index "<<i<<endl;
			i+=max_match;
			continue;
		}
		fail:
		// no entry, so no possible compression here
		// TO DO : output single byte without compression
		i++;
	}
}

void TTLEntry::LZ77WithPast(const pTTLEntry& p_past)
{

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
	auto n_p_ent =emplace(make_pair(default_ttl_+water_mark_,move(p_ent)));
	// increase water mark
	water_mark_++;

	// now we need to choose a compression algorithm
	if (p_match!=n_p_ent->second)
	{
		cout<<"different"<<endl;
		// TODO : code LZ77 for 2 blocs
		n_p_ent->second->LZ77();
	}
	else
	{
		cout<<"first"<<endl;
		n_p_ent->second->LZ77();
	}
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

