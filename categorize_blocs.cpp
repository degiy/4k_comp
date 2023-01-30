#include "categorize_blocs.h"

#include <cstring>
#include <ostream>
#include <iostream>

vector<u8> ByteOccurrence::precalc_bit_count_table_(1<<16);
u32 ByteOccurrence::count_{0};

ByteOccurrence::ByteOccurrence() : w1{0},w2{0},w3{0},w4{0}
{
	if (count_++==0)
	{
		Precalc();
	}
}

// precalc of a table to count bit at one in all 16 bits word from 0x0000 to 0xffff
// count (0 to 16) stored on a 8 bits word
void ByteOccurrence::Precalc()
{
	for (u16 i=1;i>0;i++)
	{
		u8 cp=CountOnes(i);
		precalc_bit_count_table_[i]=cp;
	}
}

u8 ByteOccurrence::CountOnes(u16 ones_and_zeros)
{
	u8 cp=0;
	while(ones_and_zeros)
	{
		if (ones_and_zeros&1) cp++;
		ones_and_zeros>>=1;
	}
	return cp;
}

u16 ByteOccurrence::Compare(const ByteOccurrence &second)
{
	u16 cp=0;
	u64 ws[4];
	ws[0]=w1&second.w1;
	ws[1]=w2&second.w2;
	ws[2]=w3&second.w3;
	ws[3]=w4&second.w4;

	u16* pt=(u16*)(&ws[0]);
	u16* pte=(u16*)( (u64)((&ws[0])+32) );
	while (pt!=pte)
	{
		cp+=precalc_bit_count_table_[*pt++];
	}
	return cp;
}

#define THRES 16

void ByteOccurrence::ParseBloc(void *ad_bloc)
{
	u16 cps[256];
	u8* pt=(u8*)ad_bloc;
	u8* pte=pt+4096;

	memset(&cps,0,512);

	// parsing of bytes in block
	while (pt<pte)
	{
		cps[*pt++]++;
	}

	// are we above threshold
	u16 i=0;
	while (i<64)
	{
		w1<<=1;
		if (cps[i++]>THRES) w1|=1;
	}
	while (i<128)
	{
		w2<<=1;
		if (cps[i++]>THRES) w2|=1;
	}
	while (i<192)
	{
		w3<<=1;
		if (cps[i++]>THRES) w3|=1;
	}
	while (i<256)
	{
		w4<<=1;
		if (cps[i++]>THRES) w4|=1;
	}
	if (debug)
	{
		cout<<"bloc at "<<hex<<(u64)ad_bloc<<" : occurrences of bytes above threshold "
		<<dec<<THRES<<" : ";
		Show();
	}
}

void ByteOccurrence::Show()
{
	char buf[1024];
	sprintf(buf,"%016lx,%016lx,%016lx,%016lx",w1,w2,w3,w4);
	cout<<buf<<endl;
}
