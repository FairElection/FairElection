#ifndef CLOCK_H
#define CLOCK_H

#include <string>
#include <string.h>
class Clock
{
    public:
    // store keys of cache
    //list<string> dq;
    int *clock;
    int hand;
    // store references of key in cache
    unordered_map<string, int> ma;
    int csize; //maximum capacity of cache

    Clock(int);
    int refers(string);
};

Clock::Clock(int n)
{
    csize = n;
    clock=new int[n];
    hand=0;
}

/* Refers key x with in the LRU cache */
int Clock::refers(string x)
{
    int ret=0;
    // not present in cache
    if (ma.find(x) == ma.end())
    {
        ret=-1;
        // cache is full
        if (ma.size() == csize)
        {
            //delete
            while(clock[hand]!=0){clock[hand]=0;hand=(hand+1)%csize;}
            for(auto wtf=ma.begin();wtf!=ma.end();++wtf){
                if(wtf->second == hand){
                    ma.erase(wtf);

                }
            }
        }
        clock[hand]=1;
        ma[x]=hand;
        hand=(hand+1)%csize;
    }

    // present in cache
    else{
        clock[ma[x]]=1;
        ret=0;
    }

    return ret;
}

// display contents of cache
#endif // CLOCK_H
