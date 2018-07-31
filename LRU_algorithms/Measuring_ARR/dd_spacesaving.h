#ifndef DD_SPACESAVING_H
#define DD_SPACESAVING_H

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
using namespace std;
class dd_spacesaving
{
    public:
        int M,C,num;
        struct kv {string ID; int f;} SS[10000005][40],Heap[10000005];  // Heap denotes L2, here f is the time, the larger the later
        BOBHash32 * bobhash;
        dd_spacesaving(int M,int C,int num):M(M),C(C),num(num) {bobhash = new BOBHash32(1000);} // totally M buckets, C cells for each bucket, num entries in L2, (num=0 means that L2 is empty).
        int Insert(string s,int time) //this function insert the (id,timestamp) pair into our FE data structure
        {
            for (int j=0; j<num; j++){
                if(Heap[j].ID==s){Heap[j].ID=s; Heap[j].f=time; return -1;}
                else if(Heap[j].f==0){Heap[j].ID=s; Heap[j].f=time; return 0;}
            }

            int WZ = bobhash->run(s.c_str(),s.size()) % M,Y=C;
            for (int j=0; j<C; j++)
                if (SS[WZ][j].ID==s) {SS[WZ][j].f=time; Y=j; return -1;}
            //next we deal with the case where s is no where to be found in both layers
            if (Y==C)
            {
                for (int j=0; j<C; j++)
                if (SS[WZ][j].f==0)
                {
                    SS[WZ][j].f=time;
                    SS[WZ][j].ID=s;
                    return 0; //corresponding bucket still has available cell
                }

                int MIN = 1000000000, Y = 0;
                for (int j=0; j<C; j++)
                    if (SS[WZ][j].f<MIN) MIN=SS[WZ][j].f,Y=j;
                //now SS[WZ][Y] is the coldest item in the bucket
                //first find the coldest item in the heap:
                int mini=1000000000;
                int pos=0;
                for(int jaziel=0;jaziel<num;++jaziel){
                    if(Heap[jaziel].f<mini){
                        mini=Heap[jaziel].f;
                        pos=jaziel;
                    }
                }
                if(SS[WZ][Y].f<=mini){
                    int temp=SS[WZ][Y].f;
                    SS[WZ][Y].f=time;
                    SS[WZ][Y].ID=s;
                    return temp;
                }
                else{//SS[WZ][Y]>mini
                    //find the correspending bucket of item Heap[mini]:
                    int bucket_pos=bobhash->run((Heap[mini].ID).c_str(),s.size()) % M;
                    //if it is full, then get the coldest item in that bucket
                    int mi=100000000;
                    int position=0;
                    for(int kat=0;kat<C;++kat){
                        if(SS[bucket_pos][kat].f==0){
                            position=kat;
                            mi=100000001;
                            break;
                        }
                        else if(SS[bucket_pos][kat].f<mi){
                            mi=SS[bucket_pos][kat].f;
                            position=kat;
                        }
                    }
                    if(mi==100000001){
                        SS[bucket_pos][position].f=Heap[pos].f;
                        SS[bucket_pos][position].ID=Heap[pos].ID;
                        Heap[pos].f=SS[WZ][Y].f;
                        Heap[pos].ID=SS[WZ][Y].ID;
                        SS[WZ][Y].f=time;
                        SS[WZ][Y].ID=s;
                        return 0;
                    }
                    //compare the two
                    else if(Heap[pos].f<=mi){
                        int temp=Heap[pos].f;
                        Heap[pos].f=SS[WZ][Y].f;
                        Heap[pos].ID=SS[WZ][Y].ID;
                        SS[WZ][Y].f=time;
                        SS[WZ][Y].ID=s;
                        return temp;
                    }
                    else{

                        int temp=SS[bucket_pos][position].f;
                        SS[bucket_pos][position].f=Heap[pos].f;
                        SS[bucket_pos][position].ID=Heap[pos].ID;
                        Heap[pos].f=SS[WZ][Y].f;
                        Heap[pos].ID=SS[WZ][Y].ID;
                        SS[WZ][Y].f=time;
                        SS[WZ][Y].ID=s;
                        return temp;
                    }
                }
            }
        }

