#ifndef TTL_TABLE_H_
#define TTL_TABLE_H_

#include "categorize_blocs.h"
#include "hash_table_3bytes.h"

#include <map>

using namespace std;

// TTL entry for mmap, contains :
// - page id
// - bitmap of byte occurence above a threshold
// - hashtable with all 3 consecutive bytes
class TTLEntry
{
public:
	TTLEntry(u32 id,void *ad);

private:
	u32 id_page_;
	ByteOccurrence bitmap_;
	HashTable3B ht_3b_;

	friend class TTLTable;
};

// multimap for ttl in pages, indexed by TTL value (as we need to purge less used pages)
// a maximum number of page are kept inside this table
class TTLTable : multimap<u32,TTLEntry>
{
public:
	TTLTable(u32 nb_max,u32 ttl,u32 bonus);
	void Add(const TTLEntry &ent);

private:
	const TTLEntry& BestMatch(const TTLEntry &ent) const;
	u32 nb_max_entries_;	// max entries of multimap : once full we remove the one with the lower key
	u32 default_ttl_;		// advance given from watermark to remains alive in the table
	u32 water_mark_;		// a counter who increase at each bloc insertion (never stops)
	u32 bonus_;				// a small push up given to a bloc who match
							// with a value of 1, it will finish by disapear except if always best match
							// with a value of 2, it will remain in the table if matched at least 50% of time
							// with a value of 10, it will remain if matched at least 10% of time
							// must be chosen in relation with nb_max_entries_ (e.g. at least smaller)
};



#endif /* TTL_TABLE_H_ */
