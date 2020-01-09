#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <ctype.h>
#include <set>
#include <map>
#include <vector>
#include <Windows.h>
#include <ctime>
#include <iomanip>
#include <math.h>
#include "stemming/english_stem.h"

#include "QueryExpansion.h"
using namespace std;


int main () {

   vector <string> FileList;
  set<string> stoplist;


  map<string, int> termFreq;
  map<string, coTerm> coTerms, coTerms1;
  set<pair<string, int>, cmp>  termFreqSF;

  ifstream infile ("stop_words.txt");
  stoplist= ReadStopList(infile);

  //quelques parametres utilises par les fonctions, document_parser et OutFile

  int seuil=10,dist=15;
  int cont=10;
  bool boolseuil=true;
  bool bothSide=false;
  bool Porter=false;
  string FilePath="outputT.txt";
  string FName="AP1";

  read_folder(FName, FileList);
  ifstream myfile;
  string filen;
  int Lfl=FileList.size();
  vector<int> pp(2, 0);
  	clock_t begin = clock();
  	int y=0;

  //la boucle pour parcourir tous les fichiers AP qui se trouvent
  //dans le dossier AP2

  for (int f=2;f<Lfl;f++)  {
    filen=FName+"/"+FileList[f];
   	myfile.open(filen.c_str(),ios::in);
   	if (myfile.is_open()) {
   	   cout<<(f-1)<<" "<<FileList[f]<<endl;
       document_parser(myfile, termFreq, termFreqSF, coTerms, pp, stoplist, seuil, dist,Porter,boolseuil, bothSide);
       myfile.close();
                          }
  else cout << "Unable to open file: "<<FileList[f]<<endl;
  y=y+1;
  if (y%200==0) {               //chaque 200 fichiers on obtient un fichier de sortie
  coTerms1=coTermFilter (coTerms, seuil, boolseuil);
  ofstream OutputFile(FilePath.c_str());
  OutFile(y,coTerms1, termFreqSF, pp, OutputFile);
           }

   }
  clock_t end = clock();       //Pour obtenir le fichier de sortie final ainsi que le temps total de calcul approximatif
  double time1 = double(end - begin) / CLOCKS_PER_SEC;
  cout<<"Temps1(freq,cofreq):"<<time1<<endl;
  string FilePath2="output1.txt";
  begin = clock();
  coTerms1=coTermFilter (coTerms, seuil, boolseuil);
  ofstream OutputFile(FilePath2.c_str());
  OutFile(y,coTerms1, termFreqSF, pp, OutputFile);
  end = clock();
  double time2 = double(end - begin) / CLOCKS_PER_SEC;
  cout<<"Temps2(PMI, P(Tj/Tj), sorting freq...):"<<time2<<endl;

  return 0;
}
/*
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
     }*/
