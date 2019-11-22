//============================================================================
// Name        : cache_memory.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#define MAX_SIZE 10000 // max size of inputs accepted

//global variable (input stream)
unsigned short int input_arr[MAX_SIZE];
int size_file;
unsigned int num_cache_lines;
unsigned int main_cache[MAX_SIZE][MAX_SIZE]; //initialize cache block of block_size rows and 1 column

// global structures
struct cache_type {
	int block_size = 16; // default to 16 bits
	int num_blocks = 4; // default to 4 blocks
	int associativity = 1; //default to direct
	int policy; // default to random policy
} user;

struct block_data {
	unsigned int index;
	unsigned int tag;
	unsigned int data;
	bool valid = false;
} blocks[MAX_SIZE];
//define functions
void read_f();
int cache_addr(ifstream &file); // read values from file
void user_args();
void block_calc();
void cache_build();
// main execution
int main() {
	cout << "======================================" << endl;
	cout << "Cache Simulator w/ Direct Mapped cache" << endl;
	cout << "======================================" << endl;
	cout << "Program specifications: " << endl;
	cout << "Mapping Type: Direct"<< endl;
	cout << "Set associativity: 2-way associative" << endl;
	cout << endl;
	user_args();
	read_f();
	cout << "The size of the file (in terms of number of lines) is " << size_file << endl;
	block_calc();
	return 0;
}

void read_f() {
	try {
		ifstream file("test.txt");
		if(!file.is_open()) {
			throw runtime_error("Unable to open file");
		}
		else
		{
			size_file = cache_addr(file);
		}


	} catch (exception& e) {
		cout << e.what() << '\n';
	}
	return;
}

int cache_addr(ifstream &file) {
	int i = 0; // number of lines within file
	do {
		file >> input_arr[i];
		i+=1;
	} while(!file.eof());
	cout<<endl <<"There are " << i << " lines within this file." << endl;
	return i;
}

void user_args() {
	try {

		cout<<"How wide will each block be? (Hint: width of the block in words)" << endl;
		cin >> user.block_size;

		if(user.block_size < 1) {
			throw "Illegal entry. Please input a value equal too or larger than a quarter-word (1 bytes) ";
		}

		cout<<"How many blocks should the cache contain? (Hint: # of blocks in the cache = power of 2)" << endl;
		cin >> user.num_blocks;

		cout <<"Set-Associative type? (Hint: 1 for direct, 2 for two-way set associative)" << endl;
		cin >> user.associativity;

		cout <<"What type of replacement policy should we incorporate? (Hint: 1 for random, 2 for Least Recently Used)" << endl;
		cin >> user.policy;

	} catch (const char *e) {
		cout << e << '\n';
		cout<<'\n';
		user_args();
		return;
	}
	return;
}

void block_calc() {

	//unsigned int main_cache[user.num_blocks]; //initialize cache block of block_size rows and 1 column
	//unsigned int block_cont[user.block_size]; //initialize block array of block size
	unsigned int local_vals[size_file];
	num_cache_lines = (user.num_blocks*user.block_size)/(user.associativity*user.block_size);
	// 1. offset of cache = user.block_size * 4 (# words * (4 bytes/word)) = 2^offset_bits
	// i.e 16 block size = 16 * 4 => 64 = 6 bits for offset
	// i.e 8 block size = 8 * 4 => 32 = 5 bits for offset

	// FOR FULLY ASSOCIATIVE CACHE, THERE ARE NO INDEX BITS
	// 2. index of cache (cache size) = user.num_blocks = 2^user.num_blocks
	// i.e 2^x = num_blocks => x = log[base2](num_blocks) = index (# of bits)

	// 3. tag of cache = remainder of bits not used by index and offset
	// i.e. 32 bits total : [total # of bits] - offset - index = tag


	for(int i = 0; i < size_file; i++) {
		local_vals[i] = input_arr[i];
	}
	cout<<"finished copying into new arr"<<endl;

	for(int j = 0; j < size_file; j++) {
		if(local_vals[j] == 0) {
			blocks[j].index = 0;
		} else {
			blocks[j].index = local_vals[j] % num_cache_lines; //grab index of block at address j
		}
	}

	for(int i = 0; i < size_file; i++) {
		blocks[i].tag = (local_vals[i])/(num_cache_lines * user.block_size);
	}

	for(int i = 0; i < size_file; i++) {
		blocks[i].data = local_vals[i];
	}
	cout<<"finished populating block array.offset"<<endl;
	for(int k = 0; k < size_file; k++) {
		cout<<"Cache Entry " <<k<<endl;
		cout<<"Block index: "<< blocks[k].index << endl;
		cout<<"Block tag: " << blocks[k].tag<<endl;
		cout<<"Block data: " << blocks[k].data<<endl<<endl<<endl;
	}

	cache_build();

	return;
}

void cache_build() {
	int new_int;
	for(int i = 0; i < size_file; i++) {
		new_int = (int) user.num_blocks;
		main_cache[blocks[i].index][new_int];
	}
	if (user.associativity == 1) { // direct mapped
		//row = index, //col = block
		for(int i = 0; i < user.block_size;i++) {
			for(int j = 0; j < user.num_blocks; j++) {
				main_cache[blocks[i].index][blocks[j].index] = blocks[j].data;
				cout<<main_cache[i][j]<<" ";
			}
		}
	}




	return;
}
