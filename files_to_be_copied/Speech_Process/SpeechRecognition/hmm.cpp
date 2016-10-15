
// 154101019_HMM.cpp : Defines the entry point for the console application.


#include "stdafx.h"
#include "hmmh.h"
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

#define ObsLen 600
#define hmmN 5     
using namespace std;

long double alpha[ObsLen][hmmN + 1];
long double pi[hmmN];
long double piBar[hmmN];


//for fileNames


char piFile[50];
char aFile[50];
char bFile[50];
char obFile[50];

//max number of iterations
int iterationNumber = 300;
int iterationCounter = 0;


//5 * 32
long double Bhmm[hmmN][300];
long double Bhmmbar[hmmN][300];
int Obs[ObsLen];

//A and Ahmm Bar matrix
long double Ahmm[hmmN][hmmN];
long double Ahmmbar[hmmN][hmmN];

//dimension for Ahmm matrix
int AhmmRows = 5;
int AhmmCols = 5;

//dimension for B matrix
int BhmmRows = 5;
int BhmmCols = 32;

int alphaCount = 0;// Alpha matrix rows
int numberOfObservations = 0;// from observation file


long double Gamma[ObsLen][hmmN];
long double Beta[ObsLen][hmmN];
long double delta[ObsLen][hmmN];

int psi[ObsLen][hmmN];

long double pStar = 0;
int qTStar = 0;
int qTStarArr[ObsLen];
long double zhi[ObsLen][hmmN][hmmN];

long double Probterm = 0;// probability O given lambda

long double codebook[32][13];//32X12


// Parse the Configuration File to define Constants
int  parse_HMMConfigFile()
{
	ifstream iFile;
	iFile.open("Configuration_hmm.txt");
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
		str.append("..\\Debug\\input\\");
		str = str + line.substr(semiColon + 1);
		stringstream stream(str);

		if (line.find("piFile") != string::npos)
		{
			stream >> piFile;
		}

		if (line.find("aFile") != string::npos)
		{
			stream >> aFile;
		}

		if (line.find("bFile") != string::npos)
		{
			stream >> bFile;
		}


		if (line.find("obsFile") != string::npos)
		{
			stream >> obFile;
		}

	}
	iFile.close();
	return 1;
}

//create source matrix from file
void get2DMatricesInput(char*fileName, int type)
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
	int row = 0;
	int col = 0;
	char*split = NULL;
	while ((getline(iFile, line1)))
	{
		char line[15000];
		strncpy_s(line, line1.c_str(), line1.length());
		split = strtok(line, "\t");
		col = 0;
		while (split)
		{

			stringstream str(split);
			long double val;
			str >> val;
			if (type == 1)
				Ahmm[row][col] = val;
			if (type == 2)
				Bhmm[row][col] = val;
	
			split = strtok(NULL, "\t"); // split by comma
			col++;
		}

		row++;
		if (type == 1)
		{
			AhmmCols = col;
			AhmmRows = row;
		}

		else if (type == 2)
		{
			//BhmmCols = col;
		//	BhmmRows = row;
		}	
	}
}


//get obsSequence from obs file
void getObsSeq(char *fileName)
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
	while ((getline(iFile, line1)))
	{
		char line[150];
		strncpy_s(line, line1.c_str(), line1.length());
		stringstream str(line);
		long double val;
		str >> val;
		Obs[count++] = val ;//- 1;
	}
	numberOfObservations = count;
}

//get Pi Array from file
void getPiArray(char *fileName)
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
	char*split = NULL;
	while ((getline(iFile, line1)))
	{
		char line[600];
		strncpy_s(line, line1.c_str(), line1.length());
		split = strtok(line, "\t");
		while (split)
		{
			stringstream str(split);
			long double val;
			str >> val;
			pi[count++] = val;
			split = strtok(NULL, "\t"); // split by \t
		}
		alphaCount = count;
		count++;
	}

}

