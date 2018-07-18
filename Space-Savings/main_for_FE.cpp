#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <map>
#include <fstream>
#include "BOBHASH32.h"
#include "FE_spacesaving_incre.h"
#include "FE_spacesaving_minus.h"
#include "FE_spacesaving_replace.h"
using namespace std;
int sss[2000005];
map <int ,int> B,C,Rank;
struct node {int x; int y;} p[10000005];
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
int cmp(node i,node j) {return i.y>j.y;}
int main()
{
    int m=2000000;  // the number of flows

    // preparing spacesaving
    int K=500;
    int MEM = 30;

    int cc;



    for (int i=1; i<=m; i++) {sss[i]=Read(); B[sss[i]]++;}

    for (MEM=20; MEM<=100; MEM+=20)
    {
    C.clear(); Rank.clear();
	int cnt=0;
    for (map <int,int>::iterator sit=B.begin(); sit!=B.end(); sit++)
    {
        p[++cnt].x=sit->first;
        p[cnt].y=sit->second;
    }
    sort(p+1,p+cnt+1,cmp);
    for (int i=1; i<=K; i++) C[p[i].x]=p[i].y,Rank[p[i].x]=i;


    for (cc=0; 64*cc<=MEM*1024*8; cc++);  // # cells
    cout<<cc<<endl;

    FE_spacesaving_incre *dd; dd=new FE_spacesaving_incre(cc/8,8,0);
    FE_spacesaving_minus *dd1; dd1=new FE_spacesaving_minus(cc/8,8,0);
    FE_spacesaving_replace *dd2; dd2=new FE_spacesaving_replace(cc/8,8,0);
    for (int i=1; i<=m; i++)
	{
		int s=sss[i];
		dd->Insert(s);
		dd1->Insert(s);
		dd2->Insert(s);
	}
	//return 0;
	dd->work();
	dd1->work();
	dd2->work();

	// preparing true flow

    // Calculating PRE, ARE, AAE
    cout<<"Calculating"<<endl;


    int dd_sum=0,dd_AAE=0; double dd_ARE=0;
    int dd_string; int dd_num; double MAP=0;
    for (int i=0; i<K; i++)
    {
        dd_string=(dd->Query(i)).first; dd_num=(dd->Query(i)).second;
        dd_AAE+=abs(B[dd_string]-dd_num); dd_ARE+=abs(B[dd_string]-dd_num)/(B[dd_string]+0.0);
        if (C[dd_string]) dd_sum++;
        int summ=0;
        for (int j=0; j<=i; j++)
            if (Rank[dd->Query(j).first]>0 && Rank[dd->Query(j).first]<=i+1) summ++;
        MAP+=(double)summ/(i+1);
    }
    MAP/=K;

    int dd_sum1=0,dd_AAE1=0; double dd_ARE1=0;
    int dd_string1; int dd_num1; double MAP1=0;
    for (int i=0; i<K; i++)
    {
        dd_string1=(dd1->Query(i)).first; dd_num1=(dd1->Query(i)).second;
        dd_AAE1+=abs(B[dd_string1]-dd_num1); dd_ARE1+=abs(B[dd_string1]-dd_num1)/(B[dd_string1]+0.0);
        if (C[dd_string1]) dd_sum1++;
        int summ=0;
        for (int j=0; j<=i; j++)
            if (Rank[dd1->Query(j).first]>0 && Rank[dd1->Query(j).first]<=i+1) summ++;
        MAP1+=(double)summ/(i+1);
    }
    MAP1/=K;


    int dd_sum2=0,dd_AAE2=0; double dd_ARE2=0;
    int dd_string2; int dd_num2; double MAP2=0;
    for (int i=0; i<K; i++)
    {
        dd_string2=(dd2->Query(i)).first; dd_num2=(dd2->Query(i)).second;
        dd_AAE2+=abs(B[dd_string2]-dd_num2); dd_ARE2+=abs(B[dd_string2]-dd_num2)/(B[dd_string2]+0.0);
        if (C[dd_string2]) dd_sum2++;
        int summ=0;
        for (int j=0; j<=i; j++)
            if (Rank[dd2->Query(j).first]>0 && Rank[dd2->Query(j).first]<=i+1) summ++;
        MAP2+=(double)summ/(i+1);
    }
    MAP2/=K;

    printf("%.5f %.5f %.10f %.10f\n",dd_sum/(K+0.0),MAP,dd_ARE/K,dd_AAE/(K+0.0));
    printf("%.5f %.5f %.10f %.10f\n",dd_sum1/(K+0.0),MAP1,dd_ARE1/K,dd_AAE1/(K+0.0));
    printf("%.5f %.5f %.10f %.10f\n\n",dd_sum2/(K+0.0),MAP2,dd_ARE2/K,dd_AAE2/(K+0.0));
    }
    return 0;
}
