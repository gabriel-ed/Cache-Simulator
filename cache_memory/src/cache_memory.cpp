//===========================================================================
// Name : cache_memory.cpp
// Author : Gabriel Smith
// Version :
// Copyright : Your copyright notice
// Description :
//===========================================================================

#include <iostream>

#include <fstream>

#include <string>

using namespace std;
#define MAX_SIZE 10100 // max size of inputs accepted
// global variable (input stream)
int input_arr[MAX_SIZE];
int size_file;
int num_cache_lines;
int main_cache[MAX_SIZE][5]; // initialize cache block of block_size rows and 1 column
int total_reads;
// global structures
struct cache_type {
    int block_size = 16; // default to 16 bits
    int num_blocks = 4; // default to 4 blocks
    int associativity = 1; // default to direct
    int policy; // default to random policy
}
user;
struct block_data {
    int index;
    int tag;
    int data;
    int addr;
    bool hit = false;
    bool valid = false;
}
blocks[MAX_SIZE];
// define functions
void read_f();
int cache_addr(ifstream & file); // read values from file
void user_args();
void block_calc();
void cache_build(int local_vals[]);
// main execution
int main() {
    cout << "======================================" << endl;
    cout << "Cache Simulator w/ Direct Mapped cache" << endl;
    cout << "======================================" << endl;
    cout << "Program specifications: " << endl;
    cout << "Mapping Type: Direct" << endl;
    cout << "Set associativity: 2-way associative" << endl;

    cout << endl;
    user_args();
    read_f();
    // cout << "The size of the file (in terms of number of lines) is " << size_file << endl;
    block_calc();
    return 0;
}
void read_f() {
    try {
        ifstream file("addresses.txt");
        if (!file.is_open()) {
            throw runtime_error("Unable to open file");
        } else {
            size_file = cache_addr(file);
        }
    } catch (exception & e) {
        cout << e.what() << '\n';
    }
    return;
}
int cache_addr(ifstream & file) {
    int i = 0; // number of lines within file
    string str;
    do {
        while (getline(file, str)) {
            input_arr[i] = stoi(str, 0, 16);
            i++;
        }
        /*file >> input_arr[i];
        i+=1;
        */
    } while (!file.eof());
    total_reads = i;
    return i;
}
void user_args() {
    try {
        cout << "How wide will each block be? (Hint: width of the block in words)" << endl;
    cin >> user.block_size;
    if (user.block_size < 1) {
        throw "Illegal entry. Please input a value equal too or larger than a quarter-word (1 bytes) ";
    }
    cout << "How many blocks should the cache contain? (Hint: # of blocks in the cache = power of 2)" << endl;
    cin >> user.num_blocks;
    cout << "Set-Associative type? (Hint: 1 for direct, 2 for two way set associative)" << endl;
    cin >> user.associativity;
    cout << "What type of replacement policy should we incorporate? (Hint: 1 for random, 2 for Least Recently Used)" <<endl;
    cin >> user.policy;
}
catch (const char * e) {
    cout << e << '\n';
    cout << '\n';
    user_args();
}
return;
}
void block_calc() {
    // unsigned int main_cache[user.num_blocks]; //initialize cache block of block_size rows and 1 column
    // unsigned int block_cont[user.block_size]; //initialize block array of block size
	int local_vals[size_file];
    num_cache_lines = (user.num_blocks * user.block_size) / (user.associativity * user.block_size);
    // 1. offset of cache = user.block_size * 4 (# words * (4 bytes/word)) = 2^offset_bits
    // i.e 16 block size = 16 * 4 => 64 = 6 bits for offset
    // i.e 8 block size = 8 * 4 => 32 = 5 bits for offset
    // FOR FULLY ASSOCIATIVE CACHE, THERE ARE NO INDEX BITS
    // 2. index of cache (cache size) = user.num_blocks = 2^user.num_blocks
    // i.e 2^x = num_blocks => x = log[base2](num_blocks) = index (# of bits)
    // 3. tag of cache = remainder of bits not used by index and offset
    // i.e. 32 bits total : [total # of bits] - offset - index = tag
    for (int i = 0; i < size_file; i++) {
        local_vals[i] = input_arr[i];
    }
    for (int j = 0; j < size_file; j++) {
        if (local_vals[j] == 0) {
            blocks[j].index = 0;
        } else {
            blocks[j].index = local_vals[j] % num_cache_lines;
            // grab index of block at address j
        }
    }
    for (int i = 0; i < size_file; i++) {
        blocks[i].tag = (local_vals[i]) / (num_cache_lines * user.block_size);
    }
    for (int i = 0; i < size_file; i++) {
        blocks[i].data = local_vals[i];
    }
    /*for(int k = 0; k < size_file; k++) {
    cout<<"Cache Entry " <<k<<endl;
    cout<<"Block index: "<< blocks[k].index << endl;
    cout<<"Block tag: " << blocks[k].tag<<endl;
    cout<<"Block data: " << blocks[k].data<<endl<<endl<<endl;
    }*/
    cache_build(local_vals);
    return;
}
void cache_build(int local_vals[]) {
    int hit_count = 0;
    int miss_count = 0;
    if (user.associativity == 1) { // direct mapped
        // row = index, //col = block
        for (int i = 0; i < size_file; i++) {
            if ((main_cache[blocks[i].index][2] == true) &&
                (main_cache[blocks[i].index][1] == blocks[i].tag)) {
                hit_count += 1;
                main_cache[blocks[i].index][4] = true;
                // index_hit = i;
            } else {
                miss_count += 1;
            }
            if (blocks[i].hit == false) {
                main_cache[blocks[i].index][0] = blocks[i].data; // data stored into main cache
                main_cache[blocks[i].index][1] = blocks[i].tag;
                // tag stored into main cache
                main_cache[blocks[i].index][2] = true; // valid stored into main cache main_cache[blocks[i].index][3] = blocks[i].index; // index stored into main cache
            }
        }
        /*cout<<"Index"<<" "<<"Valid"<<" "<<"Tag"<<" "<<"Data"<<"
        "<<"Hit/Miss"<<endl;
        for(int i = 0; i < user.num_blocks; i++) {
        cout<<" "<<i<<" ";
        cout<<main_cache[i][2]<<" ";
        cout<<main_cache[i][1]<<" ";
        cout<<main_cache[i][0]<<" ";
        cout<<" "<<main_cache[i][4]<<" ";
        cout<<endl;
        }*/
        float hit_perc = float(hit_count) / (float(hit_count) + float(miss_count));
        float mis_perc = float(miss_count) / (float(hit_count) + float(miss_count));
        cout << "Cache size: 640k" << endl;
        cout << "Reads: " << total_reads << endl;
        cout << "Hits: " << hit_count << endl;
        cout << "Missess:" << miss_count << endl;
        cout << "Hit Rate: " << 100 * hit_perc << "%" << endl;
        cout << "Miss Rate: " << 100 * mis_perc << "%" << endl;
    }
    if (user.associativity == 2) { // 2 way associative
        user.num_blocks = (user.num_blocks) / 2;
        num_cache_lines = user.num_blocks;
        int set1[num_cache_lines][2] = {};
        int set2[num_cache_lines][2] = {};
        int place[size_file] = {};
        for (int j = 0; j < size_file; j++) {
            if (local_vals[j] == 0) {
                blocks[j].index = 0;
            } else {
                blocks[j].index = local_vals[j] % num_cache_lines; // grab index of block at address j
            }
        }
        for (int i = 0; i < size_file; i++) {
            blocks[i].addr = local_vals[i] / user.block_size;
            // grab the block addr
        }
        for (int i = 0; i < size_file; i++) {
            blocks[i].tag = (blocks[i].addr) / (num_cache_lines * user.block_size);
        }
        for (int i = 0; i < size_file; i++) {
            blocks[i].data = local_vals[i];
        }
        for (int i = 0; i < size_file; i++) { // i = set
            if ((blocks[blocks[i].index].valid == true) &&
                ((set1[blocks[i].index][0] == blocks[i].tag) ||
                    (set2[blocks[i].index][0] == blocks[i].tag))) {
                // cout<<"hit="<<hit_count<<endl;
                hit_count += 1;
            } else {
                // cout<<"miss="<<miss_count<<endl;
                miss_count += 1;
                blocks[blocks[i].index].valid = true;
            }
            if ((place[blocks[i].index] == 0) && (set2[blocks[i].index][1] != blocks[i].data)) {
                // cout<<"test"<<endl;
                set1[blocks[i].index][0] = blocks[i].tag;
                set1[blocks[i].index][1] = blocks[i].data;
                place[blocks[i].index] = 1;
            } else {
                // cout<<"set2"<<endl;
                set2[blocks[i].index][0] = blocks[i].tag;
                set2[blocks[i].index][1] = blocks[i].data;
                place[blocks[i].index] = 0;
            }
        }
        for (int i = 0; i < user.num_blocks; i++) {
            cout << "Set" << i << ": " << set1[i][1] << endl;
            cout << "Set" << i << ": " << set2[i][1] << endl;
            cout << endl;
        }
        float hit_perc = float(hit_count) / (float(hit_count) + float(miss_count));
        float mis_perc = float(miss_count) / (float(hit_count) + float(miss_count));
        cout << "Cache size: 640k" << endl;
        cout << "Reads: " << total_reads << endl;
        cout << "Hits: " << hit_count << endl;
        cout << "Misses:" << miss_count << endl;
        cout << "Hit Rate: " << 100 * hit_perc << "%" << endl;
        cout << "Miss Rate: " << 100 * mis_perc << "%" << endl;
        return;
    }
}
