#ifndef SIMD_H
#define SIMD_H
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <x86intrin.h>
#include <bmiintrin.h>
#include "bobhash32.h"
#include <vector>

class spacesaving
{
    const static int KEYLEN = 4;
public:
    int bucket_num, cell_per_bucket, l2_num;

    uint32_t SS_key[100005][35], SS_val[100005][35];
    uint32_t Heap_key[1005], Heap_val[1005];

    BOBHash32 *bobhash;

    spacesaving(int M, int C, int num):bucket_num(M), cell_per_bucket(C), l2_num(num) //this function initialize the FE structure
    {
        memset(SS_key, 0, sizeof(SS_key));
        memset(SS_val, 0, sizeof(SS_val));
        memset(Heap_key, 0, sizeof(Heap_key));
        memset(Heap_val, 0, sizeof(Heap_val));
        bobhash = new BOBHash32(1000);
    }
    ~spacesaving(){delete bobhash;}

    int insert(int key,int time)//this function insert a (key,time)pair to FE
        {
            uint32_t fp = *(uint32_t*)&key;

        /* try to match l2 */
            const __m256i item = _mm256_set1_epi32((int)fp);

            for(int i = 0; i < l2_num / 8; i++)
            {
                __m256i *keys_p = (__m256i*)(&Heap_key[8 * i]);
                int matched = 0;
                __m256i a_comp = _mm256_cmpeq_epi32(item, keys_p[0]);
                matched = _mm256_movemask_ps((__m256)a_comp);

                if(matched)
                {
                    int matched_index = _tzcnt_u32((uint32_t)matched);
                    Heap_val[8 * i + matched_index]=time;
                    return -1;
                }

            }

            /* try to find an empty bucket in l2 */
                int heap_m_val = 100000000, heap_m_cell = l2_num;

                for(int i = 0; i < l2_num / 8; ++i)
                {
                    const __m256i *counters = (__m256i*)(&Heap_val[8 * i]);
                    __m256 results = *(__m256*)counters;

                    __m128i low_part = _mm_castps_si128(_mm256_extractf128_ps(results, 0));
                    __m128i high_part = _mm_castps_si128(_mm256_extractf128_ps(results, 1));

                    __m128i x = _mm_min_epi32(low_part, high_part);
                    __m128i min1 = _mm_shuffle_epi32(x, _MM_SHUFFLE(0, 0, 3, 2));
                    __m128i min2 = _mm_min_epi32(x, min1);
                    __m128i min3 = _mm_shuffle_epi32(min2, _MM_SHUFFLE(0, 0, 0, 1));
                    __m128i min4 = _mm_min_epi32(min2, min3);

                    int min_counter_val = _mm_cvtsi128_si32(min4);

                    const __m256i ct_item = _mm256_set1_epi32(min_counter_val);
                    int ct_matched = 0;

                    __m256i ct_a_comp = _mm256_cmpeq_epi32(ct_item, (__m256i)results);
                    int matched = _mm256_movemask_ps((__m256)ct_a_comp);
                    int min_counter = _tzcnt_u32((uint32_t)matched);

                    if(min_counter_val == 0)        // find empty cell
                    {
                        Heap_key[8 * i + min_counter] = fp;
                        Heap_val[8 * i + min_counter] = time;
                        return 0;
                    }
                    if(min_counter_val < heap_m_val)
                                {
                                    heap_m_val = min_counter_val;
                                    heap_m_cell = 8 * i + min_counter;
                                }

                }
                int wz = bobhash->run((const char*)&key, KEYLEN) % bucket_num;
                        int Y = cell_per_bucket;
                        for(int i = 0; i < cell_per_bucket / 8; ++i)
                        {
                        /* try to match in bucket */
                            __m256i *keys_p = (__m256i*)(&SS_key[wz][8 * i]);
                            int matched = 0;

                            __m256i a_comp = _mm256_cmpeq_epi32(item, keys_p[0]);
                            matched = _mm256_movemask_ps((__m256)a_comp);

                            if(matched)
                            {
                                int matched_index = _tzcnt_u32((uint32_t)matched);
                                SS_val[wz][8 * i + matched_index]=time;

                                return -1;
                            }
                        }
    int bucket_min_val = 1000000000, bucket_min_cell = cell_per_bucket;
        for(int i = 0; i < cell_per_bucket / 8; ++i)
        {

            const __m256i *counters = (__m256i*)(&SS_val[wz][8 * i]);
            __m256 results = *(__m256*)counters;

            __m128i low_part = _mm_castps_si128(_mm256_extractf128_ps(results, 0));
            __m128i high_part = _mm_castps_si128(_mm256_extractf128_ps(results, 1));

            __m128i x = _mm_min_epi32(low_part, high_part);
            __m128i min1 = _mm_shuffle_epi32(x, _MM_SHUFFLE(0, 0, 3, 2));
            __m128i min2 = _mm_min_epi32(x, min1);
            __m128i min3 = _mm_shuffle_epi32(min2, _MM_SHUFFLE(0, 0, 0, 1));
            __m128i min4 = _mm_min_epi32(min2, min3);

            int min_counter_val = _mm_cvtsi128_si32(min4);

            const __m256i ct_item = _mm256_set1_epi32(min_counter_val);
            int ct_matched = 0;

            __m256i ct_a_comp = _mm256_cmpeq_epi32(ct_item, (__m256i)results);
            int matched = _mm256_movemask_ps((__m256)ct_a_comp);
            int min_counter = _tzcnt_u32((uint32_t)matched);

            if(min_counter_val == 0)        // find empty cell
            {
                SS_key[wz][8 * i + min_counter] = fp;
                SS_val[wz][8 * i + min_counter] = time;
                return 0;
            }
            if(min_counter_val < bucket_min_val)
                            {
                                bucket_min_val = min_counter_val;
                                bucket_min_cell = 8 * i + min_counter;
                            }

        }
        Y = bucket_min_cell;
        int heap_min_val = 1000000000, heap_min_cell = l2_num;
                for(int i = 0; i < l2_num / 8; ++i)
                {
                    const __m256i *counters = (__m256i*)(&Heap_val[8 * i]);
                    __m256 results = *(__m256*)counters;

                    __m128i low_part = _mm_castps_si128(_mm256_extractf128_ps(results, 0));
                    __m128i high_part = _mm_castps_si128(_mm256_extractf128_ps(results, 1));

                    __m128i x = _mm_min_epi32(low_part, high_part);
                    __m128i min1 = _mm_shuffle_epi32(x, _MM_SHUFFLE(0, 0, 3, 2));
                    __m128i min2 = _mm_min_epi32(x, min1);
                    __m128i min3 = _mm_shuffle_epi32(min2, _MM_SHUFFLE(0, 0, 0, 1));
                    __m128i min4 = _mm_min_epi32(min2, min3);

                    int min_counter_val = _mm_cvtsi128_si32(min4);

                    const __m256i ct_item = _mm256_set1_epi32(min_counter_val);
                    int ct_matched = 0;

                    __m256i ct_a_comp = _mm256_cmpeq_epi32(ct_item, (__m256i)results);
                    int matched = _mm256_movemask_ps((__m256)ct_a_comp);
                    int min_counter = _tzcnt_u32((uint32_t)matched);

                    if(min_counter_val < heap_min_val)
                    {
                        heap_min_val = min_counter_val;
                        heap_min_cell = 8 * i + min_counter;
                    }
                }
        if(SS_val[wz][Y]<=heap_min_val){
            SS_val[wz][Y]=time;
            SS_key[wz][Y]=fp;
            return 1;
        }
        else{
            int bucket_pos=bobhash->run((const char*)&Heap_key[heap_min_cell], KEYLEN) % bucket_num;
            int bucket_m_val = 1000000000, bucket_m_cell = cell_per_bucket;
                for(int i = 0; i < cell_per_bucket / 8; ++i)
                {
                    const __m256i *counters = (__m256i*)(&SS_val[wz][8 * i]);
                    __m256 results = *(__m256*)counters;

                    __m128i low_part = _mm_castps_si128(_mm256_extractf128_ps(results, 0));
                    __m128i high_part = _mm_castps_si128(_mm256_extractf128_ps(results, 1));

                    __m128i x = _mm_min_epi32(low_part, high_part);
                    __m128i min1 = _mm_shuffle_epi32(x, _MM_SHUFFLE(0, 0, 3, 2));
                    __m128i min2 = _mm_min_epi32(x, min1);
                    __m128i min3 = _mm_shuffle_epi32(min2, _MM_SHUFFLE(0, 0, 0, 1));
                    __m128i min4 = _mm_min_epi32(min2, min3);

                    int min_counter_val = _mm_cvtsi128_si32(min4);

                    const __m256i ct_item = _mm256_set1_epi32(min_counter_val);
                    int ct_matched = 0;

                    __m256i ct_a_comp = _mm256_cmpeq_epi32(ct_item, (__m256i)results);
                    int matched = _mm256_movemask_ps((__m256)ct_a_comp);
                    int min_counter = _tzcnt_u32((uint32_t)matched);

                    if(min_counter_val == 0)        // find empty cell
                    {
                        SS_key[bucket_pos][8 * i + min_counter] = fp;
                        SS_val[bucket_pos][8 * i + min_counter] = time;
                        bucket_m_val=0;
                        bucket_m_cell=8 * i + min_counter;
                        break;
                    }
                    if(min_counter_val < bucket_m_val)
                                    {
                                        bucket_m_val = min_counter_val;
                                        bucket_m_cell = 8 * i + min_counter;
                                    }

                }
                if(heap_min_val<=bucket_m_val){
                    Heap_key[heap_min_cell]=SS_key[wz][Y];
                    Heap_val[heap_min_cell]=SS_val[wz][Y];
                    SS_key[wz][Y]=fp;
                    SS_val[wz][Y]=time;
                    return 1;
                }
                else{
                    SS_val[bucket_pos][bucket_m_cell]=heap_min_val;
                    SS_key[bucket_pos][bucket_m_cell]=Heap_key[heap_min_cell];
                    Heap_key[heap_min_cell]=SS_key[wz][Y];
                    Heap_val[heap_min_cell]=SS_val[wz][Y];
                    SS_key[wz][Y]=fp;
                    SS_val[wz][Y]=time;
                    return 0;
                }

        }


    }


};

#endif // SIMD_H
