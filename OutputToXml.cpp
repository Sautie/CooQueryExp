
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include "stemming/english_stem.h"


//generating xml files for P values and five top terms, (th=10) with expanded query

using namespace std;


wstring StrToWStr(const string& s)
{
    wstring temp(s.length(),L' ');
    copy(s.begin(), s.end(), temp.begin());
    return temp;
}


string WStrToStr(const wstring& s)
{
    string temp(s.length(), ' ');
    copy(s.begin(), s.end(), temp.begin());
    return temp;
}
bool isNumb(const string& s)
{
    string::const_iterator it = s.begin();
    while ((it != s.end())&&(isdigit(*it))&&((*it)!='-')) ++it;
    return it == s.end();
}

void tokenizer(string chain, vector<string>& NStokens, vector<string>& tokens, bool PortStemming, const string& delim = " ")
{
    int lastPos = chain.find_first_not_of(delim, 0);
    int pos     = chain.find_first_of(delim, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        string tok=chain.substr(lastPos, pos - lastPos);
        NStokens.push_back(tok);
        if (PortStemming) {
              wstring wordl=StrToWStr(tok);
              stemming::english_stem<> StemEnglish;
              StemEnglish(wordl);
              tok=WStrToStr(wordl);
                     }
        if ((not(isNumb(tok)))&&(tok.length()>1)) {
           tokens.push_back(tok);
		                           }
		   lastPos = chain.find_first_not_of(delim, pos);
           pos = chain.find_first_of(delim, lastPos);
		 }

}

