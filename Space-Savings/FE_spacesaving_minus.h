#ifndef _FE_spacesaving_minus_H
#define _FE_spacesaving_minus_H

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
class FE_spacesaving_minus
{
    public:
        int M,C,num;
        struct kv {int ID; int f;} SS[100005][35],Heap[1005];  // Heap denotes L2
        BOBHash32 * bobhash;
        FE_spacesaving_minus(int M,int C,int num):M(M),C(C),num(num) {bobhash = new BOBHash32(1000);} // totally M buckets, C cells for each bucket, num entries in L2, (num=0 means that L2 is empty).
        void Insert(int s)
        {
            int MIN = 1000000000, X;

            char str[105];
            sprintf(str,"%d",s);


            for (int j=0; j<num; j++)
                if (Heap[j].ID==s || Heap[j].f==0) {Heap[j].ID=s; Heap[j].f++; return;}

            for (int j=0; j<num; j++)
                if (Heap[j].f<MIN) MIN=Heap[j].f,X=j;

            int WZ = bobhash->run(str,strlen(str)) % M,Y=C;
            for (int j=0; j<C; j++)
                if (SS[WZ][j].ID==s) {SS[WZ][j].f++; Y=j; break;}

            if (Y<C && SS[WZ][Y].f>MIN)
            {
                int ss=Heap[X].ID; int ff=Heap[X].f;
                sprintf(str,"%d",ss);

                Heap[X].f=SS[WZ][Y].f;
                Heap[X].ID=SS[WZ][Y].ID;
                SS[WZ][Y].f=0;
                SS[WZ][Y].ID=0;

                WZ = bobhash->run(str,strlen(str)) % M;

                MIN = 1000000000; X = 0;
                for (int j=0; j<C; j++)
                    if (SS[WZ][j].f<MIN) MIN=SS[WZ][j].f,X=j;
                SS[WZ][X].ID=ss; SS[WZ][X].f=ff;
            }

            if (Y==C)
            {
                for (int j=0; j<C; j++)
                if (SS[WZ][j].f==0)
                {
                    SS[WZ][j].f=1;
                    SS[WZ][j].ID=s;
                    return;
                }

                MIN = 1000000000, X = 0;
                for (int j=0; j<C; j++)
                    if (SS[WZ][j].f<MIN) MIN=SS[WZ][j].f,X=j;

                SS[WZ][X].f--;
                if (SS[WZ][X].f==0) SS[WZ][X].ID=s,SS[WZ][X].f=1;
            //}
            }
        }
        struct Node {int x; int y;} q[3000000];
        static int cmp(Node i,Node j) {return i.y>j.y;}
        void work()
        {
            int CNT=0;
            for (int i=0; i<num; i++) {q[CNT].x=Heap[i].ID; q[CNT].y=Heap[i].f; CNT++;}
            for (int i=0; i<M; i++)
                for (int j=0; j<C; j++) {q[CNT].x=SS[i][j].ID; q[CNT].y=SS[i][j].f; CNT++;}
            sort(q,q+CNT,cmp);
        }
        pair<int ,int> Query(int k)
        {
            return make_pair(q[k].x,q[k].y);
        }
};
#endif
