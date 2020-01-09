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
