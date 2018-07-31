#include "BOBHash32.h"
#include <sstream>
#include <cstring>
#include <iostream>

using std::cout;
using std::endl;

	
bool les(int a, int b) {
	return a < b;
}
	
bool gre(int a, int b) {
	return a > b;
}

bool tru(int a, int b) {
	return true;
}

template<uint8_t key_len>
struct FAIR_LRU {
	int mem_in_bytes, w1, w3, w2;

	const int COUNTER_SIZE = 8;
	
	struct Counter {
		uint8_t key[key_len];
		int clo = -1;
	};
	
	Counter **level_1, *level_2;
	
	BOBHash32 *hash;

	public:
	string name;


	FAIR_LRU(int mem_in_bytes_, int w3_, int w2_) : mem_in_bytes(mem_in_bytes_), w3(w3_), w2(w2_) {
		/*
		 * Construct a cache structure with the given parameters.
		 *
		 * mem_in_bytes_: Total memory of the cache structure.
		 * w3: Number of counters in each bucket.
		 * w2: Number of counters in the coordinator.
		 */

		w1 = mem_in_bytes / COUNTER_SIZE / w3;

		level_1 = new Counter*[w1];
		for (int i = 0; i < w1; i++) {
			level_1[i] = new Counter[w3];
		}
		
		level_2 = new Counter[w2];

		random_device rd;
		hash = new BOBHash32(uint32_t(rd() % MAX_PRIME32));

		stringstream name_buf;
		name_buf << "FAIR_LRU@" << mem_in_bytes;
		name = name_buf.str();
	}

	void lookup(uint8_t *key, Counter *level, int len, int &cou_idx, int &emp_idx, int &lru_idx, bool (*oper) (int, int)) {
		/*
		 * Find the status of a key in a bucket or coordinator.
		 *
		 * key: Pointer to the address of the key.
		 * level: The first address of the counter array in the bucket
		 * 	or in the coordinator.
		 * len: The length of the counter array in the bucket or in
		 * 	the coordinator.
		 * cou_idx: If the key exists in the array, record its index 
		 * 	in the array; otherwise it is -1.
		 * emp_idx: If there is an empty counter in the array, record
		 * 	its index in the array; otherwise it is -1.
		 * lru_idx: If there is a non-empty counter in the array, 
		 * 	record the index of the least recently used element in 
		 * 	the array; otherwise -1.
		 * oper: The comparison function used by the replacement 
		 * 	strategy.
		 */

		cou_idx = -1;
		emp_idx = -1;
		lru_idx = -1;
		
		for(int i = 0; i < len; i++) {
			if (level[i].clo == -1) {
				emp_idx = i;
				continue;
			}
			if (lru_idx == -1 || oper(level[i].clo, level[lru_idx].clo)) {
				lru_idx = i;
			}
			
			bool equal = true;

			for (int j = 0; j < key_len; j++) {
				if (key[j] != level[i].key[j]) {
					equal = false;
				}
			}
			
			if (equal) {
				cou_idx = i;
				break;
			}
		}
	}

	bool set_key(uint8_t *key, int clo, Counter *level, int cou_idx, int emp_idx) {
		/*
		 * If the key is in the bucket to which it is mapped, or if 
		 * 	there is an empty counter in the bucket to which the 
		 * 	key is mapped, the corresponding counter is updated.
		 *
		 * key: Pointer to the address of the key.
		 * clo: Time stamp of the key.
		 * level:  The first address of the counter array in the bucket
		 * 	or in the coordinator.
		 * cou_idx: If the key exists in the array, record its index 
		 * 	in the array; otherwise it is -1.
		 * emp_idx: If there is an empty counter in the array, record
		 * 	its index in the array; otherwise it is -1.
		 *
		 * -> Return whether the counter is updated.
		 */

		if (cou_idx != -1) {
			level[cou_idx].clo = clo;
			return true;
		}
		
		if (emp_idx != -1) {
			for (int j = 0; j < key_len; j++) {
				level[emp_idx].key[j] = key[j];
			}
			level[emp_idx].clo = clo;
			return true;
		}

		return false;
	}