int main () {
//    vector<pair<double, string> > P_tij;
//	vector<pair<double, string> > PMI;

	vector< vector<string> > queryMatrix;
	vector< vector<string> > OrigQueryMatrix;
	priority_queue <double> AuxHeap;

	typedef map<string,string> coTerm;
	map<string, coTerm> coTermP;
    map<string, coTerm> coTermPMI;
    string line;
    ostringstream stream;
	vector<string> tokens;
	vector<string> NStokens;

   bool porter=true;
   porter=true;
   string FilePath2="outputP1.txt";
   string firstname="dist15_PMI_top10_200ap";
   string firstname2="dist";
   string  selectS="/";  //"/" for P; "," for PMI


   //to read the xml file for original query in queryMatrix
   cout<<"XML"<<endl;
   string FilePath1="1_50.xml";
   ifstream myfile(FilePath1.c_str());

   if (myfile.is_open())
    {
	   //if (porter)   OutputFile <<"With Porter stemming"<<endl<<endl;
	     while ( getline (myfile,line) )
          {
            int t=line.find("<text>",0);
            if (t!=string::npos) {
              int tt=line.find("</text>",0);
              string textQuery=line.substr(t+6, tt-t-6);
              tokenizer(textQuery, NStokens, tokens, porter);
              for (int i = 0; i < tokens.size(); i++)  {
              transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), ::tolower);       //with/whithout stemming
              //transform(NStokens[i].begin(), NStokens[i].end(), NStokens[i].begin(), ::tolower);  //without stemming
               }
              vector<string> aux;
              for (int j = 0; j < tokens.size(); j++)
			  {
	            aux.push_back(tokens[j]);
			      }
			  queryMatrix.push_back(aux);	                                                    //with/whithout stemming
			  aux.clear();

			  vector<string> aux1;
			   for (int j = 0; j < NStokens.size(); j++)
			  {
	            aux1.push_back(NStokens[j]);
			      }
			  OrigQueryMatrix.push_back(aux1);                                                   //without stemming
			  aux1.clear();

			   tokens.clear();
			  NStokens.clear();
                   }
                        }
          } //myfile
    myfile.close();

     //to read the output file in coTermP for P values

    cout<<"OUTPUT FILE (p values)"<<endl;

    ifstream myfile2(FilePath2.c_str());
    map<string, coTerm>::iterator it1;
	map<string,string>::iterator in_it1;
	string str1, str2;

    bool block=false;
    it1=coTermP.begin();
    int out=0;
    if (myfile2.is_open())  {
		   while ( getline (myfile2,line) )
          {
	        int t, tt, ttt;
			t=line.find("w: ",0);
	        if (t!=string::npos) {
                 str1=line.substr((t+3), (line.length()-t-3));
				 //in_it1=(*it1).second.begin();
                            }
            else if (isdigit(line[0])){
				   t=line.find(")=",0);
				   ttt = line.find_first_of("0123456789", t);
				   tt=line.find(selectS,0);    //"/"
                   str2=line.substr((tt+1), (t-tt-1));                // cooc
                    coTermP[str1][str2]=line.substr(ttt, line.length()-ttt);      //
					cout<<out<<endl;

				  }


		 out++;
                     }
                    }
   myfile2.close();

  //generating NEW xml files  with expanded query for P values and ten top terms, (th=10)

   cout<<"Generating NEW xml files (ten top P values, th=10)"<<endl;

   float uu=0.1;
   while (uu<1) {

   	stream <<uu;

   	FilePath2=firstname +stream.str()+".xml";  //for 10
    ofstream OutputFile(FilePath2.c_str());

    OutputFile<<"<parameters>"<<endl;
    stream.str("");

   	int rr=0;
   	while (rr<queryMatrix.size()) {

   		//#weight(0.5 #combine(foreign communications products) 0.5 #weight(0.03 occu1 0.023 occu2 ……))
   		OutputFile<<"\t<query>"<<endl;
   		OutputFile<<"\t\t<number>"<<rr+1<<"</number>"<<endl;
  		OutputFile<<"\t\t<text>#weight("<<uu<<"#combine(";

   		int yr=0;
   		while(yr<OrigQueryMatrix[rr].size()){
   			OutputFile<<OrigQueryMatrix[rr][yr]<<" ";
   			yr++;
   			 }
		 OutputFile<<") ";
		yr=0;
		bool go=false;
  		while(yr<queryMatrix[rr].size()){
  			for (map<string, coTerm>::iterator itt=coTermP.begin() ; itt != coTermP.end(); itt++ ) {
			 if (queryMatrix[rr][yr]==itt->first){
			 	if (go==false) {OutputFile<<(1-uu)<<"#weight("; }
     	        for(map<string,string>::iterator in_it=(*itt).second.begin(); in_it != (*itt).second.end(); in_it++) {
                	 OutputFile<<" "<<in_it->second<<" "<<in_it->first;
                                             }
                go=true;
                break;
                                   }
                       }
         	  yr++;
         	}

   if (go) OutputFile<<"))</text>"<<endl;
   else    OutputFile<<")</text>"<<endl;
   OutputFile<<"\t</query>"<<endl;
   rr++;
            }

  OutputFile<<"\t<memory>1G</memory>"<<endl;
  OutputFile<<"\t<index>/home/marc/Downloads/indri-5.11/index</index>"<<endl;
  OutputFile<<"\t<trecFormat>true</trecFormat>"<<endl;
  OutputFile<<"\t<count>1000</count>"<<endl;
  OutputFile<<"\t<runID>topics_1_50</runID>"<<endl;
  OutputFile<<"</parameters>"<<endl;

  uu=uu+0.1;
  OutputFile.close();
     }


   /*  //generating xml files for P values and five top terms, (th=10) with expanded query
    cout<<"Generating NEW xml files (five top P values, th=10)"<<endl;

    //if (10<=coTermP[0].size()) {

   uu=0.1;
   while (uu<1) {

   	stream <<uu;

   	FilePath2=firstname2+stream.str()+".xml";  //for 10
    ofstream OutputFile2(FilePath2.c_str());
    OutputFile2<<"<parameters>"<<endl;
    stream.str("");

   	int rr=0;
   	while (rr<queryMatrix.size()) {

   		//#weight(0.5 #combine(foreign communications products) 0.5 #weight(0.03 occu1 0.023 occu2 ……))
   		OutputFile2<<"\t<query>"<<endl;
   		OutputFile2<<"\t\t<number>"<<rr+1<<"</number>"<<endl;
  		OutputFile2<<"\t\t<text>#weight("<<uu<<"#combine(";

   		int yr=0;
   		while(yr<OrigQueryMatrix[rr].size()){
   			OutputFile2<<OrigQueryMatrix[rr][yr]<<" ";
   			yr++;
   			 }
		OutputFile2<<") ";
		yr=0;
		bool go1=false;
  		while(yr<queryMatrix[rr].size()){
  			for (map<string, coTerm>::iterator  itt=coTermP.begin() ; itt != coTermP.end(); itt++ ) {
			 if (queryMatrix[rr][yr]==itt->first){
			 	if (go1==false) OutputFile2<<(1-uu)<<"#weight(";
			 	go1=true;
			 	string S;
			 	double dd;
     	        for(map<string,string>::iterator in_it=(*itt).second.begin(); in_it != (*itt).second.end(); in_it++) {
     	        	            S=in_it->second;
                	            istringstream(S) >> dd;
								AuxHeap.push(dd);
								 //stream.str("");                     //OutputFile2<<" "<<in_it->second<<" "<<in_it->first;
                                    }
				  stringstream strdd;
				  int ct=0;
                  while (!AuxHeap.empty())
                      {
                        dd=AuxHeap.top();
                        strdd <<dd;
						if (ct<5)   {
						 for(map<string,string>::iterator in_it=(*itt).second.begin(); in_it != (*itt).second.end(); in_it++) {
						 	   if (strdd.str()==in_it->second)
						 	         OutputFile2<<" "<<in_it->second<<" "<<in_it->first;
						            }           }
                         ct++;
                         strdd.str("");
                         AuxHeap.pop();
                             }
                   break;
                           }
                       }
         	  yr++;
         	}

   if (go1) OutputFile2<<"))</text>"<<endl;
   else    OutputFile2<<")</text>"<<endl;
   OutputFile2<<"\t</query>"<<endl;
   rr++;
            }

  OutputFile2<<"\t<memory>1G</memory>"<<endl;
  OutputFile2<<"\t<index>/home/marc/Downloads/indri-5.11/index</index>"<<endl;
  OutputFile2<<"\t<trecFormat>true</trecFormat>"<<endl;
  OutputFile2<<"\t<count>1000</count>"<<endl;
  OutputFile2<<"\t<runID>topics_1_50</runID>"<<endl;
  OutputFile2<<"</parameters>"<<endl;

  uu=uu+0.1;
  OutputFile2.close();
     }*/

 }
