// 154101019_kMe

//LBG
// 154101019_LBG.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
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
#include "hmm.h"
using namespace std;


int maxCodeBookRows;
int codeBookColumns;
long double eps = 0.03;
int curCodeBookSize = 1;
long double source[150000][12];//  from file
long double diff[150000];
long double CODEBOOK[32][12]; // coddebook

int numLines = 0;

struct bucket{
	long double value[12];
	int count;

};
// bucket stores sum of column i in value i for vectors in the bucket

bucket buckets[32];

int weight[] = { 1, 3, 7, 13, 19, 22, 25, 33, 42, 50, 56, 61 };

//calculates min distance vector corresponding to source[vector]
void calculateMinimumDistanceVector(int vector)
{

	long double min = FLT_MAX;
	int multiplier = 0;
	long long int minIndex = -1;
	for (int i = 0; i <curCodeBookSize; i++)
	{
		long double  dLoc = 0;

		for (int j = 0; j < codeBookColumns; j++)
		{

			dLoc += weight[j] * ((source[vector][j] - CODEBOOK[i][j])*(source[vector][j] - CODEBOOK[i][j]));
		}

		if (dLoc < min)
		{

			diff[vector] = dLoc;
			min = dLoc; // make it minimum
			minIndex = i;

		}

	}
	buckets[minIndex].count++;
	for (int j = 0; j < codeBookColumns; j++) // store sum for vectors in same bucket
		buckets[minIndex].value[j] += source[vector][j];

}


// Parse the Configuration File to define Constants
int  parseConfigFile()
{
	ifstream iFile;
	iFile.open("Configuration_kmeans.txt");
	if (!iFile)
	{
		perror("Could not open Config file.. Press any key to exit");
		int k;
		cin >> k;
		exit(0);
	}

	string line;
	while ((getline(iFile, line)))
	{
		//line[line.length()] = '\0';    //Remove Delimiter

		int semiColon = line.find(":");
		string str;
		str = line.substr(semiColon + 1);
		stringstream stream(str);

		if (line.find("codeBookColumns") != string::npos)
		{
			stream >> codeBookColumns;
		}

		if (line.find("codeBookRows") != string::npos)
		{
			stream >> maxCodeBookRows;
		}

	}
	iFile.close();
	return 1;
}


//create source matrix from file
void getFromFile(char *fileName)
{
	ifstream iFile;
	int count = 0;
	iFile.open(fileName);
	if (!iFile)
	{
		perror("Could not open Input File , Press any key to exit");
		int x;
		cin >> x;
		exit(0);
	}
	string line1;
	numLines = 0;
	int col = 0;
	char*split = NULL;
	while ((getline(iFile, line1)))
	{
		char line[600];
		strncpy_s(line, line1.c_str(), line1.length());
		split = strtok(line, ",");
		col = 0;
		while (split)
		{

			stringstream str(split);
			long double val;
			str >> val;
			source[numLines][col] = val;
			split = strtok(NULL, ","); // split by comma
			col++;
		}

		numLines++;

	}
}

// create a random codebook using random vectors from source
void createRandomCodeBook()
{
	srand(time(NULL));

	int i = 0;
	while (i <curCodeBookSize) // any 32 vectors
	{
		int random = rand() % numLines;
		random = random / 3;
		for (int k = 0; k < codeBookColumns; k++)
		{
			CODEBOOK[i][k] = source[random][k];

		}
		i++;
	}
}



// returns average of a particular column of code book  like C1 for a particular bucket
long double getAvg(int index, int col)
{

	if (buckets[index].count == 0)
		return 0;
	else

		return (long double)(buckets[index].value[col]) / (long double)(buckets[index].count); // divide by number of vectors in the bucket
}

// THIS CODE DOUBLES THE SIZE OF CODE BOOK USING EPSILON VALUE =0.3
void expandCodeBook()
{

	long double tempCB[32][12];
	int newi = 0;

	for (int i = 0; i < curCodeBookSize; i++)
	{
		for (int j = 0; j < codeBookColumns; j++)
		{

			tempCB[newi][j] = CODEBOOK[i][j] * ((long double)(1.0 + eps));
			tempCB[newi + 1][j] = CODEBOOK[i][j] * ((long double)(1.0 - eps));// 0.140 0.528  0.30
		}
		newi += 2;//  INCREASE SIZE

	}
	curCodeBookSize = newi;
	memcpy(CODEBOOK, tempCB, curCodeBookSize*(codeBookColumns*sizeof(tempCB[0][0])));
	// COPY TEMP CODE BOOK(tempCB) TO ORIGINAL CODEBOOK  (CODEBOOK)
}


//call this to before preparing codeboo
void initCodeBook_Preparation_Process()
{

	memset(source, 0, sizeof(source));
	int x = sizeof(source);

	memset(CODEBOOK, 0, sizeof(CODEBOOK));

	memset(buckets, 0, sizeof(buckets));

	extern long double source[150000][12];//  from file
	extern long double CODEBOOK[32][12]; // coddebook
	

}



