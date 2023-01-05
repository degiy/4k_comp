#ifndef COMPRESSOR_H_
#define COMPRESSOR_H_

#include "file.h"
#include "common.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

class Compressor
{
    public:
    Compressor(vector<string> &input_files);
    void* AddressByGlobalBlocId(u32 id_bloc);
  void ParseBlocs();

    private:
    void MapFile(string &file);

    vector<File> files_;
    map<u32,File*> tree_4k_;
    u32 last_bloc_id_;

};

#endif
