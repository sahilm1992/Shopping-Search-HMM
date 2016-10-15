//

#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<sstream>
#include<fstream>
#include<math.h>
#include<algorithm>
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<sstream>
#include<fstream>
#include<math.h>
#include<algorithm>
using namespace std;

#ifndef cepStrum_H
#define cepStrum_H
	extern int cepStrumP;
	extern int maxValue ;
	extern long double dcSHIFTVALUE ;
	extern vector<string> ignoreTerms;
	extern double window[320];
	extern long double cepStrumAlpha[13][13];
	extern long double Ais[13];
	extern long double Energy[13];
	extern long double Ris[13];
	extern long double CepStral[13];

	// The following values are redfined in parseConfigFile functions by using constants from Configuration.txt file
	extern int frameSize;
	extern int normalValue;
	extern int noOfSamples ;
	//extern int vars;

#endif
	void calculateCepstrum(char *fullPath, char*fileName, char *outputBase, char *appendFile, FILE *univFP, char*modifiedInputsPath);
		void initCepstrum_CalcuateProcess();