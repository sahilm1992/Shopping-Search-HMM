#include "stdafx.h"
#include"cepstrum.h"
int cepStrumP = 12;
long double cepStrumAlpha[13][13];
long double Ais[13];
vector<string> ignoreTerms;
int maxValue = 0;
long double Energy[13];
long double dcSHIFTVALUE = 0.0;
long double Ris[13];
long double CepStral[13];
double HammingWindow[320];
long long int SampleListArray[75000];

// The following values are redfined in parseCepstrum_ConfigFile functions by using constants from Configuration.txt file
int frameSize = -1;
int normalValue = 15000;
int noOfSamples = -1;
#include<time.h>

//Function to byPass the Initial inValid Strings in File
int ifValidSample(string line1)
{
	if ((line1.length()) > 2 && strtoll(line1.c_str(), NULL, 10) == 0)
	{
		return 0;

	}
	else
		return 1;

}


long long int getSquare(long int x1, long int x2)
{
	return x1*x2;
}

void preProcess(char  *fileName, char*modifiedInputsPath)
{
	int countnew = 0;
	for (int i = 0; i < noOfSamples; i++)
	{
		if (SampleListArray[i] <= 4)
			countnew++;

		if (SampleListArray[i] >= 5)
			break;
	}
	int start = countnew;

//	long int tempSampleList[75000];
		
	long long int EnergySamples[3000];
	memset(EnergySamples, 0, sizeof(EnergySamples));
	int indices = 0;
	long long int initEnergy = 0;

	for (int i = start; i < noOfSamples; i++)
	{
		long long int Energy = getSquare(SampleListArray[i], SampleListArray[i]);
	
		if (indices<5)
			initEnergy += Energy;

		if ((i -start+1)% 300 == 0)
			indices++;
			
		EnergySamples[indices] += Energy;

	}	
	long long int initAvgThreshold =( initEnergy*(float)1.5f / (float)5.0);

	int begSIGNAL = start;
	int endSIGNAL = noOfSamples;
	int endDone = 0;
	int i1 = 0;
	for (i1 = 4; i1 < indices -4&&endDone == 0; i1++)
	{
		if (EnergySamples[i1]>initAvgThreshold)
		{
			
			//found Data
			if (begSIGNAL==start)
				begSIGNAL = start + (i1 * 300);
	
			
			while (i1++ < indices)

			{
				if (EnergySamples[i1] < initAvgThreshold)
					{
						endDone = 1;
						endSIGNAL = start + (i1 * 300);
						break;
					}
			
			}
		}

	}

	for (; i1 < indices-4; i1++)
	{
		if (EnergySamples[i1]>initAvgThreshold*1.3f)
		{
			while (EnergySamples[i1] > initAvgThreshold*1.3f && i1<indices)
			{
				endSIGNAL = start + (i1 * 300);


				i1++;
			}



		}
	}

	//noOfSamples = noOfSamples - countnew;
	//long long int tempArray[75000];
	memcpy(SampleListArray, SampleListArray + begSIGNAL, sizeof(SampleListArray[0])*(endSIGNAL - begSIGNAL));

	//memcpy(SampleListArray, tempArray, sizeof(SampleListArray[0])*(endSIGNAL - begSIGNAL));

	char fullPath[120];
	strcpy(fullPath, (modifiedInputsPath));
	strcat(fullPath, fileName);
	strcat(fullPath, "_COPY");
	FILE *file = fopen(fullPath,"w");
	noOfSamples = endSIGNAL - begSIGNAL;
	for (int i = 0; i < endSIGNAL - begSIGNAL; i++)
	{
		fprintf(file,"%d\n", SampleListArray[i]);
	}
	fclose(file);


}





//creates a list of Samples having Amplitudes 
// also finding dc Shift if any in the sample
void buildSampleList(char *fileName)
{
	long long  int dcVal = 0;
	int num = 0;
	//vector<  long long int>Samples;
	//std::cout << " \n Cis for " << fileName << "  ";
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
	string line;
	while ((getline(iFile, line)))
	{
		count++;
		if (count<7 && !ifValidSample(line))
			continue;
		else
		{
			line[line.length()] = '\0';    //Remove Delimiter
			stringstream str(line);
			int val;
			str >> val;
			SampleListArray[num++] = val;
			//			Samples.push_back(val);
			if (abs(val)>abs(maxValue))
				maxValue = abs(val);

			dcVal += val;
		}
	}

	noOfSamples = num;
	long int tempSampleListArray[75000];
	//int countnew = 0;
	//noOfSamples = noOfSamples / 2;
	
	
	//noOfSamples = noOfSamples - countnew; 

	dcSHIFTVALUE = (double)dcVal / noOfSamples;



	iFile.close();

}

//Normalizes values to maxValue
void normalizeSample()
{
	for (int i = 0; i < noOfSamples; i++)
	{
		SampleListArray[i] = ((long long int)SampleListArray[i] * (long double)normalValue / (long double)maxValue);
	
	}


}


// Parse the Configuration File to define Constants
int  parseCepstrum_ConfigFile()
{
	ifstream iFile;
	iFile.open("Configuration_cepstrum.txt");
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
		line[line.length()] = '\0';    //Remove Delimiter


		int semiColon = line.find(":");
		string str;
		str = line.substr(semiColon + 1);
		stringstream stream(str);

		if (line.find("frameSize") != string::npos)
		{
			stream >> frameSize;
		}
		if (line.find("cepStrumP") != string::npos)
		{
			stream >> cepStrumP;
		}
		if (line.find("normalValue") != string::npos)
		{
			stream >> normalValue;
		}
	}
	iFile.close();
	return 1;
}


