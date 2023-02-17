#include "common.h"
#include "compressor.h"
#include "ttl_table.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

Compressor::Compressor(vector<string> &input_files)
{
	if (verbose)
		cout<<"Running compression"<<endl;

	files_.resize(input_files.size());

	// map all files in memory
	u32 id_file=0;
	u32 id_bloc=0;
	File *pf=nullptr;
	for (auto itv = input_files.begin(); itv != input_files.end(); ++itv)
	{
		pf=&files_.at(id_file);
		pf->LoadFile(*itv,id_file+1);
		tree_4k_[id_bloc]=pf;
		id_bloc+=pf->nb4k;
		id_file++;
	}
	last_bloc_id_=id_bloc;

	// find identical blocs
	SameBlocs();

	// caracterize blocs and compare to others
	TTLTable tt(10,5,2);
	for (u32 i=0;i<last_bloc_id_;i++)
	{
		TTLEntry te(i,AddressByGlobalBlocId(i));
		tt.Add(te);
	}
	//ParseBlocs();
}

map<u32,File*>::iterator Compressor::MapEntryByGlobalBlocId(u32 id_bloc)
{
	// find the file with first bloc just equal or just after our bloc id
	auto it=tree_4k_.lower_bound(id_bloc);
	if ( (it==tree_4k_.end()) || (it->first>id_bloc) )
	{
		// we are just too far, we need to rewind one step to find the right file
		--it;
	}
	return it;
}

File *Compressor::FileByGlobalBlocId(u32 id_bloc)
{
	// find the file with first bloc just equal or just after our bloc id
	auto it=MapEntryByGlobalBlocId(id_bloc);
	return it->second;
}

void *Compressor::AddressByGlobalBlocId(u32 id_bloc)
{
	// find the file with first bloc just equal or just after our bloc id
	auto it=MapEntryByGlobalBlocId(id_bloc);
	// we deduce the address of the bloc
	u32 bloc_offset=id_bloc-it->first;
	if (bloc_offset>=it->second->nb4k)
	{
		// oups we want to access a bloc beyond the limit of the last file
		cerr<<"try to access a 4k bloc out of limit of file "<<it->second->name<<endl;
		exit (-1);
	}
	void *pt_bloc=(void*)((u64)it->second->addr+((u64)(bloc_offset)<<12));
	return pt_bloc;
}

bool Compressor::CompareBlocs(u32 b1,u32 b2)
{
	void* a1=AddressByGlobalBlocId(b1);
	void* a2=AddressByGlobalBlocId(b2);

	int ret=memcmp(a1,a2,4096);
	return ret==0;
}

// 4k same bloc
void Compressor::SameBlocs()
{
	// we use a table with as many entries as we have 4k blocs
	// TBD : add an argument to command line to define table size
	HashTable4k table(last_bloc_id_+1,*this);

	for (u32 i=0;i<last_bloc_id_;i++)
	{
		u32 ii=table.ProcessBloc(i);
		if (ii!=0xffffffff)
		{
			// duplicated bloc, we need to keep track of it, as we won't work on it anymore for further comp.
			bloc_is_same_than_[i]=ii;
		}
	}
}


// just for debug purposes
void Compressor::ParseBlocs()
{
	for (u32 i=0;i<=last_bloc_id_+1;i++)
	{
		cout<<hex<<" id : "<<i<<", ad : "<<AddressByGlobalBlocId(i)<<endl;
	}
}
