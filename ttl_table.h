#ifndef TTL_TABLE_H_
#define TTL_TABLE_H_

#include "categorize_blocs.h"
#include "hash_table_3bytes.h"

#include <map>
#include <memory>

using namespace std;

// TTL entry for mmap, contains :
// - page id
// - bitmap of byte occurence above a threshold
// - hashtable with all 3 consecutive bytes

typedef unique_ptr<class TTLEntry> pTTLEntry;

class TTLEntry
{
public:
	TTLEntry(u32 id,void *ad);
	void LZ77();								// mostly for first bloc (no ancestry)
	void LZ77WithPast(const pTTLEntry& p_past);	// with 2 blocs (current one and best ancestry)

private:
	u32 id_page_;
	void* add_;
	ByteOccurrence bitmap_;
	HashTable3B ht_3b_;

	friend class TTLTable;
};

// multimap for ttl in pages, indexed by TTL value (as we need to purge less used pages)
// a maximum number of page are kept inside this table
class TTLTable : multimap<u32,pTTLEntry>
{
public:
	TTLTable(u32 nb_max,u32 ttl,u32 bonus);
	void Add(pTTLEntry p_ent);
	void Dump();

private:
	const pTTLEntry& BestMatch(const pTTLEntry& p_ent);
	u32 nb_max_entries_;	// max entries of multimap : once full we remove the one with the lower key
	u32 default_ttl_;		// advance given from watermark to remains alive in the table
	u32 water_mark_;		// a counter who increase at each bloc insertion (never stops)
	u32 bonus_;				// a small push up given to a bloc who match
							// with a value of 1, it will finish by disapear except if always best match
							// with a value of 2, it will remain in the table if matched at least 50% of time
							// with a value of 10, it will remain if matched at least 10% of time
							// must be chosen in relation with nb_max_entries_ (e.g. at least smaller)
};

// Best match could return a list of the N most look-alike blocs in order to populate the "past" from current
//  bloc by optimizing the chance to get on past occurrence of 3 bytes. Right now it's only a 4K past
//  (or 8K when we reach the end of the current bloc). So better choose the right one to make its past.
// As LZ77 use 32K past, we should use at least 8 blocs as possible past.


#endif /* TTL_TABLE_H_ */