//call this to prepare codebook . universeCSV is ouput CSV path
void createCodeBook(char *universeBase)
{
	//cout << " \n *************LBG ****************\n";
	char univFile[200];
	sprintf(univFile, "%s%s", universeBase, "universe.csv");

	char logFile[200];
	sprintf(logFile, "%s%s", universeBase, "log.txt");

	FILE *fp2 = fopen(logFile, "w");		// destroy log file if exists
	char univGraph[200];
	sprintf(univGraph,  "%s%s", universeBase, "Graph.csv");

	fp2 = fopen(univGraph, "w");//destroy

	parseConfigFile();		// assign constants
	getFromFile(univFile);// create source matrix using file

	createRandomCodeBook();

	int printMe = 0;
	// DO TILL YOU REACH CODEBOOK SIZE 32
	while (curCodeBookSize <= maxCodeBookRows)
	{

		long double distortion = 100;
		printMe = 1;
		int iter = 0;

		long double diffOLD = -2;			// stores AVG Diff values with respect to Older code book

		do{

			for (int index = 0; index < numLines; index++)
			{
				calculateMinimumDistanceVector(index); // create buckets 
			}

			long double sum = 0;
			sum = 0;

			for (int i = 0; i < numLines; i++)
			{
				sum = sum + diff[i];					// calculate DIFF Values
			}

			long double diffNEW = -2;// stores AVG Diff values with respect to NEW code book


			if (diffOLD <= -1)
				diffOLD = sum / numLines;// THIS IS DONE ON FIRST ITERATION OF K-MEANS ON A PARTICULAR SIZE OF CODEBOOK
			else
				diffNEW = sum / numLines;

			memset(diff, 0, numLines * sizeof(diff[0]));

			//update codebook - AVERAGING ALL BUCKETS ASSOCIATED TO IT
			for (int b = 0; b < curCodeBookSize; b++)
			{
				for (int col = 0; col < codeBookColumns; col++)
				{
					long double avg = getAvg(b, col);		// find average value of columns in a bucket
					CODEBOOK[b][col] = avg;					// assign average to codebook 
				}
			}


			if (curCodeBookSize>1)				// proceed only if >first iteration
			{
				// THIS BLOCKS CONTAINS CODE TO WRITE TO FILE AND PRINT TO SCREEN

				long double distToPrint = (diffNEW >= -1) ? diffNEW : diffOLD;
				char univLog[200];
				sprintf(univLog,"%s%s", universeBase, "log.txt");

				FILE *fp = fopen(univLog, "a");
				if (!fp)
					cout << "UNABLE TO OPEN FILE FOR WRITING";

				else
				{
					fprintf(fp, "\n");
					if (printMe == 1)
					{
						fprintf(fp, "\n\nCode book SIze %d    ", curCodeBookSize);
						printMe = 0;
					}

					if (diffNEW >= -1)
						fprintf(fp, "   \n\tDistortion: %Lf  ", distToPrint);
					fprintf(fp, "\n\n\tIteration Number %d    ", iter + 1);

					fprintf(fp, " \n");

					for (int b = 0; b < curCodeBookSize; b++)
					{
						fprintf(fp, " \tBucket [%d] :: %d  ", b, buckets[b].count);
					}
					fprintf(fp, " \n");
					for (int i = 0; i < curCodeBookSize; i++)
					{
						fprintf(fp, " \n\t ");
						for (int j = 0; j < 12; j++)
							fprintf(fp, " %Lf ", CODEBOOK[i][j]);
					}
					fclose(fp);

				}
				if (diffNEW >= -1)
				{
				//	printf("\n  Iteration  [%d]  Distortion  %Lf ", iter, distToPrint);
				}
				iter++; // counter for iterator

				// write distortion value to file ( append it )
				fp = fopen(univGraph, "a");
				if (!fp)
					cout << "UNABLE TO OPEN FILE FOR WRITING";

				else
				{
					char line[60];
					{
						if (diffNEW >= -1)
						{
							sprintf(line, "\n%Lf", distToPrint);
							fprintf(fp, line);
							fclose(fp);
						}
					}

				}
			}




			if (diffNEW >= -1) // if we have calculated atleast 2 iterations in this particular size of Codebook
			{
				distortion = abs(diffNEW - diffOLD);  // this will be used in calculating whether to continue performing k Means or not 
				diffOLD = diffNEW;
			}


			// RESET BUCKETS to 0
			for (int b = 0; b < curCodeBookSize; b++)
			{
				buckets[b].count = 0;
				for (int col = 0; col < codeBookColumns; col++)
				{
					buckets[b].value[col] = 0;						// Initialize buckets again to 0 
				}

			}

			if (curCodeBookSize == 1) // only 1 iteration required for Codebook of size 1 . no need to calculate distortion as all values will be mapped to same vector in codebook
				break;

		} while (distortion >= 0.001);					// run till distortion is >=0.001

		if (curCodeBookSize < maxCodeBookRows)
			expandCodeBook();  // expand the codeBook ( double its size)
		else
			break;
	//	cout << "\n CODEBOOK SIZE : " << curCodeBookSize;


	}

//	cout << "\n \n Code book ,  log file , distortion file are stored in output folder :\n\n  Press any key to exit  ";

	// write codebook to file
	char codebooktxt[200];
	sprintf(codebooktxt, "%s%s", universeBase, "codebook.txt");

	FILE *fp = fopen(codebooktxt, "w");
	if (!fp)
		cout << "UNABLE TO OPEN FILE FOR WRITING";

	else
	{
		//fprintf(fp, " \n\n ");
		// PRINT CODEBOOK TO FILE

		for (int i = 0; i < curCodeBookSize; i++)
		{
			for (int j = 0; j < codeBookColumns; j++)
			{
				char line[60];
				sprintf(line, "%lf", CODEBOOK[i][j]);
		
				fprintf(fp, line);
				if (j<codeBookColumns - 1)
					fprintf(fp, ",");

			}
			if (i<curCodeBookSize-1)
				fprintf(fp, "\n");
	
		}


		fclose(fp);
	}
//	getchar();

	//getchar();
}


