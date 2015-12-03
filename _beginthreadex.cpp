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
int search(wordRecord wR);
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
    vector<string> str;
    str.push_back("abc");
    str.push_back("def");
    for(int i=0;i<str.size();i++) cout<<str[i];
    cout<<endl;
    swap(str[0],str[1]);
    for(int i=0;i<str.size();i++) cout<<str[i];
	//cout<<(5-4)%2;
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
int search(wordRecord wR){
	int start=0, end=dict.size()-1;
	int mid,compareResult;
	while (start<=end){
		mid = (start + end) / 2;
		compareResult=dict[mid].word.compare(wR.word);
        if (compareResult==0 )
        {
            return mid;
        }
        else if (compareResult > 0)
        {
            end = mid - 1;
        }
        else if (compareResult < 0)
        {
            start = mid + 1;
        }
	}
	return -1;
}
void sortDict(){
	
}
