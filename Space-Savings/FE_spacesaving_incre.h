#ifndef _FE_spacesaving_incre_H
#define _FE_spacesaving_incre_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include "BOBHASH32.h"
#include "BOBHASH64.h"
using namespace std;
class FE_spacesaving_incre
{
    public:
        int M,C,num;
        struct kv {int ID; int f; int tag;} SS[100005][35],Heap[1005];  // Heap denotes coordinator
        BOBHash32 * bobhash;
        FE_spacesaving_incre(int M,int C,int num):M(M),C(C),num(num) {bobhash = new BOBHash32(1000);} // totally M buckets, C cells for each bucket, num entries in coordinator, (num=0 means that coordinator is empty).
        void Insert(int s,int tt=0)
        {
            int MIN = 1000000000, X=0;

            char str[105];
            sprintf(str,"%d",s);

            for (int j=0; j<num; j++)
                if (Heap[j].ID==s || Heap[j].f==0) {Heap[j].ID=s; Heap[j].f++; if (tt==1) Heap[j].tag=1; else Heap[j].tag=0; return;}

            for (int j=0; j<num; j++)
                if (Heap[j].f<MIN) MIN=Heap[j].f,X=j;

            int MINN=MIN;
            int WZ = bobhash->run(str,strlen(str)) % M,Y=C;
            if (tt) WZ = M;
            for (int j=0; j<C; j++)
                if (SS[WZ][j].ID==s) {SS[WZ][j].f++; Y=j; break;}
            if (Y==C)
            {
                for (int j=0; j<C; j++)
                if (SS[WZ][j].f==0)
                {
                    SS[WZ][j].f=1;
                    SS[WZ][j].ID=s;
                    return;
                }

                MIN = 1000000000, Y = 0;
                for (int j=0; j<C; j++)
                    if (SS[WZ][j].f<MIN) MIN=SS[WZ][j].f,Y=j;

                SS[WZ][Y].f++; SS[WZ][Y].ID=s;  // plus 1

            }
            if (SS[WZ][Y].f>MINN)
            {
                int ss=Heap[X].ID; int ff=Heap[X].f; int Tag=Heap[X].tag;
                sprintf(str,"%d",ss);

                Heap[X].f=SS[WZ][Y].f;
                Heap[X].ID=SS[WZ][Y].ID;
                if (WZ==M) Heap[X].tag=1; else Heap[X].tag=0;
                SS[WZ][Y].f=0;
                SS[WZ][Y].ID=0;

                WZ = bobhash->run(str,strlen(str)) % M;
                if (Tag==1) WZ=M;

                MIN = 1000000000; X = 0;
                for (int j=0; j<C; j++)
                    if (SS[WZ][j].f<MIN) MIN=SS[WZ][j].f,X=j;
                SS[WZ][X].ID=ss; SS[WZ][X].f=ff;
            }
        }
        struct Node {int x; int y;} q[3000000];
        static int cmp(Node i,Node j) {return i.y>j.y;}
        void work(int pl=0) //sort the frequencies of items which are stored in the stream-summary
        {
            int CNT=0;
            for (int i=0; i<num; i++) {q[CNT].x=Heap[i].ID; q[CNT].y=Heap[i].f; CNT++;}
            for (int i=0; i<M+pl; i++)
                for (int j=0; j<C; j++) {q[CNT].x=SS[i][j].ID; q[CNT].y=SS[i][j].f; CNT++;}
            sort(q,q+CNT,cmp);
        }
        pair<int ,int> Query(int k)  // output the k-th largest frequency and the corresponding item
        {
            return make_pair(q[k].x,q[k].y);
        }
};
#endif
