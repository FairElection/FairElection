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
#include "params.h"
#include "ssummary.h"
#include "spacesaving_incre.h"
#include "spacesaving_minus.h"
#include "spacesaving_replace.h"
using namespace std;
string sss[2000005];
map <string ,int> B,C,Rank;
struct node {string x;int y;} p[10000005];
ifstream fin("0.dat",ios::in|ios::binary);
char a[105];
string Read()
{
    fin.read(a,13);
    a[4]='\0';
    string tmp=a;
    return tmp;
}
int cmp(node i,node j) {return i.y>j.y;}
int main()
{
    int m=2000000;  // the number of flows

    // prepare for the spacesaving
    int K=500;
    int MEM=30;
    int cc;

    for (int i=1; i<=m; i++) {sss[i]=Read(); B[sss[i]]++;}

    for (MEM=20; MEM<=80; MEM+=20)
    {
    C.clear(); Rank.clear();
	int cnt=0;
    for (map <string,int>::iterator sit=B.begin(); sit!=B.end(); sit++)
    {
        p[++cnt].x=sit->first;
        p[cnt].y=sit->second;
    }
    sort(p+1,p+cnt+1,cmp);
    for (int i=1; i<=K; i++) {C[p[i].x]=p[i].y; Rank[p[i].x]=i;}
    spacesaving_incre *ss; spacesaving_minus *ss1;spacesaving_replace *ss2;

    for (cc=0; 432*cc<=MEM*1024*8; cc++);  // # cells
    cc=max(cc,K);
    cout<<cc<<endl;
    ss=new spacesaving_incre(cc);
     ss1=new spacesaving_minus(cc);
     ss2=new spacesaving_replace(cc);
    for (int i=1; i<=m; i++)
	{
		string s=sss[i];
		ss->Insert(s);
		ss1->Insert(s);
		ss2->Insert(s);
	}
	ss->work();
	ss1->work();
	ss2->work();

	// preparing for the true flows

    // Calculating PRE, ARE, AAE
    cout<<"Calculating"<<endl;


    int ss_sum=0,ss_AAE=0; double ss_ARE=0;
    string ss_string; int ss_num;
    double MAP=0;
    for (int i=0; i<K; i++)
    {
        ss_string=(ss->Query(i)).first; ss_num=(ss->Query(i)).second;
        ss_AAE+=abs(B[ss_string]-ss_num); ss_ARE+=abs(B[ss_string]-ss_num)/(B[ss_string]+0.0);
        if (C[ss_string]) ss_sum++;
        int summ=0;
        for (int j=0; j<=i; j++)
            if (Rank[ss->Query(j).first] && Rank[ss->Query(j).first]<=i+1) summ++;
        MAP+=(double)summ/(i+1);
    }
    MAP/=K;

    int ss_sum1=0,ss_AAE1=0; double ss_ARE1=0;
    string ss_string1; int ss_num1;
    double MAP1=0;
    for (int i=0; i<K; i++)
    {
        ss_string1=(ss1->Query(i)).first; ss_num1=(ss1->Query(i)).second;
        ss_AAE1+=abs(B[ss_string1]-ss_num1); ss_ARE1+=abs(B[ss_string1]-ss_num1)/(B[ss_string1]+0.0);
        if (C[ss_string1]) ss_sum1++;
        int summ=0;
        for (int j=0; j<=i; j++)
            if (Rank[ss1->Query(j).first] && Rank[ss1->Query(j).first]<=i+1) summ++;
        MAP1+=(double)summ/(i+1);
    }
    MAP1/=K;




    int ss_sum2=0,ss_AAE2=0; double ss_ARE2=0;
    string ss_string2; int ss_num2;
    double MAP2=0;
    for (int i=0; i<K; i++)
    {
        ss_string2=(ss2->Query(i)).first; ss_num2=(ss2->Query(i)).second;
        ss_AAE2+=abs(B[ss_string2]-ss_num2); ss_ARE2+=abs(B[ss_string2]-ss_num2)/(B[ss_string2]+0.0);
        if (C[ss_string2]) ss_sum2++;
        int summ=0;
        for (int j=0; j<=i; j++)
            if (Rank[ss2->Query(j).first] && Rank[ss2->Query(j).first]<=i+1) summ++;
        MAP2+=(double)summ/(i+1);
    }
    MAP2/=K;

    printf("%.5f %.5f %.10f %.10f\n%.5f %.5f %.10f %.10f\n%.5f %.5f %.10f %.10f\n\n",ss_sum/(K+0.0),MAP,ss_ARE/K,ss_AAE/(K+0.0),ss_sum1/(K+0.0),MAP1,ss_ARE1/K,ss_AAE1/(K+0.0),ss_sum2/(K+0.0),MAP2,ss_ARE2/K,ss_AAE2/(K+0.0));

    delete ss; delete ss1; delete ss2;
    }
    return 0;
}
