#ifndef LRU_QUEU_H
#define LRU_QUEU_H
#include <string.h>
#include <string>
class LRUCache
{
    public:
    // store keys of cache
    list<string> dq;

    // store references of key in cache
    unordered_map<string, list<string>::iterator> ma;
    int csize; //maximum capacity of cache

    LRUCache(int);
    int refer(string);
    void display();
};

LRUCache::LRUCache(int n)
{
    csize = n;
}

/* Refers key x with in the LRU cache */
int LRUCache::refer(string x)
{
    int ret=0;
    // not present in cache
    if (ma.find(x) == ma.end())
    {
        ret=-1;
        // cache is full
        if (ma.size() == csize)
        {
            //delete least recently used element
            string last = dq.back();
            dq.pop_back();
            ma.erase(last);
        }
    }

    // present in cache
    else{
        dq.erase(ma[x]);
        ret=0;
    }

    // update reference
    dq.push_front(x);
    ma[x] = dq.begin();
    return ret;
}

// display contents of cache
void LRUCache::display()
{
    for (auto it = dq.begin(); it != dq.end();
                                        it++){
        cout << (*it) << " ";
        cout<<*(ma[*(it)])<<",";
    }
    cout << endl;
}

#endif // LRU_QUEU_H
