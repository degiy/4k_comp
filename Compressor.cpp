#include "common.h"
#include "compressor.h"

#include <iostream>
#include <vector>
#include <string>

Compressor::Compressor(vector<string> &input_files)
{
    if (verbose)
        cout<<"Running compression"<<endl;

    files_.resize(input_files.size());
    
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
    //ParseBlocs();
}

void *Compressor::AddressByGlobalBlocId(u32 id_bloc)
{
  // find the file with first bloc just equal or just after our bloc id
  auto it=tree_4k_.lower_bound(id_bloc);
  if ( (it==tree_4k_.end()) || (it->first>id_bloc) )
    {
      // we are just too far, we need to rewind one step to find the right file
      --it;
    }
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

// just for debug purposes
void Compressor::ParseBlocs()
{
  for (u32 i=0;i<=last_bloc_id_+1;i++)
    {
      cout<<hex<<" id : "<<i<<", ad : "<<AddressByGlobalBlocId(i)<<endl;
    }
}
