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
void sortDictbyWord();
void sortDictbyCount();
void merge();
void merge2();
void toLower(string &s);
void filter(string &s);
unsigned  WINAPI secondThreadFunc(void* argu) {  
	char* filename=(char*)argu;
    fstream  file ;
    string s;
    int pos;
    file.open(filename,ios::in);  
	if(!file) {
		WaitForSingleObject( hResourceMutex, INFINITE );//wait and lock vector dict
        cerr << "Can't open file "<<filename<<endl;
        exit(1);     //在不正常情形下，中斷程式的執行
        ReleaseMutex( hResourceMutex );					//release vector dict
    } 
    while(!file.eof( )){
		file>>s;toLower(s);filter(s);
		if(!s.empty()){
			WaitForSingleObject( hResourceMutex, INFINITE );//wait and lock vector dict
	    	dict.push_back(wordRecord(s));
	    	ReleaseMutex( hResourceMutex );					//release vector dict
		}
	    
	}
   
    file.close();
    _endthreadex(0);          
    return 1;  
}  

int main(int argc, char* argv[]) {  
	string outName="out(";
	for(int i=1;i<argc;i++) {
		if(i<argc-1){outName+=argv[i];outName+=" ";}
		else outName+=argv[i];
	}
	outName+=").txt";
    HANDLE hThread[MAX_THREADS];   
    hResourceMutex=CreateMutex(NULL,FALSE,NULL);
    fstream  file ;
    file.open(outName,ios::out);
    if(!file) {
        cerr << "Can't open file! "<<outName<<endl;
        exit(1);     //在不正常情形下，中斷程式的執行
    }
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
    sortDictbyWord();
    cout<<"start merging..."<<endl;
    merge();
    cout<<"start sorting by count..."<<endl;
    sortDictbyCount();
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
void sortDictbyWord(){
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
void sortDictbyCount(){
	int pos,current;
	wordRecord tmp("");
	for(int i=1;i<dict.size();i++){
		tmp=dict[i];
		for(int j=i-1;j>=0;j--){
			if(dict[j].count<tmp.count&&j==0) {
				dict[j+1]=dict[j];
				dict[j]=tmp;
			}
			else if(dict[j].count<tmp.count) dict[j+1]=dict[j];
			else {
				dict[j+1]=tmp;
				break;	
			}
		}
		
	}
}
void toLower(string &s){
	for(int i=0;i<s.size();i++){
		if(s.at(i)<='Z'&&s.at(i)>='A') s[i]=s.at(i)-('A'-'a');
	}
}
void merge(){
	vector<wordRecord>::iterator it_start;
	vector<wordRecord>::iterator it_end;
	vector<wordRecord>::iterator it_current;
	for(it_current=dict.begin();it_current<dict.end();){
		it_end=it_current;
		it_start=it_current+1;
		while((it_end+1!=dict.end())&&(*(it_end+1)).word.compare((*it_current).word)==0){
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
			//it_current--;
		}
		else it_current++;
	}
	
}
//void merge2(){
//	vector<wordRecord>::iterator it_start;
//	vector<wordRecord>::iterator it_end;
//	vector<wordRecord>::iterator it_current;
//	for(it_current=dict.rbegin();it_current!=dict.rend();){
//		it_end=it_current;
//		it_start=it_current;
//		while((*(it_end-1)).word.compare((*it_current).word)==0){
//			it_end--;
//		}
//		if(it_end-it_start==1){
//			it_current=it_end-1;
//			(*it_end).count++;
//			dict.erase(it_start);
//		}
//		else if(it_end-it_start>0){
//			it_current=it_end-1;
//			(*it_end).count+=it_end-it_start-1;
//			dict.erase(++it_end,it_start);
//			//it_current--;
//		}
//		else it_current--;
//	}
//	
//}
void filter(string &s){
	string::iterator  it= s.end()-1;
	while(((*it)>'z'||(*it)<'a')&& it>=s.begin()){
		//cout<<"char="<<*it<<endl; 
		it--;
	}
	if(it<s.begin())s.clear();
	else if(it==s.end()-2){
		//cout<<s<<endl;
		s.erase(it+1);
	}
	//else if((s.end()-1==s.begin())&&(*s.begin()>'z'||*s.begin()<'a')) s.clear();
	else if (it!=(s.end()-1)) s.erase(it+1,s.end());
	
}
