/*************************************************************************
	> File Name: test.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 27 Mar 2015 07:02:17 AM PDT
 ************************************************************************/

#include<iostream>
#include<stdlib.h>

using namespace std;


int main(int argc, char** argv)
{
    char *p = NULL;
    char *q = NULL;
    p=(char*)malloc(10*sizeof(char*));
    q=p;
    p=(char*)realloc(q,20*sizeof(char*));

    cout<<p<<"\t"<<q<<endl;

    if (q == p) {
        cout<<"xxx"<<endl;
    }

    return 1;
}
