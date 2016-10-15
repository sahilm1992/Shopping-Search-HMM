#include "stdafx.h"
/*
#ifndef hmm
#define hmm
//variables

#endif
*/
long double hmmModelling(char* obFilefullPath, char* obFileName);

void createObsFiles(char*fullPath, char*fileName);
void readCodeBook(char*baseCodeBook);

void trainHMMData(char*inputDirRegex, char*baseHMMOutputs, char*itemsArray[], int numItems, int trainSamples);
int initTestSpeech(char *fullPath, int numItems, char *itemsArray[]);

int testSpeech(char*inputRegexPathObs, char*baseOutput, int numItems, char *itemArray[]);

void calculateCepstrum(char *fullPath, char*fileName, char *outputBase, char *appendFile, FILE *univFP, char*modifiedInputsPath);


int tellWhatMySoundIs(char *basePath, char *fileName, int numItems, char *itemArray[]);
void createObsFiles(char* fullPath, char*fileName, char*baseOutput);