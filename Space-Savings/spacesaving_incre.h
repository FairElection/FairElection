#ifndef _spacesaving_incre_H
#define _spacesaving_incre_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include "BOBHASH32.h"
#include "params.h"
#include "ssummary.h"
#include "BOBHASH64.h"
#define rep(i,a,n) for(int i=a;i<=n;i++)
using namespace std;
class spacesaving_incre
{
    private:
        ssummary *ss;
        int M2;
    public:
        spacesaving_incre(int M2):M2(M2) {ss=new ssummary(M2); ss->clear();}
        void Insert(string x) // insert an item into Space-Saving
        {
            bool mon=false;
            int p=ss->find(x);
            if (p) mon=true;
            if (!mon)  // if it is not in Space-Saving
            {
                int q;
                if (ss->tot<M2) q=1; else
                q=ss->getmin()+1;
                int i=ss->getid();
                ss->add2(ss->location(x),i);
                ss->str[i]=x;
                ss->sum[i]=q;
                ss->link(i,0);
                while(ss->tot>M2)
                {
                    int t=ss->Right[0];
                    int tmp=ss->head[t];
                    ss->cut(ss->head[t]);
                    ss->recycling(tmp);
                }
            } else  // otherwise
            {
                int tmp=ss->Left[ss->sum[p]];
                ss->cut(p);
                if(ss->head[ss->sum[p]]) tmp=ss->sum[p];
                ss->sum[p]++;  // replece the original item and plus 1 of its frequency
                ss->link(p,tmp);
            }
        }
        struct Node {string x; int y;} q[MAX_MEM+10];
        static int cmp(Node i,Node j) {return i.y>j.y;}
        void work() //sort the frequencies of items which are stored in the stream-summary
        {
            int CNT=0;
            for(int i=N;i;i=ss->Left[i])
                for(int j=ss->head[i];j;j=ss->Next[j]) {q[CNT].x=ss->str[j]; q[CNT].y=ss->sum[j]; CNT++; }
            sort(q,q+CNT,cmp);
        }
        pair<string ,int> Query(int k)  // output the k-th largest frequency and the corresponding item
        {
            return make_pair(q[k].x,q[k].y);
        }
};
#endif