// return DC Shifted wave
void shiftDC()
{
	//	long long int temp=0;
	for (int i = 0; i < noOfSamples; i++)
		SampleListArray[i] -= dcSHIFTVALUE;


}

//Generates coefficients for hamming HammingWindow
void HammingWindowFunction(int max)
{
	for (int i = 0; i < max; i++)
	{
		HammingWindow[i] = 0.54 - 0.46*cos((2 * 22.0 / 7.0)*i / (max - 1));
	}
}

//function to calculate Ri's coefficient
void calcRis(long long int * samples, int size, int number = cepStrumP)
{
	int i = 0;
	int sampleSize = size;

	for (int i = 0; i <=number; i++)
	{
		long long int sum = 0;
		for (int j = 0; j < size - i - 1; j++)
			sum += (long long int)(samples[j] * (samples[j + i]));
		Ris[i] = sum;
		//	*iter = *iter * (HammingWindow[iter - samples.begin()]);
	}
	
}

// applies hamming window on middle frame
long long int* windowing(int beg,int size)
{

	long long int *mem = (long long int*)malloc(sizeof(long long int)*(size));
	for (int i = 0; i < size; i++)
	{
		mem[i] = (long long int)(SampleListArray[i+beg] * HammingWindow[i]);

	}
	
	return mem;
}

//Durbin's Algorithm, generates Ai's

void durbin()
{
	long double Energy[13] = {};
	Energy[0] = Ris[0];

	long double  K[13] = {};

	K[1] = Ris[1] / Energy[0];

	for (int i = 1; i <= cepStrumP; i++)
	{
		long double sum = 0;
		for (int j = 1; j <= i - 1; j++)
		{

			sum = sum + (cepStrumAlpha[j][i - 1] * Ris[i - j]);

		}
		K[i] = (Ris[i] - sum) / Energy[i - 1];
		cepStrumAlpha[i][i] = K[i];
		for (int j = 1; j <= i - 1; j++)
			cepStrumAlpha[j][i] = cepStrumAlpha[j][i - 1] - K[i] * cepStrumAlpha[i - j][i - 1];


		Energy[i] = (1 - K[i] * K[i]) * Energy[i - 1];
	}

	for (int i = 1; i <= cepStrumP; i++)
	{
		Ais[i] = cepStrumAlpha[i][cepStrumP];
		//cout << "\n " << Ais[i];
	}
}

//Calculates Ci's
void cepstralCoeff()
{
	for (int m = 1; m <= cepStrumP; m++)
	{
		long double sum = 0;

		for (int k = 1; k <= m - 1; k++)
		{

			sum = sum + ((double)k / (double)m)*CepStral[k] * Ais[m - k];
		}

		CepStral[m] = Ais[m] + sum;
	}
}




//reset values of global arrays
void InitArraysandValues()
{

	for (int i = 0; i <= cepStrumP; i++)
	Ais[i] = 0;


	for (int i = 0; i <= cepStrumP; i++)
	Ris[i] = 0;


	for (int i = 0; i <= cepStrumP; i++)
	CepStral[i] = 0;

	for (int i = 0; i <= cepStrumP; i++)
	for (int j = 0; j <= cepStrumP; j++)
	cepStrumAlpha[i][j] = 0;

	

}


void initCepstrum_CalcuateProcess()
{
	parseCepstrum_ConfigFile();
	HammingWindowFunction(320);
}


//init Cepstrum call before calling this
//calculates cis. call this from external interface
void calculateCepstrum(char *fullPath, char*fileName, char *outputBase, char *appendFile, FILE *univFP, char*modifiedInputsPath)
{

	FILE *CiFP = NULL;
	string str(fileName);
	//	str = "C_"+ str;
	str = outputBase + str;
	CiFP = fopen(str.c_str(), "w");

	buildSampleList(fullPath);
	preProcess(fileName,modifiedInputsPath);

	if (dcSHIFTVALUE != 0)
	{
		shiftDC();
	}

	normalizeSample();
	int maxNumber = (noOfSamples - 320);
	maxNumber = maxNumber / 80;

	for (int i = 0; i < maxNumber; i++)
	{
		InitArraysandValues();

		//	vector<long long int> frame(SamplesOrig.begin() + i*80, SamplesOrig.begin()+i*80 +frameSize);
		//windowFunction(320);// creates window array
		long long int * frame = windowing(i * 80, frameSize);
		//	vector<long long int> windowedSample = windowing(frame); // apply hamming window
		calcRis(frame, frameSize, cepStrumP);
		free(frame);

		//	calcRis(windowedSample, cepStrumP); // calculate Risi coefficients for windowed sample
		durbin();// apply durbins algorithm which yields A values.
		cepstralCoeff();

		//	write cepstral to csv file
		for (int j = 1; j <= cepStrumP; j++)
		{
			//cout << " \n\n\n C[" << i << "] " << CepStral[i];
			if (univFP)
				fprintf(univFP, "%Lf,", CepStral[j]);

			if (CiFP)
				fprintf(CiFP, "%Lf,", CepStral[j]);
		}
		if (i < maxNumber)
		{
			if (CiFP)
				fprintf(CiFP, "\n");
		}

		if (univFP)
			fprintf(univFP, "\n");
	}
	if (CiFP)	
		fclose(CiFP);

	//fprintf(univFP, "\n");
	
}