	void set_key(uint8_t *key, int clo, Counter *level, int lru_idx, uint8_t *o_key, int &o_clo, bool (*oper) (int, int)) {
		/*
		 * Updates the counter of the least recently used element in 
		 * 	the array.
		 *
		 * key: Pointer to the address of the key.
		 * clo: Time stamp of the key.
		 * level:  The first address of the counter array in the bucket
		 * 	or in the coordinator.
		 * lru_idx: If there is a non-empty counter in the array, 
		 * 	record the index of the least recently used element in 
		 * 	the array; otherwise -1.
		 * o_key: The key of the replaced element.
		 * o_clo: Time stamp of the replaced element.
		 * oper: The comparison function used by the replacement 
		 * 	strategy.
		 */

		if (oper(level[lru_idx].clo, clo)) {
			for (int j = 0; j < key_len; j++) {
				o_key[j] = level[lru_idx].key[j];
				level[lru_idx].key[j] = key[j];
			}

			o_clo = level[lru_idx].clo;
			level[lru_idx].clo = clo;
		}
		else {
			for (int j = 0; j < key_len; j++) {
				o_key[j] = key[j];
			}
			
			o_clo = clo;
		}
	}

	int insert(uint8_t *key, int clo) {
		/*
		 * Insert the most recently used element into the cache 
		 * 	structure.
		 *
		 * key: Pointer to the address of the key.
		 * clo: Time stamp of the key.
		 *
		 * -> Returns the timestamp of the replaced element if a
		 *  	replacement occurs; otherwise returns -1.
		 */

		int cou_idx, emp_idx, lru_idx;
		
		uint8_t key_1[key_len], key_2[key_len], key_o[key_len];
		int clo_1, clo_2, clo_o;


		/**************************************************************/
		lookup(key, level_2, w2, cou_idx, emp_idx, lru_idx, les);
		
		if (set_key(key, clo, level_2, cou_idx, emp_idx)) {
			return -1;
		}
		/**************************************************************/

		
		/**************************************************************/
		int idx = hash->run((char *)key, key_len) % w1;
		
		lookup(key, level_1[idx], w3, cou_idx, emp_idx, lru_idx, les);

		if (set_key(key, clo, level_1[idx], cou_idx, emp_idx)) {
			return -1;
		}

		set_key(key, clo, level_1[idx], lru_idx, key_1, clo_1, les);
		/**************************************************************/

		
		/**************************************************************/
		lookup(key_1, level_2, w2, cou_idx, emp_idx, lru_idx, les);

		set_key(key_1, clo_1, level_2, lru_idx, key_2, clo_2, les);
		/**************************************************************/


		/**************************************************************/
		int idx_2 = hash->run((char *)key_2, key_len) % w1;

		lookup(key_2, level_1[idx_2], w3, cou_idx, emp_idx, lru_idx, les);
		
		if (set_key(key_2, clo_2, level_1[idx_2], cou_idx, emp_idx)) {
			return -1;
		}

		set_key(key_2, clo_2, level_1[idx_2], lru_idx, key_o, clo_o, les);
		/**************************************************************/

		
		return clo_o;
	}
	
	double query(int clo) {
		/* 
		 * Find the relative ranking of a timestamp in the cache 
		 * 	structure.
		 *
		 * clo: Time stamp of the key.
		 *
		 * -> Returns the relative ranking of a timestamp in the cache
		 * 	structure.
		 */

		int coun = 0, rank = 0;
		
		for (int i = 0; i < w1; i++) {
			for (int j = 0; j < w3; j++) {
				if (level_1[i][j].clo != -1) {
					coun += 1;
					
					if (level_1[i][j].clo >= clo) {
						rank += 1;
					}
				}
			}
		}
		
		for (int i = 0; i < w2; i++) {
			if (level_2[i].clo != -1) {
				coun += 1;
				
				if (level_2[i].clo >= clo) {
					rank += 1;
				}
			}
		}
		
		return (double) rank / coun;
	}
};