//Init alpha values
void initAlpha()
{
	for (int j = 0; j < hmmN; j++)
	{
		int ob = Obs[0];
		alpha[0][j] = pi[j] * Bhmm[j][ob];
	}
}

//Problem1 , induction on Alpha matrix
void inductAlphas()
{
	// loop for all observations
	for (int t = 1; t <numberOfObservations; t++)
	{

		for (int j = 0; j < hmmN; j++)
		{
			long double sum = 0;
			for (int i = 0; i < hmmN; i++)
			{
				sum += alpha[t - 1][i] * Ahmm[i][j];
			}
			int obs = Obs[t];
			alpha[t][j] = sum*Bhmm[j][obs];
		}
	}
}

//terminate problem 1 and return probability
long double Ptermination()
{
long double	Probterm = 0;
	for (int i = 0; i <hmmN; i++)
		Probterm += alpha[numberOfObservations - 1][i];
	//cout << Probterm<<"\n";
//	printf("\n Iteration Number %d Probabilty %0.20E", iterationCounter, Probterm);
	return Probterm;
}

// Backward procedure for Problem 1
void backWard()
{
	for (int i = 0; i < hmmN; i++)
	{
		Beta[numberOfObservations - 1][i] = 1;
	}

	//induct
	int ob = Obs[numberOfObservations - 1];

	for (int t = numberOfObservations - 2; t >= 0; t--)
	{
		int ob = Obs[t + 1];
		for (int i = 0; i < hmmN; i++)
		{
			long double sum = 0;

			for (int j = 0; j < hmmN; j++)
			{
				sum += Ahmm[i][j] * Bhmm[j][ob] * Beta[t + 1][j];
			}

			Beta[t][i] = sum;
		}
	}
}
//Initialize viterbi algo
void initViterbi()
{
	int Obs0 = Obs[0];
	for (int i = 0; i < hmmN; i++)
	{
		delta[0][i] = pi[i] * Bhmm[i][Obs0];
		psi[0][i] = 0;
	}
}

//computer psi and delta matrix
void recurViterbi()
{
	for (int t = 1; t < numberOfObservations; t++)
	{
		for (int j = 0; j < hmmN; j++)
		{
			long double maxdel = 0;
			long double maxpsi = 0;


			for (int i = 0; i < hmmN; i++)
			{
				int obst = Obs[t];
				long double calculateVal = delta[t - 1][i] * Ahmm[i][j];
				long double calculateVal2 = calculateVal*Bhmm[j][obst];

				if (calculateVal2>maxdel)
				{
					maxdel = calculateVal2;
					delta[t][j] = maxdel;
				}

				if (calculateVal > maxpsi)
				{
					maxpsi = calculateVal;
					psi[t][j] = i;
				}
			}

		}
	}
	return;
}


// compute qTStar 
void terminateViterbi()
{
	long double max = 0;
	pStar = delta[numberOfObservations - 1][0];
	//cout << "\n prob " << pStar<<"  "  ;
	for (int i = 1; i < hmmN; i++)
	{
		if (delta[numberOfObservations - 1][i]>max)
		{
			pStar = delta[numberOfObservations - 1][i];
			qTStar = i;
		}
	}
	qTStarArr[numberOfObservations - 1] = qTStar;

}

// Backtracking for viterbi
////computer qTStar array
void viterbiStateSequenceBackTrack()
{
	for (int t = numberOfObservations - 2; t >= 0; t--)
	{
		int qStarttplus1 = qTStarArr[t + 1];
		qTStarArr[t] = psi[t + 1][qStarttplus1];

	}
}


//Viterbi Algorithm
void viterbi()
{
	initViterbi();
	recurViterbi();
	terminateViterbi();
	viterbiStateSequenceBackTrack();

}

