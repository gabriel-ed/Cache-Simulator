//============================================================================
// Name        : cache_memory.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
using namespace std;

void read_f();
void cache_addr(ifstream &file);


int main() {
	cout << "======================================" << endl;
	cout << "Cache Simulator w/ Direct Mapped cache" << endl;
	cout << "======================================" << endl;
	cout << "Program specifications: " << endl;
	cout << "Mapping Type: Direct"<< endl;
	cout << "Set associativity: 2-way associative" << endl;
	read_f();
	return 0;
}

void read_f() {
	ifstream addresses;
	try {
		addresses.open("E:/School/Fall2019/EGRE426/labs/Lab 5/Cache-Simulator/cache_memory/addresses.txt");
		if(!addresses.is_open()) {
			throw runtime_error("Unable to open file");
		}
		else
		{
			cache_addr(addresses);
		}


	} catch (exception& e) {
		cout << e.what() << '\n';
	}
	return;
}

void cache_addr(ifstream &addresses) {
	int i = 0;
	while(!addresses.eof()) {
		i++;
		cout<<"i = " << i << endl;
	}
	cout << "number of lines: " << i << endl;
	return;
}