        int SIMD_Insert(string s,int time) //this function uses SIMD speedup to insert
        {
            uint32_t fp=*(uint32_t*)s.c_str();
            const __m256i item = _mm256_set1_epi32((int)fp);
            const __m256i item2 = _mm256_set1_epi32(0);


            for (int j=0; j<num/8; j++){
                __m256i *keys_p = (__m256i*)(&Heap[8 * j].ID);
                int matched=0;
                __m256i a_comp = _mm256_cmpeq_epi32(item, keys_p[0]);
                matched = _mm256_movemask_ps((__m256)a_comp);

                __m256i *keys_p2 = (__m256i*)(&Heap[8 * j].f);
                int mat=0;
                __m256i a_comp2 = _mm256_cmpeq_epi32(item2, keys_p2[0]);
                mat = _mm256_movemask_ps((__m256)a_comp2);


                if(matched)
                            {
                                int matched_index = _tzcnt_u32((uint32_t)matched);
                                Heap[8 * j + matched_index].f=time;
                                return -1;
                            }
                else if(mat){
                    int matched_index2 = _tzcnt_u32((uint32_t)mat);
                    Heap[8*j+matched_index2].ID=s;
                    Heap[8*j+matched_index2].f=time;
                    return 0;
                }

            }


            int WZ = bobhash->run(s.c_str(),s.size()) % M,Y=C;
            for(int j=0;j<C/8;++j){
                __m256i *keys_p = (__m256i*)(&SS[WZ][8 * j].ID);
                            int matched = 0;

                            __m256i a_comp = _mm256_cmpeq_epi32(item, keys_p[0]);
                            matched = _mm256_movemask_ps((__m256)a_comp);
                            if(matched){
                                int matched_index = _tzcnt_u32((uint32_t)matched);
                                SS[WZ][matched_index].f=time;
                                Y=8*j+matched_index;
                                return -1;
                            }

            }

            //next we deal with the case where s is no where to be found in both layers
            if (Y==C)
            {
                for (int j=0; j<C; j++)
                if (SS[WZ][j].f==0)
                {
                    SS[WZ][j].f=time;
                    SS[WZ][j].ID=s;
                    return 0; //corresponding bucket still has available cell
                }

                int MIN = 1000000000, Y = 0;
                for (int j=0; j<C; j++)
                    if (SS[WZ][j].f<MIN) MIN=SS[WZ][j].f,Y=j;
                //now SS[WZ][Y] is the coldest item in the bucket
                //first find the coldest item in the heap:
                int mini=1000000000;
                int pos=0;
                for(int jaziel=0;jaziel<num;++jaziel){
                    if(Heap[jaziel].f<mini){
                        mini=Heap[jaziel].f;
                        pos=jaziel;
                    }
                }

                if(SS[WZ][Y].f<=mini){
                    int temp=SS[WZ][Y].f;
                    SS[WZ][Y].f=time;
                    SS[WZ][Y].ID=s;
                    return temp;
                }
                else{//SS[WZ][Y]>mini
                    //find the correspending bucket of item Heap[mini]:
                    int bucket_pos=bobhash->run((Heap[mini].ID).c_str(),s.size()) % M;
                    //if it is full, then get the coldest item in that bucket
                    int mi=100000000;
                    int position=0;
                    for(int kat=0;kat<C;++kat){
                        if(SS[bucket_pos][kat].f==0){
                            position=kat;
                            mi=100000001;
                            break;
                        }
                        else if(SS[bucket_pos][kat].f<mi){
                            mi=SS[bucket_pos][kat].f;
                            position=kat;
                        }
                    }
                    if(mi==100000001){
                        SS[bucket_pos][position].f=Heap[pos].f;
                        SS[bucket_pos][position].ID=Heap[pos].ID;
                        Heap[pos].f=SS[WZ][Y].f;
                        Heap[pos].ID=SS[WZ][Y].ID;
                        SS[WZ][Y].f=time;
                        SS[WZ][Y].ID=s;
                        return 0;
                    }
                    //compare the two
                    else if(Heap[pos].f<=mi){

                        int temp=Heap[pos].f;
                        Heap[pos].f=SS[WZ][Y].f;
                        Heap[pos].ID=SS[WZ][Y].ID;
                        SS[WZ][Y].f=time;
                        SS[WZ][Y].ID=s;
                        return temp;
                    }
                    else{
                        int temp=SS[bucket_pos][position].f;
                        SS[bucket_pos][position].f=Heap[pos].f;
                        SS[bucket_pos][position].ID=Heap[pos].ID;
                        Heap[pos].f=SS[WZ][Y].f;
                        Heap[pos].ID=SS[WZ][Y].ID;
                        SS[WZ][Y].f=time;
                        SS[WZ][Y].ID=s;
                        return temp;
                    }
                }
            }
        }

//this function print out the stored item in the cache
        vector<int> print(){
            vector<int>ans;
            for(int jaziel=0;jaziel<num;++jaziel)
                if(Heap[jaziel].f!=0)ans.push_back(Heap[jaziel].f);
            for(int j=0;j<M;++j){
                for(int kat=0;kat<C;++kat){
                    if(SS[j][kat].f!=0)ans.push_back(SS[j][kat].f);
                }
            }
            return ans;

        }


};

#endif // DD_SPACESAVING_H