//Calculate Gamma Array
void calcGamma()
{

	for (int t = 0; t < numberOfObservations; t++)
	{
		long double sum = 0.0;
		for (int j = 0; j < hmmN; j++)
			sum = sum + alpha[t][j] * Beta[t][j];

		for (int i = 0; i < hmmN; i++)
		{
			Gamma[t][i] = (alpha[t][i] * Beta[t][i]) / sum;
		}
	}

}

void problem2()
{
	calcGamma();
	viterbi();

}

// Re estimate pi , A and B matrices
void reEstimate()
{

	//for pi
	for (int i = 0; i < hmmN; i++)
	{
		piBar[i] = Gamma[0][i];
	}

	//for Ahmm
	for (int i = 0; i < hmmN; i++)
	{
		for (int j = 0; j < hmmN; j++)
		{
			long double num = 0;
			long double den = 0;

			for (int t = 0; t < numberOfObservations; t++)
			{

				num += zhi[t][i][j];
				den += Gamma[t][i];

			}

			Ahmmbar[i][j] = num / den;
		}
	}

	long double num = 0;
	long double den = 0;

	int tempArr[hmmN];
	int iter = 0;
	while (iter < hmmN)
	{
		tempArr[iter] = 0;
		iter++;
	}

	//set Bbar to 0 
	for (int q = 0; q < hmmN; q++)
	{
		for (int j = 0; j < BhmmCols; j++)
		{

			Bhmmbar[q][j] = 0;
		}

	}
	int q = 0;

	//set Bhmmbar
	for (int q = 0; q < numberOfObservations; q++)
	{
		int qVal = qTStarArr[q];
		int obsVal = Obs[q];
		Bhmmbar[qVal][obsVal] += 1;

		tempArr[qVal]++;
	}


	for (int q = 0; q < hmmN; q++)
	{
		for (int j = 0; j < BhmmCols; j++)
		{
			if (tempArr[q] != 0)
				Bhmmbar[q][j] /= tempArr[q];
		}

	}

	//Fix Bhmmbar , as told by sir . 
	for (int q = 0; q < hmmN; q++)
	{

		int count = 0;
		long double max = 0;
		int jmaxIndex = 0;
		for (int j = 0; j < BhmmCols; j++)
		{

			if (Bhmmbar[q][j]>max)
			{
				// to get max index
				max = Bhmmbar[q][j];
				jmaxIndex = j;

			}
			if (Bhmmbar[q][j] <= 0.000000000000000000000000001)
			{
				Bhmmbar[q][j] += pow(10, -20);
				count++;
			}
		}
		long double subt = pow(10, -20);
		subt = subt*count;
		Bhmmbar[q][jmaxIndex] -= subt;

	}
}

//prob 3 , reestimation
void problem3()
{

	for (int t = 0; t < numberOfObservations - 1; t++)
	{

		long double denom = 0;

		int ob = Obs[t + 1];
		for (int i = 0; i < hmmN; i++)
		{

			for (int j = 0; j < hmmN; j++)
			{
				denom += alpha[t][i] * Ahmm[i][j] * Bhmm[j][ob] * Beta[t + 1][j];
			}
		}
		for (int i = 0; i < hmmN; i++)
		{
			for (int j = 0; j < hmmN; j++)
			{
				long double  num = (alpha[t][i] * Ahmm[i][j] * Bhmm[j][ob] * Beta[t + 1][j]);
				zhi[t][i][j] = num / denom;
			}
		}
	}

	reEstimate();
}

void problem1()
{

	initAlpha();
	inductAlphas();
	long double pTerm = Ptermination();
	backWard();

}

//Copies data from Bar matrices , Abar to A, BBar to B 

void copyData()
{

	for (int i = 0; i < hmmN; i++)
	{

		for (int j = 0; j < hmmN; j++)
		{
			Ahmm[i][j] = Ahmmbar[i][j];

		}
	}
	for (int i = 0; i < hmmN; i++)
	{

		for (int j = 0; j < BhmmCols; j++)
		{
			Bhmm[i][j] = Bhmmbar[i][j];

		}
	}
	for (int i = 0; i < hmmN; i++)
	{
		pi[i] = piBar[i];
	}

}


