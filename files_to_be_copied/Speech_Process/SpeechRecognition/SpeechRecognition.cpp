// SpeechRecognition.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "SpeechRecog.h"

#include "hmmh.h"

#include "cepstrum.h"
#include "kMeans.h"
#include <Windows.h>


#include<process.h>
using namespace std;

//create universe file
void createUniverse(char*inputDirectory, char * outputCSVFolder, char*baseCiPath,char *univFileName, int numItems, char *itemArray[], int numSamples)
{
//	cout << "\n Creating Universe";
	char outputCsv[200];
	sprintf(outputCsv, "%s%s", outputCSVFolder, univFileName);
	FILE *fp = fopen(outputCsv, "w");
	fclose(fp);

	FILE *univFP = NULL;

	char fileName[200];

	initCepstrum_CalcuateProcess();

	for (int i = 0; i <numItems; i++)
	{
		char item[40];
		sprintf(item, "%s", itemArray[i]);
		for (int j = 1; j <=numSamples; j++)
		{
			char tempFileName[30];

			strcpy(tempFileName, item);
			char seqn[4];

			//					strcat(tempFileName,)

			sprintf(seqn, "%d", j);

			//		strcpy(fileName, fileRegex);
			strcpy(fileName, tempFileName);
		//	strcat(fileName, "_");

			strcat(fileName, seqn);

			strcat(fileName, ".txt");
			char fullPath[300];
			strcpy(fullPath, inputDirectory);
			strcat(fullPath, fileName);
			//	cout << fileName<<"\n";

			if (univFP == NULL)
				univFP = fopen(outputCsv, "a");
			if (univFP==NULL)
				cout << univFP;
		//	perror("Error");
			char modifiedInputs[200];
			sprintf(modifiedInputs,"%s%s",inputDirectory,"modified\\" );
			
			calculateCepstrum(fullPath, fileName, baseCiPath, outputCsv, univFP,modifiedInputs);

		}
	}


}

//to create observatio sequences from Ci's
void prepareObservations(char*inputsDirRegex, char*baseObs, int numItems, int numSamples, char*itemsArray[], char *codebookBase)
{

//	cout << "\n Creating Observation Files";
	char *outputBase = baseObs;
	readCodeBook(codebookBase);
	char fileName[200];
	for (int i = 0; i <numItems; i++)
	{
		char item[40];
		sprintf(item, "%s", itemsArray[i]);
		for (int j = 1; j <=numSamples; j++)
		{
			char tempFileName[100];

			strcpy(tempFileName, item);
			char seqn[4];
			strcpy(fileName, tempFileName);
			sprintf(seqn, "%d", j);

//			strcat(fileName, "_");
			strcat(fileName, seqn);
			strcat(fileName, ".txt");

			char fullPath[200];
			strcpy(fullPath, inputsDirRegex);
			strcat(fullPath, fileName);

			createObsFiles(fullPath, fileName,outputBase);
		}
	}
}


void pressSpaceKeyAndRelease()
{


	INPUT space = { 0 };
	space.type = INPUT_KEYBOARD;
	space.ki.wVk = VK_SPACE;
	SendInput(1, &space, sizeof(INPUT)); // Send KeyDown
	space.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &space, sizeof(INPUT)); // Send KeyUp

}

PROCESS_INFORMATION pi;

void threadFunction(void *param)
{
	//cout << "In thread function" << endl;
	Sleep(5000); //send key after 4 seconds max
	//cout << "Thread function ends" << endl;
	pressSpaceKeyAndRelease();
	TerminateProcess(pi.hProcess, 1);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	_endthread();
}

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

//returns Saved Path
char * recordSpeechAndSave(char*fileName, char*basePath, int recNum)

