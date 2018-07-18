#include <iostream>
#include <list>
#include <unordered_map>
#include "dd_spacesaving.h"
#include "lru_queu.h"
#include "clock.h"
#include <fstream>
#include <vector>
#include <ctime>
using namespace std;

char insert[99000000][16];
char four[99000000][5];


int sss[2000005];
ifstream fin("0.dat",ios::in|ios::binary);
char a[105];
int Read()
{
    fin.read(a,13);
    a[4]='\0';
    int tmp=0;
    for (int i=0; i<4; i++) tmp=tmp*256+(int)a[i];
    return tmp;
}

int main()
{
        int package_num=0;
    while(package_num<10001){sss[package_num]=Read();++package_num;}

    package_num=0;
    unordered_map<string, int> ump;
    FILE *file_stream=fopen("0.dat","rb");
    while(fread(insert[package_num],1,13,file_stream) && package_num< 1000001){
        ump[string(insert[package_num])]=package_num%100+1;

        for(int i=0;i<4;++i){
            four[package_num][i]=insert[package_num][i];

        }
        ++package_num;
    }


    int mem=100; //in MB
    int cell_num=32;
    int second_layer=0;
    int j=mem*4-1;
    dd_spacesaving *dd=new dd_spacesaving(j,cell_num,second_layer);
    LRUCache *lru = new LRUCache(mem*1024/42);
    Clock *cloc= new Clock(mem*1024/34);
    timespec time1,time2;
    long long  resns;
    int times=0;
    int tim=0;
    int ti=0;
    clock_gettime(CLOCK_MONOTONIC,&time1);
    for(int i=0;i<package_num;++i){

        int del=dd->Insert(string(four[i]),i);
        int del2=lru->refer(string(four[i]));
        int del3=cloc->refers(string(four[i]));
        if(del!=-1)++times;

        if(del2==-1)++tim;
        if(del3==-1)++ti;
    }
    clock_gettime(CLOCK_MONOTONIC,&time2);
    //here are the Miss Rate results:
    cout<<mem<<","<<(double)times/package_num<<","<<(double)tim/package_num<<","<<(double)ti/package_num<<endl;

    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    double th_ls_4 = (double)1000.0 * package_num / resns;
    cout<<"FE:"<<th_ls_4<<endl;
    delete dd;
    return 0;
}