// used to write matrices to file
void writeToFile(char*fileName, int counter, int rows, int cols, int type)
{
	FILE *fp = fopen(fileName, "w");
	if (!fp)
		cout << "UNABLE TO OPEN FILE FOR WRITING";


	for (int k = 0; k < counter; k++)
	{

		if (type == 6)
		{
			fprintf(fp, "\n ZHI [%d ]\n\n ", k);

		}
		for (int i = 0; i < rows; i++)
		{

			for (int j = 0; j < cols; j++)
			{
				long double val = 0;
				int valInt = 0;

				//for different files
				switch (type)
				{

				case 0:val = Ahmm[i][j];
					break;

				case 1:val = Bhmm[i][j];
					break;

				case 2:val = alpha[i][j];
					break;

				case 3:val = Beta[i][j];
					break;

				case 4:val = Gamma[i][j];
					break;

				case 5:val = pi[j];
					break;

				case 6:val = zhi[k][i][j];
					break;

				case 7:val = delta[i][j];
					break;
				case 8:valInt = qTStarArr[j];
					break;

				}
				if (type == 8)
					fprintf(fp, " %d ,", valInt);

				else
					fprintf(fp, " %0.20E ,", val);


			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "\n");
	}

	fclose(fp);
}



//read codebook from file
void readCodeBook(char*baseCodeBook)
{
	ifstream iFile;
	string line1;

	char*split = NULL; 
	char codebookpath[200];
	sprintf(codebookpath, "%s%s",baseCodeBook,"codebook.txt" );
	iFile.open(codebookpath);
	if (!iFile)
	{
		perror("Could not open cis file.. Press any key to exit");
		int k;
		cin >> k;
		exit(0);
	}


	int row = 0;
	while ((getline(iFile, line1)))
	{
		int col = 0;
		char line[600];
		strncpy_s(line, line1.c_str(), line1.length());
		split = strtok(line, ",");
		while (split)
		{

			stringstream str(split);
			long double	 val;

			string forLen = str.str();
			if (strlen(forLen.c_str()) >= 1)
			{
				str >> val;
				codebook[row][col] = val;
			}

			split = strtok(NULL, ","); // split by comma
			col++;
		}
		row++;
	}




}

// calc obsa num based upon closest dist in codebook
int getMinDistanceEntry(long double Ci[][12], int ciIndex)

{

	int codeBookColumns = 12;
	int codeBookRows = 32;

	int weight[] = { 1, 3, 7, 13, 19, 22, 25, 33, 42, 50, 56, 61 };
	long double min = FLT_MAX;
	//tokhurafor
	int minIndex = 0;

	for (int i = 0; i <codeBookRows; i++)
	{
		long double  dLoc = 0;

		for (int j = 0; j < codeBookColumns; j++)
		{

			dLoc += weight[j] * ((Ci[ciIndex][j] - codebook[i][j])*(Ci[ciIndex][j] - codebook[i][j]));
		}

		if (dLoc < min)
		{

			min = dLoc; // make it minimum
			minIndex = i;

		}

	}

	return minIndex;

}


//create obs from cis
void createObsFiles(char* fullPath, char*fileName, char*baseOutput)
{

	//readCis
	long double Ci[1000][12];

	memset(Ci, 0, sizeof(Ci));
	int ciIndex = 0;
	ifstream iFile;

	iFile.open(fullPath);
	if (!iFile)
	{
		perror("Could not open cis file.. Press any key to exit");
		int k;
		cin >> k;
		exit(0);
	}
	string line1;

	char*split = NULL;
	while ((getline(iFile, line1)))
	{
		char line[600];
		strncpy_s(line, line1.c_str(), line1.length());
		split = strtok(line, ",");
		int ciCol = 0;
		while (split)
		{

			stringstream str(split);
			long double	 val;

			string forLen = str.str();
			if (strlen(forLen.c_str()) >= 1)
			{
				str >> val;
				Ci[ciIndex][ciCol++] = val;
			}

			split = strtok(NULL, ","); // split by comma
		}
		ciIndex++;

	}

	iFile.close();
	//int *ObsVector = (int*)malloc(sizeof(int)*(ciIndex+10));

	char obsFile[100];
	strcpy(obsFile,baseOutput);
	strcat(obsFile, fileName);
//	strcat(obsFile, fileCisURL);
	char outputString[4000];
	strcpy(outputString, "");
	for (int i = 0; i < ciIndex; i++)
	{
		int minIndex = getMinDistanceEntry(Ci, i);
		//ObsVector[i] = minIndex;

		char obNum[25];
		//sprintf(obNum, "%d", ObsVector[i]);
		
		sprintf(obNum, "%d", minIndex);
		if (i < ciIndex - 1)
			strcat(obNum, "\n");
		
		strcat(outputString, obNum);
	}

	FILE* oFile = fopen(obsFile, "w");
	fprintf(oFile, outputString);
	//readCodeBook
	//cout << "\n Obs " << fileName;
	fclose(oFile);
	//matchFeatureVector inCodeBook 
	//free(ObsVector);
}


// 1/32 for all 
void initB()
{
	long double oneby32 = 1.0 / 32.0;

	for (int i = 0; i < BhmmRows; i++)
	{
		for (int j = 0; j < BhmmCols; j++)
		{

			Bhmm[i][j] = oneby32;
		}
	}
}

//call parseHMM Config File 
long double hmmModelling(char* obFilefullPath, char* obFileName)
{

	
	//qwchar *PIFILE = piFile;
	//getPiArray(PIFILE);
	//get2DMatricesInput(aFile, 1);
	//get2DMatricesInput(bFile, 2);
	getObsSeq(obFilefullPath);
	//initB();

	iterationCounter = 0;
	long double oldProb = 0;
	long double newProb = 1;
	while (iterationCounter++ <iterationNumber && (newProb != oldProb))
	{
		problem1();
		problem2();
		problem3();
		copyData();
		oldProb = newProb;
		newProb = pStar;
	
	}
//	cout << " \n Prob " << newProb;

	/*writeToFile("OUTPUT//Ahmm.csv", 1, hmmN, hmmN, 0);
	writeToFile("OUTPUT//Bhmm.csv", 1, hmmN, BhmmCols, 1);
	writeToFile("OUTPUT//Beta.csv", 1, numberOfObservations, hmmN, 3);
	writeToFile("OUTPUT//Alpha.csv", 1, numberOfObservations, hmmN, 2);
	writeToFile("OUTPUT//Gamma.csv", 1, numberOfObservations, hmmN, 4);
	writeToFile("OUTPUT//Pi.csv", 1, 1, hmmN, 5);
	writeToFile("OUTPUT//zhi.csv", numberOfObservations, hmmN, hmmN, 6);
	writeToFile("OUTPUT//delta.csv", 1, numberOfObservations, hmmN, 7);
	writeToFile("OUTPUT//qStar.csv", 1, 1, numberOfObservations, 8);
*/

	//int x;
	//cin >> x;


	return newProb;
}

long double Atemp[hmmN][hmmN];
long double Btemp[hmmN][ObsLen];
long double piTemp[hmmN];


//add 20 matrices for averaging later . store sum in tempMatrix
void addTomatricesForAverage()
{
	for (int i = 0; i < hmmN; i++)
		piTemp[i] += pi[i];

	for (int i = 0; i < hmmN; i++)
		for (int j = 0; j < hmmN; j++)
			Atemp[i][j] += Ahmm[i][j];

	for (int i = 0; i < hmmN; i++)
		for (int j = 0; j < BhmmCols; j++)
			Btemp[i][j] += Bhmm[i][j];
}


//calculate average of A, B , pi matrices
void ABpiAverage()
{
	for (int i = 0; i < hmmN; i++)
		pi[i] = piTemp[i]/20.0;

	for (int i = 0; i < hmmN; i++)
	for (int j = 0; j < hmmN; j++)
		Ahmm[i][j] = (long double)Atemp[i][j] / (long double)20.0;

	for (int i = 0; i < hmmN; i++)
	for (int j = 0; j < BhmmCols; j++)
		Bhmm[i][j] = (long double)Btemp[i][j] / (long double) 20.0;


		long double sum = 0.0;
		/*
		for (int i = 0; i < hmmN; i++)
		{
			sum = 0;

			for (int j = 0; j < hmmN; j++)
			{
				sum += Ahmm[i][j];
			}
			cout << " a  sum" << sum;
		}
		
		cout << "\n b sum";

		for (int i = 0; i < hmmN; i++)
		{
			sum = 0;

			for (int j = 0; j < BhmmCols; j++)
			{
				sum += Bhmm[i][j];
			}
			cout << "   sum" << sum;
		}
		*/
}


long double Ahmm_backup[hmmN][hmmN];
long double Bhmm_backup[hmmN][100];
long double pi_backup[hmmN];


// For each digit . calculate using A ,B, pi originally copied from File 
void Use_backUpA_B_pi()
{

	for (int i = 0; i < hmmN; i++)
		pi[i] = pi_backup[i];

	for (int i = 0; i < hmmN; i++)
	for (int j = 0; j < hmmN; j++)
		Ahmm[i][j] = Ahmm_backup[i][j];

	for (int i = 0; i < hmmN; i++)
	for (int j = 0; j < BhmmCols; j++)
		Bhmm[i][j] = Bhmm_backup[i][j];

}

// after every Iteration of 20 samples , use the averaged matrices
void backUpA_B_pi()
{

	for (int i = 0; i < hmmN; i++)
		pi_backup[i]=pi[i];

	for (int i = 0; i < hmmN; i++)
	for (int j = 0; j < hmmN; j++)
		Ahmm_backup[i][j] = Ahmm[i][j];

	for (int i = 0; i < hmmN; i++)
	for (int j = 0; j < BhmmCols; j++)
		Bhmm_backup[i][j] = Bhmm[i][j];


}

//performs training of samples , by creating A, B , pi matrices and then averaging overt them.
//input is obs File
void trainHMMData(char*inputDirRegex, char*baseHMMOutputs, char*itemsArray[], int numItems, int trainSamples)
{


//	cout << "\n Train HMM Data";
	//readCodeBook();
	char fileName[200];
	int looper = 3;
	parse_HMMConfigFile();

	for (int	i = 0; i <numItems; i++)
	{
		char *PIFILE = piFile;
		getPiArray(PIFILE);
		get2DMatricesInput(aFile, 1);
		get2DMatricesInput(bFile, 2);

		backUpA_B_pi();
	//	initB();

		

			for (int k = 0; k < looper; k++)
			{

				memset(Atemp, 0, sizeof(long double)*hmmN*hmmN);
				memset(Btemp, 0, sizeof(long double)*hmmN*ObsLen);
				memset(piTemp, 0, sizeof(long double)*hmmN);
					long double probPstar = 0.0;

					char partfileName[100];
					sprintf(partfileName, "%s", itemsArray[i]);
//					cout << "\n";
					for (int j = 1; j <=trainSamples; j++)
					{
						char FileName[150];

						strcpy(FileName, partfileName);
						char seqn[4];
						sprintf(seqn, "%d", j);
					//	strcat(FileName, "_");
						strcat(FileName, seqn);
						strcat(FileName, ".TXT");

						char fullPath[200];

						strcpy(fullPath, inputDirRegex);
						strcat(fullPath, FileName);

						Use_backUpA_B_pi();

						probPstar += hmmModelling(fullPath, FileName);
						addTomatricesForAverage();
						cout << "\n" << FileName << "  " << probPstar;

					}

					ABpiAverage();
					backUpA_B_pi();
					char resultFileWrite[100];
					char APath[200];
					sprintf(APath, "%s%s", baseHMMOutputs, "A");
					strcpy(resultFileWrite, APath);
					strcat(resultFileWrite, partfileName);
					strcat(resultFileWrite, ".TXT");

					writeToFile(resultFileWrite, 1, AhmmRows, AhmmCols, 0);

					char BPath[200];
					sprintf(BPath, "%s%s", baseHMMOutputs, "B");
					strcpy(resultFileWrite, BPath);
					strcat(resultFileWrite, partfileName);
					strcat(resultFileWrite, ".TXT");

					writeToFile(resultFileWrite, 1, BhmmRows, BhmmCols, 1);

					char piPath[200];
					sprintf(piPath, "%s%s", baseHMMOutputs, "pi");

					strcpy(resultFileWrite, piPath);
					strcat(resultFileWrite, partfileName);
					strcat(resultFileWrite, ".TXT");

					writeToFile(resultFileWrite, 1, 1, hmmN, 5);

					//cout << "\n" << digit << "    "<<k;
		}
	}

}



// Below part is for testing

// 3D matrices to store models for A, B , PI .. Xtest [i] stores I th digit model
const int numberOfTest = 10;
long double Atest[numberOfTest][hmmN][hmmN];
long double Btest[numberOfTest][hmmN][40];
long double piTest[numberOfTest][hmmN];


//create source matrix from file
void load2DHMMMatricesInput(char*fileName,int index, int type)
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
	int row = 0;
	int col = 0;
	char*split = NULL;
	while ((getline(iFile, line1)))
	{
		char line[15000];
		strncpy_s(line, line1.c_str(), line1.length());
		split = strtok(line, ",");
		col = 0;
		while (split)
		{
			stringstream str(split);
			long double val;
			str >> val;

			if (type == 1)
				Atest[index][row][col] = val;
			if (type == 2)
				Btest[index][row][col] = val;


			if (type == 3)
				piTest[index][col]=val;
			split = strtok(NULL, ","); // split by comma
			col++;
		}

		row++;
		if (type == 1)
		{
			AhmmCols = col;
			AhmmRows = row;
		}

		else if (type == 2)
		{
		//	BhmmCols = col;
		//	BhmmRows = row;
		}
	}
}

