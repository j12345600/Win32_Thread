#include <windows.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <vector>
#include <iostream>
#include <fstream>
#define MAX_THREADS 3
using namespace std;
struct wordRecord {
   	string word;
    int count;
    wordRecord(string s){
    	word=s;
    	count=0; 
	}
};
HANDLE  hResourceMutex;
vector<wordRecord> dict;

unsigned  WINAPI secondThreadFunc(void* argu) {  
	char* filename=(char*)argu;
    fstream  file ;
    string s;
    file.open(filename,ios::in);   
    while(!file.eof( )){
    WaitForSingleObject( hResourceMutex, INFINITE );
    file>>s;
    
    dict.push_back(wordRecord(s));
    
    ReleaseMutex( hResourceMutex );
	}
   
    file.close();
    _endthreadex(0);          
    return 1;  
}  

int main(int argc, char* argv[]) {  

//	_beginthreadex.exe gcc.txt bash.txt
//	argc=MAX_THREADS;
//	argv[1]="gcc.txt";
//	argv[2]="bash.txt";
    HANDLE hThread[MAX_THREADS];   
    hResourceMutex=CreateMutex(NULL,FALSE,NULL);
    

    for(int i=0;i<argc-1;i++) {
		hThread[i] = (HANDLE)_beginthreadex(NULL,				//security
											0,					//stack
		  									secondThreadFunc,	//subProgram
		  									argv[i+1],			//Address of Parameter
		   									0,					//initialFlag
											NULL); 				//ThrAddress
	}
    	
										
    for(int i=0;i<argc-1;i++) WaitForSingleObject(hThread[i], INFINITE);  
    
   // printf("Counter should be %d, it is %d now!\nThreadID=%i", icounterMax, icounter,threadID);  
    for(int i=0;i<argc-1;i++) CloseHandle(hThread[i]);  
    for(int i=0;i<dict.size()&&i<100;i++) cout<<dict[i].word<<" ";
    return 1;
}  
