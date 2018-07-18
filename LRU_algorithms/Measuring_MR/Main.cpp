#include <bits/stdc++.h>
#include "FAIR_LRU.h"

const int key_len_1 = 13;
const int key_len_2 = 4;
const int MAX_ITE = 1000000;

map<string, bool> key_map;


int main(int argc, char **argv) {
	ifstream fin(argv[1], ios::in | ios::binary);
	ofstream fout(argv[2], ios::app);
	
	int memory = atoi(argv[3]), w3 = atoi(argv[4]), w2 = atoi(argv[5]);
	
	FAIR_LRU<key_len_2> *key_lru = new FAIR_LRU<key_len_2>(memory * 1024, w3, w2);

	uint8_t key[key_len_1];
	
	int s_cou = 0;
	double s_ran = 0;

	for (int t = 0; t < MAX_ITE; t++) {
		fin.read((char *)key, key_len_1 * sizeof(uint8_t));
		
		string key_str = "";
		for (int i = 0; i < key_len_2; i++) {
			key_str += (char)key[i];
		}
		key_map[key_str] = true;
		
		int s_clo = key_lru->insert(key, t);
		
		if (s_clo != -1) {
			s_cou += 1;
			s_ran += key_lru->query(s_clo);
		}
	}

	fout << s_ran / s_cou << "	";
	cout << s_ran / s_cou << endl;
}