//copies to A, pi and B . for use in hmm problems . A, B , pi are used
void copyApiB(int indexin3d)
{

	for (int i = 0; i < hmmN; i++)
		pi[i] = piTest[indexin3d][i];

	for (int i = 0; i < hmmN; i++)
	for (int j = 0; j < hmmN; j++)
		Ahmm[i][j] = Atest[indexin3d][i][j];

	for (int i = 0; i < hmmN; i++)
		for (int j = 0; j < BhmmCols; j++)
			Bhmm[i][j] = Btest[indexin3d][i][j];

}


//call init Test before this
//called using external inteface . provide ObsFile Path (.txt)
int tellWhatMySoundIs(char *basePath, char *fileName, int numItems, char *itemArray[])
{


	char fullPath[200];
	strcpy(fullPath, basePath);
	strcat(fullPath, fileName);
	getObsSeq(fullPath);

	int count = 0;

	long double pObyLambda = 0;
	int maxIndex = 9;


	for (int i = 0; i <numItems; i++)
	{
		//cout << "\n"<<itemArray[i];
			copyApiB(i);
			initAlpha();
			inductAlphas();
			long double max = Ptermination();
			//		cout<<"\nMODEL "<< i<<" Prob " << max;
			if (max > pObyLambda)
			{

				pObyLambda = max;
				maxIndex = i;
			}
		//	cout << " \n DETECTED " << maxIndex << "  on " << fullPath;

	}

	return maxIndex;
	//	return itemArray[maxIndex];
}


