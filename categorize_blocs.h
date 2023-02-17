#ifndef CATEGORIZE_BLOCS_H_
#define CATEGORIZE_BLOCS_H_

#include "common.h"

#include <vector>

using namespace std;

// class dedicated to manage the bitmap of byte presence inside the 4k block
class ByteOccurrence
{
public:
	ByteOccurrence();
	u16 Compare(const ByteOccurrence &second) const;	// do an AND on all the bits and return the occurrence of ones
	void ParseBloc(void *ad_bloc);				// read 4k bloc and calculate bytes occurrences
	u64 w1;
	u64 w2;
	u64 w3;
	u64 w4;

private:
	void Precalc();						// precalc of number of 1 in each u16 => precalc_bit_count_table_
	u8 CountOnes(u16 ones_and_zeros);	// count ones in a u16
	void Show();						// show bitmap of occurrences

	static vector<u8> precalc_bit_count_table_;
	static u32 count_;
};


#endif /* CATEGORIZE_BLOCS_H_ */