{
	char *fullWavePath = (char*)malloc(sizeof(char)* 200);
	sprintf(fullWavePath, "%s%s%d%s", basePath, fileName, recNum, ".txt");

	int duration = 2;
	char *tempWavPath = (char*)malloc(sizeof(char)* 200);
	strcpy(tempWavPath, "..//Debug//Output//temp//tempWavRecord.wav");

	//	strcpy(tempWavPath, "a.wav");

	char *tempTextPath = (char*)malloc(sizeof(char)* 200);
	strcpy(tempTextPath, "b.wav");

	char commandString[500];

	sprintf_s(commandString, "%s %d %s %s", "Recording_Module.exe ", duration, tempWavPath, fullWavePath); // prepare command
	//system("exetest.exe");
	//system("dir");
	_beginthread(threadFunction, 0, NULL);

	//system(commandString); // Call recording module.

	STARTUPINFO si;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	wchar_t *ptr = convertCharArrayToLPCWSTR(commandString);

	//strcpy(commandString, "Recording_Module.exe 2 a.wav b.txt");
	//ptr = convertCharArrayToLPCWSTR(commandString);

	//strcpy(commandString, "Recording_Module.exe");
	if (!CreateProcess(NULL,   // No module name (use command line)
		ptr,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return "";
	}




	return fullWavePath;
}


char * oldRecordSpeechAndSave(char*fileName, char*basePath, int recNum)

{
	char *fullWavePath = (char*)malloc(sizeof(char)* 200);
	sprintf(fullWavePath, "%s%s%d%s", basePath, fileName, recNum, ".txt");

	int duration = 2;
	char *tempWavPath = (char*)malloc(sizeof(char)* 200);
	sprintf(tempWavPath, "%s%s%d%s", basePath, fileName, recNum,".wav");
//	strcpy(tempWavPath, basePath);
	//strcat(tempWavPath, fileName);


	//	strcpy(tempWavPath, "a.wav");

	//char *tempTextPath = (char*)malloc(sizeof(char)* 200);
//	strcpy(tempTextPath, "b.wav");

	char commandString[500];

	sprintf_s(commandString, "%s %d %s %s", "Recording_Module.exe ", duration, tempWavPath, fullWavePath); // prepare command
	//system("exetest.exe");
	//system("dir");
//	_beginthread(threadFunction, 0, NULL);

	system(commandString); // Call recording module.

	return fullWavePath;
}

//return -1 if folder already exists else return 1 if folder created successfuly
int CreateFolder(const char * path)
{
	if (!CreateDirectory(convertCharArrayToLPCWSTR(path), NULL))
	{
		return 1 ;
	}

	return -1;
}

void createRecordings(char *itemName)
{

	for (int i = 1; i < 35 && printf("\n ---Iter %d \n\n",i);i++ )
		char *path = oldRecordSpeechAndSave(itemName, "..\\Debug\\Input\\soundSahil\\", i);
	return;

}

//char *items[] = { , "cookie",, , "watch", "search" };
char *items[] = { "jeans", "Syrup", "watch", "phone" ,"battery"};//, ",, };


int main(int argc, char** argv)
{
	//cout << "Hello";

	//return 0;
	//cout << "a";
	int numItems = sizeof(items) / sizeof(items[0]);
	const int modeTest = 5;
	const int modeRecord = 10;
	const int modeTrainRecorded = 11;

//	if (argc <= 2)
	//	cout << "-1";

	
//	else

	{
	//	for (int i = 0; i < argc; i++)
//			cout <<"\nitem"<<i <<" "  <<argv[i];
		char user[100];
		strcpy(user, argv[2]);
		//strcpy(user, "user");
		
		char recNum[10];
		if (argc>=5)
			strcpy(recNum, argv[4]);	

	//	strcpy(recNum, "1");
		int recNo = atoi(recNum);
		int mode = atoi(argv[1]);
		
	//	int mode = modeTrainRecorded;
		//strcpy(user, argv[2]);	
		char word[100];
		if (argc>=4)
			strcpy(word, argv[3]);
		//if (argc==4)
			//strcpy(word, argv[3]);

		char *path = (char*)malloc(sizeof(char)* 200);
		char * Cipath = (char*)malloc(sizeof(char)* 200);
		char * obsPath = (char*)malloc(sizeof(char)* 200);
		int gotThis = 0;
		FILE* red = NULL;
		FILE * ctt = NULL;
		int ca = 0;
		char baseTemp[100];
		sprintf(baseTemp, "%s%s%s", "..\\Debug\\", user, "\\Output\\temp\\");
		char basehmmOutputs[100];
		sprintf(basehmmOutputs, "%s%s%s", "..\\Debug\\", user, "\\Output\\hmmOutputs\\");

		char baseCis[100];
		sprintf(baseCis, "%s%s%s", "..\\Debug\\", user, "\\Output\\Cis\\");

		char baseObs[100];
		sprintf(baseObs, "%s%s%s", "..\\Debug\\", user, "\\Output\\obs\\");
			

		char baseSound[100];
		sprintf(baseSound, "%s%s%s", "..\\Debug\\", user, "\\Input\\sound\\");


		char baseOutput[200];
		sprintf(baseOutput, "%s%s%s", "..\\Debug\\", user, "\\Output\\");

		char userDirectory1[200];
		char userInputDirectory[200];
		char userInputSoundDirectory[200];
		char userModifiedInputDirectory[200];


		char tempPath[150];
		switch (mode)
		{
		case modeTest:
			CreateFolder(baseTemp);

			recordSpeechAndSave("tempCi", baseTemp, 1);
			Sleep(5500);
			sprintf(path, "%s%s", baseTemp, "tempCi1.txt");
				//path = "..\\Debug\\Output\\temp\\tempCi1.txt";
			//while (ca < 1000000000)
		
			initCepstrum_CalcuateProcess();
		//	sprintf(tempPath,"%s%s", baseTemp, "tempCi.txt"); 
			calculateCepstrum(path, "tempCi.txt", baseTemp, "", NULL,baseTemp);
			readCodeBook(baseOutput);
			sprintf(Cipath,"%s%s", baseTemp, "tempCi.txt");

			//Cipath = "..\\Debug\\output\\temp\\tempCi.txt";
			sprintf(obsPath,"%s%s", baseTemp, "tempObs.txt");

		//	obsPath = "..\\Debug\\output\\temp\\tempObs.txt";
			createObsFiles(Cipath, "tempObservation.txt", baseTemp);
			
			initTestSpeech(basehmmOutputs, numItems, items);
			gotThis = tellWhatMySoundIs(baseTemp, "tempObservation.txt", numItems, items);
			//ctt= fopen("CON", "w");
			// red = freopen("CON", "w", stdout); 
			// fclose(ctt);

			cout << gotThis;

			break; 

		case modeRecord:
			
			sprintf(userDirectory1, "%s%s%s", "..\\Debug\\", user, "\\");
			CreateFolder(userDirectory1);

			sprintf(userInputDirectory, "%s%s", userDirectory1, "Input\\");
			CreateFolder(userInputDirectory);

			sprintf(userInputSoundDirectory, "%s%s", userInputDirectory, "sound\\");
			CreateFolder(userInputSoundDirectory);

			sprintf(userModifiedInputDirectory, "%s%s", userInputSoundDirectory, "Modified\\");
			CreateFolder(userModifiedInputDirectory);
			recordSpeechAndSave(word, userInputSoundDirectory, recNo);
			Sleep(5500);
			cout << word << recNo;

			break;
			 
		case modeTrainRecorded:

			char *InputSoundDirectory = baseSound;
			char*outputUniverseCSV = baseOutput;
			char univName[200];
			sprintf(univName, "%s", "Universe.csv");
			char userDirectory[200];
			sprintf(userDirectory, "%s%s%s", "..\\Debug\\", user, "\\");
			CreateFolder(userDirectory);
			CreateFolder(baseOutput);
			

			CreateFolder(baseObs);

			CreateFolder(baseCis);
			CreateFolder(basehmmOutputs);
	//	createRecordings("syrup");



			createUniverse(InputSoundDirectory, outputUniverseCSV, baseCis, univName, numItems, items, 30);
	
			createCodeBook(baseOutput);// also calc Cis
		
			char *hmmRegex =baseCis;
			//hmm regex is cis File
			prepareObservations(hmmRegex, baseObs, numItems, 30, items, baseOutput);
	
			trainHMMData(baseObs,basehmmOutputs, items, numItems, 20);
			initTestSpeech(basehmmOutputs, numItems, items);
		

			testSpeech(baseObs,baseOutput, numItems, items);	
			cout << "TRAINING DONE";
			char dummyFileLoc[100];
			sprintf(dummyFileLoc,"%s%s",baseOutput,"dummy.txt");
			FILE *dummy = fopen(dummyFileLoc, "w");
			fprintf(dummy, "%s", "1");
			fclose(dummy);
			break;


		}


	}
	
		/*	char *InputDirectory = "input\\sound\\";
	char*outputUniverseCSV = "Output\\Universe.csv";

	//createUniverse(InputDirectory, outputUniverseCSV,8,items,30);

	
//createCodeBook("Output\\Universe.csv");// also calc Cis
	char *hmmRegex = "Output\\cis\\";
	//prepareObservations(hmmRegex,8,30,items);

	//trainHMMData("Output\\obs\\",items,8,20);
	
	initTestSpeech("Output\\hmmOutputs\\",8,items);

	testSpeech("Output\\obs\\", 8,items);
	*/
//	testSpeech("Output\\obs\\", "154101019");
	
	//cout << gotThis;
	/*
	int choice = 5;

	switch (choice)
	{

	case 1:
	//	cout << "yes"; 
	//	getchar();
		//cout << "y2es";
		//cout << "yo";
		//cout << "Start Speakinsg";
		//red = freopen("file.txt", "w", stdout);
		
		/*recordSpeechAndSave("tempCi","..\\Debug\\Output\\temp\\",1);
		Sleep(5500);
		path = "..\\Debug\\Output\\temp\\tempCi1.txt";
		//while (ca < 1000000000)
		{
		//	ca++;
		}
		initCepstrum_CalcuateProcess(); 
		calculateCepstrum(path,"tempCi.txt","..\\Debug\\Output\\temp\\","",NULL);
		readCodeBook();
		Cipath = "..\\Debug\\output\\temp\\tempCi.txt";
		obsPath = "..\\Debug\\output\\temp\\tempObs.txt";
		createObsFiles(Cipath, "tempObservation.txt", "..\\Debug\\Output\\temp\\");
		initTestSpeech("..\\Debug\\Output\\hmmOutputs\\", numItems, items);
		gotThis = tellWhatMySoundIs("..\\Debug\\Output\\temp\\", "tempObservation.txt", numItems, items);
		//ctt= fopen("CON", "w");
		// red = freopen("CON", "w", stdout);
		// fclose(ctt);
		
		cout<< gotThis;
		*/
	//	cout << "random";

	//	int k;
		//cin >> k; 
		
/*		break;


	case 2:
		createRecordings("syrup");
		break;
	
	case 3:
		break;
		//prepare
		char *InputDirectory = "..\\Debug\\input\\sound\\"; 
		char*outputUniverseCSV = "..\\Debug\\Output\\Universe.csv";
		createUniverse(InputDirectory, outputUniverseCSV, numItems, items, 30);


		createCodeBook("..\\Debug\\Output\\Universe.csv");// also calc Cis
		char *hmmRegex = "..\\Debug\\Output\\cis\\";
		prepareObservations(hmmRegex,numItems,30,items);

		trainHMMData("..\\Debug\\Output\\obs\\", items, numItems, 20);

		initTestSpeech("..\\Debug\\Output\\hmmOutputs\\", numItems, items);

		testSpeech("..\\Debug\\Output\\obs\\", numItems, items);
		break;
		

	
 		
	}
	*/
	//cout << 3;
		//int k;
	//cin>>k;
	
	return 0;
}