// call init test speech before this
//this function is use to check which model suits best for a particular observation
int testSpeech(char*inputRegexPathObs, char*baseOutput, int numItems, char *itemArray[])
{
	char finalResults[200];
	sprintf(finalResults, "%s%s", baseOutput, "finalResults.txt");
	FILE *file = fopen(finalResults, "w");
	fclose(file);

//	cout << "\n Performing Testing of Speech ";
	for (int i = 0; i <numItems; i++)
	{
		char item[40];
		sprintf(item, "%s", itemArray[i]);
		char tempFileName[30];
			strcpy(tempFileName, inputRegexPathObs);
//			strcat(tempFileName, "_");
				int count = 0;
			
			for (int j = 21; j <= 30; j++)
			{
				char digit2[4];
				sprintf(digit2, "%d", j);

				char fullPath[300];
				strcpy(fullPath, inputRegexPathObs);
				strcat(fullPath, item);
//				strcat(fullPath, item);
				strcat(fullPath, digit2);
				strcat(fullPath, ".TXT");

				getObsSeq(fullPath);
				
				long double pObyLambda = LDBL_MIN;
				int maxIndex = -1;

				for (int i = 0; i < numberOfTest; i++)
				{

					copyApiB(i);
					initAlpha();
					inductAlphas();
					long double max = Ptermination();
			//		cout<<"\nMODEL "<< i<<" Prob " << max;
					if (max > pObyLambda)
					{
					
						pObyLambda = max;
						maxIndex = i;
					}
				}

				if (maxIndex == i)
					count++;
				FILE *file = fopen(finalResults, "a");

				char result[300];

				if (maxIndex != -1)

				{
					sprintf(result, "\n Detected %s  on %s ", itemArray[maxIndex], fullPath);
				//	cout << " \n DETECTED " << itemArray[maxIndex] << "  on " << fullPath;
				}
				else
				{
					sprintf(result, "\n Detected %s  on %s ", "NOTHING", fullPath);
			//		cout << " \n DETECTED " <<" NOTHING"<< "  on " << fullPath;
				}

				fprintf(file, result);
				fclose(file);
			}
		//	cout << "\n\n\n------- Accuracy " << (float)count * 100 / (float)(10);
		//	FILE *file = fopen("output\\finalREsults.txt", "a");
			//sprintf(result, "\n \nAccuracy %f Percent ", (float)count * 100 / (float)(numberOfTest));
		//		fprintf(file, result);
		//	fclose(file);
		//	cout << "\n";
		}
//	cout << "\n\n Results are stored in output Folder as follow : \n1.Cis in // cis// folder \n2.Observations in // obs// Folder .\n3. A, B , Pi Models for all digits in // hmmOutputs// folder \n4. Codebook , Final results in // Output// ";

//	int a1;
	//cin >> a1;
	return 0;
	

}

//initial Test Speech . initialize /reset Global values. Call this externally before starting testing
int initTestSpeech(char *fullPath, int numItems, char *itemsArray[])
{
	for (int i = 0;i < numItems ; i++)
	{
		char item[20];
		sprintf(item, "%s", itemsArray[i]);

			char tempFileName[300];
			strcpy(tempFileName, fullPath);
			strcat(tempFileName, "A");
			strcat(tempFileName, item);
			strcat(tempFileName, ".TXT");

			load2DHMMMatricesInput(tempFileName, i, 1);

			strcpy(tempFileName, fullPath);
			strcat(tempFileName, "B");
			strcat(tempFileName, item);
			strcat(tempFileName, ".TXT");

			load2DHMMMatricesInput(tempFileName, i, 2);

			strcpy(tempFileName, fullPath);
			strcat(tempFileName, "pi");
			strcat(tempFileName, item);
			strcat(tempFileName, ".TXT");

			load2DHMMMatricesInput(tempFileName, i, 3);

	}
	return 0;

}
