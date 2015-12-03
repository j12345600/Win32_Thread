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
    	count=1; 
	}
};
struct Range {
	int start, end;
	Range(int s = 0, int e = 0) {start = s, end = e;}
};

HANDLE  hResourceMutex;
vector<wordRecord> dict;
int search(wordRecord wR);
void sortDict();
unsigned  WINAPI secondThreadFunc(void* argu) {  
	char* filename=(char*)argu;
    fstream  file ;
    string s;
    int pos;
    file.open(filename,ios::in);   
    while(!file.eof( )){
	    WaitForSingleObject( hResourceMutex, INFINITE );//wait and lock vector dict
	    file>>s;
		dict.push_back(wordRecord(s));
	    ReleaseMutex( hResourceMutex );					//release vector dict
	}
   
    file.close();
    _endthreadex(0);          
    return 1;  
}  
void testFunc(){
	vector<wordRecord>::iterator it_start;
	vector<wordRecord>::iterator it_end;
	vector<wordRecord>::iterator it_current;
	for(it_current=dict.begin();it_current<dict.end();it_current++){
		it_end=it_current;
		it_start=it_current+1;
		while((*(it_end+1)).word.compare((*it_current).word)==0){
			it_end++;
		}
		if(it_end-it_start==0){
			(*it_current).count++;
			it_current=dict.erase(it_start);
		}
		else if(it_end-it_start>0){
			it_end++;
			(*it_current).count+=it_end-it_start;
			it_current=dict.erase(it_start,it_end);
			it_current--;
		}
	}
	
}

int main(int argc, char* argv[]) {  

    HANDLE hThread[MAX_THREADS];   
    hResourceMutex=CreateMutex(NULL,FALSE,NULL);
    fstream  file ;
    file.open("output.txt",ios::out);
    for(int i=0;i<argc-1&&i<MAX_THREADS;i++) {
		hThread[i] = (HANDLE)_beginthreadex(NULL,				//security
											0,					//stack
		  									secondThreadFunc,	//subProgram
		  									argv[i+1],			//Address of Parameter
		   									0,					//initialFlag
											NULL); 				//ThrAddress
	}
    	
										
    for(int i=0;i<argc-1&&i<MAX_THREADS;i++) 
		WaitForSingleObject(hThread[i], INFINITE); 
    cout<<"start sorting..."<<endl;
    sortDict();
    testFunc();
    for(int i=0;i<argc-1;i++) CloseHandle(hThread[i]);  
    for(int i=0;i<dict.size();i++) 
		file<<dict[i].word<<" "<<dict[i].count<<endl;
	file.close();
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
	int len=dict.size();
	if (len <= 0) return; //避免len等於負值時宣告堆疊陣列當機
	//r[]模擬堆疊,p為數量,r[p++]為push,r[--p]為pop且取得元素
	Range r[len]; int p = 0;
	r[p++] = Range(0, len - 1);
	while (p) {
		Range range = r[--p];
		if(range.start >= range.end) continue;
		string mid = dict[range.end].word;
		int left = range.start, right = range.end - 1;
		while (left < right) {
			while (dict[left].word.compare(mid) < 0 && left < right) left++;
			while (dict[right].word.compare(mid) >= 0 && left < right) right--;
			std::swap(dict[left], dict[right]);
		}
		if (dict[left].word.compare(dict[range.end].word) >= 0)
			std::swap(dict[left], dict[range.end]);
		else
			left++;
		r[p++] = Range(range.start, left - 1);
		r[p++] = Range(left + 1, range.end);
	}
}



