/* -*- mode: C++; c-default-style: "linux" ; c-basic-offset: "2" ; substatement-open: "0" -*- */

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <dirent.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

#include "common.h"
#include "compressor.h"

int verbose{0};
int debug{0};

// forward declarations of other fonctions than main
void fill_entries_from_file();
void fill_entries_from_dir();

const char *argp_program_version = "4kcomp 0.1";
const char *argp_program_bug_address = "<jerome_jansou@yahoo.fr>";
static char doc[] = "4kcomp : a multiple vm data file compression tool";

/* A description of the arguments we accept. */
static char args_doc[] = "<file_to_compress> ...";

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Increase verbose level"},
    {"debug", 'D', 0, 0, "Increase debug level"},
    {"compress", 'c', 0, 0, "Compress/Pack files into archive"},
    {"decompress", 'd', 0, 0, "Decompress/Unpack files from archive"},
    {"from-file", 'f', "FILE", 0, "Use files names located into FILE as input for compression"},
    {"directory", 'r', "DIR", 0, "Will use all files from directory DIR as input for compression (not recursive)"},
    {"output", 'o', "FILE", 0, "Output to FILE when compressing"},
    {"input", 'i', "FILE", 0, "FILE to read from when decompressing"},
    {"list", 'l', 0, 0, "instead of decompressing, just display files names into archive"},
    {0}};

/* Used by main to communicate with parse_opt. */
struct arguments
{
  int verbose, debug, comp, decomp, list;
  char *output_file, *from_dir, *from_file, *input_file;
};

vector<string> in_files;

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we know is a pointer to our arguments structure. */
  struct arguments *arguments = (struct arguments *)state->input;
  
  switch (key)
  {
  case 'D':
    arguments->debug++;
    break;
  case 'v':
    arguments->verbose++;
    break;
  case 'o':
    arguments->output_file = arg;
    break;
  case 'i':
    arguments->input_file = arg;
    break;
  case 'f':
    arguments->from_file = arg;
    break;
  case 'r':
    arguments->from_dir = arg;
    break;
  case 'c':
    arguments->comp = 1;
    break;
  case 'd':
    arguments->decomp = 1;
    break;
  case 'l':
    arguments->decomp = 1;
    arguments->list = 1;
    break;
  case ARGP_KEY_ARG:
    // faudra traiter arg pour la liste des fichiers � lire
    in_files.push_back(string(arg));
    break;
  case ARGP_KEY_END:
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

/* argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};
struct arguments arguments;

int main(int argc, char **argv)
{
  /* Default values for all arguments */
  memset((void *)&arguments, 0, sizeof(arguments));

  /* Parse our arguments; every option seen by parse_opt will be reflected in arguments. */
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if (arguments.debug)
  {
    cout << "debug=" << arguments.debug
         << ", verbose=" << arguments.verbose
         << ", comp=" << arguments.comp
         << ", decomp=" << arguments.decomp
         << ", list=" << arguments.list;
    if (arguments.output_file)
      cout << ", ofile=" << arguments.output_file;
    if (arguments.input_file)
      cout << ", ifile=" << arguments.input_file;
    if (arguments.from_dir)
      cout << ", fdir=" << arguments.from_dir;
    if (arguments.from_file)
      cout << ", ffile=" << arguments.from_file;
    cout << endl;
    cout << "files to compress :";
    for (auto it = in_files.begin(); it != in_files.end(); ++it)
      cout << ' ' << *it;
    cout << endl;
  }
  // check trivial impossible combinations (like compression + decompression)
  if (arguments.comp && arguments.decomp)
  {
    cerr << "you should choose between compression (-c) or decompression (-d) option, not both\n";
    exit(-1);
  }
  else if (!(arguments.comp || arguments.decomp))
  {
    cerr << "you should choose between compression (-c) or decompression (-d) option\n";
    exit(-1);
  }
  else if (arguments.comp && (!arguments.output_file))
  {
    cerr << "when compressing, you need to choose an output file (-o)\n";
    exit(-1);
  }
  else if (arguments.decomp && (!arguments.input_file))
  {
    cerr << "when decompressing, you need to choose an input file (-i)\n";
    exit(-1);
  }

  // global variables to set before launching compressor or decompressor
  debug = arguments.debug;
  verbose = arguments.verbose;

  // so it is compression ?
  if (arguments.comp)
  {
    // process input file list feeding
    if (arguments.from_file)
    {
      // from a file
      fill_entries_from_file();
    }
    if (arguments.from_dir)
    {
      // from a directory (just plain files without recursion)
      fill_entries_from_dir();
    }
    if (in_files.size() == 0)
    {
      cerr << "when compressing, you need to provide input files from command line or through options -r or -f\n";
      exit(-1);
    }
    else if (arguments.debug)
    {
      cout << "files to compress (with all options) :";
      for (auto it = in_files.begin(); it != in_files.end(); ++it)
        cout << ' ' << *it;
      cout << endl;
    }
    // run the compression
    Compressor c(in_files);
  }
  else
  {
    // no it's decompression
  }

  return 0;
}

void fill_entries_from_file()
{
  fstream in_f;
  in_f.open(arguments.from_file, ios::in);
  if (in_f.is_open())
  {
    string line;
    while (getline(in_f, line))
    {
      in_files.push_back(line);
    }
    in_f.close();
  }
  else
  {
    cerr << "cannot read file " << arguments.from_file << endl;
    exit(-1);
  }
}

void fill_entries_from_dir()
{

  struct dirent *entry = nullptr;
  DIR *dp = nullptr;

  dp = opendir(arguments.from_dir);
  if (dp != nullptr)
  {
    while ((entry = readdir(dp)))
    {
      if (entry->d_type == DT_REG)
      {
        in_files.push_back(string(arguments.from_dir) + "/" + entry->d_name);
      }
    }
    closedir(dp);
  }
  else
  {
    cerr << "cannot read directory " << arguments.from_dir << endl;
    exit(-1);
  }
}
