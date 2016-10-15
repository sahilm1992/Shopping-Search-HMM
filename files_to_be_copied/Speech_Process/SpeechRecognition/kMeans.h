//
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<sstream>
#include<fstream>
#include<math.h>
#include<algorithm>
#include<vector> 
using namespace std;

extern  int codeBookRows;
extern int codeBookColumns;

extern long double source[50000][12];//  from file
extern long double cbook[32][12]; // coddebook
extern int numLines;
extern long double cbooktemp[32][12]; // temp codebook

extern struct bucket{
	long double value[12];
	int count;

};
// bucket stores sum of column i in value i for vectors in the bucket

extern bucket buckets[32];

extern int weight[];
void createCodeBook(char *universeCSV);
