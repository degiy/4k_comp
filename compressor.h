#ifndef COMPRESSOR_H_
#define COMPRESSOR_H_

#include "file.h"
#include "common.h"
#include "hash_table_4k.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

class Compressor
{
public:
	Compressor(vector<string> &input_files);
	File* FileByGlobalBlocId(u32 id_bloc);
	void* AddressByGlobalBlocId(u32 id_bloc);
	bool CompareBlocs(u32 b1,u32 b2);
	void ParseBlocs();

private:
	void SameBlocs();

	map<u32,File*>::iterator MapEntryByGlobalBlocId(u32 id_bloc);
	void MapFile(string &file);

	vector<File> files_;
	map<u32,File*> tree_4k_;
	u32 last_bloc_id_;

	map<u32,u32> bloc_is_same_than_;
};

#endif
