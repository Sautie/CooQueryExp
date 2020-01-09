#include "QueryExpansion.h"

//1 pour ouvrir les dossiers en Windows
void read_folder(const string& name, vector <string> & list)
{
    string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            list.push_back(data.cFileName);
			//cout<<data.cFileName<<endl;
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
}

//2 fonction de conversion string vers wstring
wstring StrToWStr(const string& s)
{
    wstring temp(s.length(),L' ');
    copy(s.begin(), s.end(), temp.begin());
    return temp;
}

//3 fonction de conversion wstring vers string
string WStrToStr(const wstring& s)
{
    string temp(s.length(), ' ');
    copy(s.begin(), s.end(), temp.begin());
    return temp;
}
//4 fonction pour lire la liste de stop words et la garder dans le set stoplist
set<string> ReadStopList(ifstream &infile)  {
    string line;
    set<string> stoplist;
	// int bout=0;
     while ( getline (infile,line) ){
     	 stoplist.insert(line);
		  		}
     return stoplist;
     			}

 //5 fonction pour trier le dictionnaire des termes par ordre decroissant des frequences
 set<pairSI, cmp> SortDictionary (map<string, int>& termFreq, int seuil, bool bseuil) {
   set<pair<string, int>, cmp>::iterator t;
   int c=0;
   set<pairSI, cmp> pret(termFreq.begin(), termFreq.end());
   if (bseuil){
	for (t = pret.begin(); t != pret.end(); ++t) {
	    if (t->second<seuil) {
         pret.erase (t, pret.end());
         break;
		 }
        }
	        }
  return pret;
  }
//6 fonction pour obtenir le dictionnaire de with (default)/without stoplist
void dictionary(vector <int>& outp,  vector<string>& tokens, map<string, int>& termFreq, const set<string>& stoplist = set<string>()) {   //with/without arg stoplist to eliminate/to add the stop words
    int i=0,s=0;
    while (i<tokens.size()){
	   transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), ::tolower);
       if (stoplist.find(tokens[i]) == stoplist.end()) {
	            ++termFreq[tokens[i]];
	            s++;
	                    }
				   	i++;
	   	        }
	   	outp[0]=outp[0]+i;    //total word count in sections corresponding to all chosen tags
	   	outp[1]=outp[1]+s;    //total word count in sections corresponding to all chosen tags, excluding the stop words, and one-letter words
	   	//outp[2]=outp[2]+termFreq.size();
    	}

//7 fonction pour afficher le dictionnaire
void printDict (map<string, int> termFreqq) {
	map<string, int>::iterator tt;
    for (tt = termFreqq.begin(); tt != termFreqq.end(); ++tt) {
         cout <<tt->first<<": "<<tt->second<<endl;
        }
	}
//8 fonction pour afficher le dictionnaire trie
void printSDict (set<pair<string, int>, cmp>  SortedtermFreq, vector <int> pp) {
	set<pair<string, int>, cmp>::iterator t;
	for (t = SortedtermFreq.begin(); t != SortedtermFreq.end(); ++t) {
         cout << t->first <<": "<< t->second<<endl;
        }
    cout<<"Total: "<<pp[0]<<" Total-StopWords: "<<pp[1]<<endl;
  }
  //9 fonction pour determiner la frequence de coocurrences des mots
  void cofreq2 (map<string, coTerm>& coTermFreq, vector<string>& tokens,  set<pairSI, cmp>& termFreqq, const set<string>& stoplist = set<string>(), int dist=15, bool bothSide=false) {

    set<pairSI, cmp>::iterator it;

    int i=0, k=1, hh;
    while (i<tokens.size()){
	     it = termFreqq.begin();
        while ((it != termFreqq.end())&&(it->first!=tokens[i])) ++it;
        //h=distance(termFreqq.begin(),it);
		//if ((it->second)>seuil){
        k=1;
        if (it != termFreqq.end()) {
        while (((i+k)<tokens.size())&&(k<=dist)){
          if ((tokens[i]!=tokens[i+k])&&(stoplist.find(tokens[i+k]) == stoplist.end()))
		  ++coTermFreq [tokens[i]][tokens[i+k]];
          k++;
           }

        if (bothSide) {
		k=1;
        while (((i-k)>=0)&&(k<=dist)){
          if ((tokens[i]!=tokens[i-k])&&(stoplist.find(tokens[i-k]) == stoplist.end()))
		  ++coTermFreq [tokens[i]][tokens[i-k]];
          k++;
           }
       }
         // }
          }
   i++;
     }

	}
	  //10 fonction pour tamiser les coocurrences dont les frequences se trouvent au dessus de seuil (10 par default)
	map<string, coTerm> coTermFilter (map<string, coTerm>& coTermFreq, int seuil=10, bool bseuil=true) {
		map<string, coTerm> coTermFreq1;
		map<string, coTerm>::iterator itt;
	    map<string,int>::iterator in_it;
	     if (bseuil){
	    for (itt=coTermFreq.begin() ; itt != coTermFreq.end(); itt++ ) {
	        for( in_it=(*itt).second.begin(); in_it != (*itt).second.end(); in_it++){
	        	if (in_it->second>=seuil) {
	        		coTermFreq1[itt->first][in_it->first]=in_it->second;
	        	  }
	        	 }
	        	  }
	        	  }
	    else {
	          coTermFreq1=coTermFreq;
	        	  }

	 return coTermFreq1;
	}

//11 fonction pour obtenir calculer les PMI et les P et obtenir fichiers de sortie
void OutFile(int uy, map<string, coTerm> coTermFreq, set<pair<string, int>, cmp>  SortedtermFreq, vector <int> pp, ofstream &OutputFile,  int cont=10, int seuil=10, bool porter=false)
{
    set<pair<string, int>, cmp>::iterator t;
    map<string, coTerm>::iterator it;
    map<string,int>::iterator in_it;

    if (OutputFile)
    {
	      //if (porter)   OutputFile <<"With Porter stemming"<<endl<<endl;

          // OutputFile << "Term occurrence frequency:"<<endl<<endl;
		//for (t = SortedtermFreq.begin(); t != SortedtermFreq.end(); ++t)
		//   OutputFile << t->first <<": "<< t->second<<endl;
	    /*for (t = SortedtermFreq.begin(); t != SortedtermFreq.end(); ++t) {
	    if (t->second>seuil)
         OutputFile << t->first <<": "<< t->second<<endl;
		 else break;
        } */
         /*   OutputFile <<endl<<  "Term co-occurrence frequency:"<<endl<<endl;
        for ( it=coTermFreq.begin() ; it != coTermFreq.end(); it++ ) {
          for( in_it=(*it).second.begin(); in_it != (*it).second.end(); in_it++)
            OutputFile<<"(" << (*it).first <<", "<< in_it->first <<"): "<< in_it->second << endl;
                               } */
		//OutputFile << "Term cooccurrence frequency:"<<endl<<endl;

	    vector<pair<double, string> > P_tij;
	    vector<pair<double, string> > PMI;

	    int L=SortedtermFreq.size();
	    string str1, str2, str3, str4;
	    double pt, pm;

		for (t = SortedtermFreq.begin(); t != SortedtermFreq.end(); ++t) {
		for ( it=coTermFreq.begin() ; it != coTermFreq.end(); it++ ) {
		    if (t->first==it->first){
		    str1=(t->first)+"/";    //
		    str3=(t->first)+",";     //
            for(in_it=(*it).second.begin(); in_it != (*it).second.end(); in_it++){
            	 str2=str1+(in_it->first);
            	 str4=str3+(in_it->first);
            	 pt=((double)in_it->second)/((double)t->second);  //P(Ti/Tj)
            	 P_tij.push_back(make_pair(pt, str2));

            	 set<pair<string, int>, cmp>::iterator t2;
            	 t2 = SortedtermFreq.begin();
            	 while((t2 != SortedtermFreq.end())&&(t2->first!=in_it->first)) t2++;
            	 if (t2 != SortedtermFreq.end()) pm=log((((double)in_it->second)*((double)pp[1]))/(((double)t->second)*((double)t2->second))); //PMI
			     PMI.push_back(make_pair(pm, str4));
				 }
                   }
                   	}
		    }
		sort(P_tij.rbegin(),P_tij.rend());
		sort(PMI.rbegin(),PMI.rend());

		OutputFile <<endl<<endl<< "P(Ti/Tj):"<<endl<<endl;
		vector<pair<double, string> >::iterator itp;

		int r=0, rr=0;
	   for (t = SortedtermFreq.begin(); t != SortedtermFreq.end(); ++t) {  //terms sorted and filtered by threshold=10

		    str1=t->first+"/"; r=0;
			OutputFile<<"w: "<<t->first<<endl;
		for (itp = P_tij.begin(); itp != P_tij.end(); ++itp) {
			if (((*itp).second.substr(0, str1.length())) ==(str1)){
			 OutputFile<<r<<" P("<<itp->second<<")= ";
			 OutputFile<<setw(10)<<itp->first<<endl;
			  r++;
		     }
		      if (r==(cont)) break;
		     }
		   }
		OutputFile <<endl<<endl<< "PMI(Ti,Tj):"<<endl<<endl;
		vector<pair<double, string> >::iterator itm;
		r=0; rr=0;
		for (t = SortedtermFreq.begin(); t != SortedtermFreq.end(); ++t) {
	        str1=t->first+","; r=0;
	      	OutputFile<<"w: "<<t->first<<endl;
		for (itm = PMI.begin(); itm != PMI.end(); ++itm) {
			if (((*itm).second.substr(0, str1.length())) ==(str1)){
			OutputFile<<r<<" PMI("<<itm->second<<")= ";
			OutputFile<<setw(10)<<itm->first<<endl;
			r++;
			   }
			   if (r==(cont)) break;
		     }
		 }

        OutputFile<<endl<<"Number of documents: "<<uy<<" Number of words: "<<pp[0]<<" Number of words excluding Stop Words: "<<pp[1]<<" Vocabulary (threshold=10, whithout stop words): "<<SortedtermFreq.size()<<endl;
        OutputFile.close();
    }
}
//12 fonction pour afficher les coocurrences
void printCoDict (map<string, coTerm> coTermFreq) {
	map<string, coTerm>::iterator it;
    map<string,int>::iterator in_it;
    for ( it=coTermFreq.begin() ; it != coTermFreq.end(); it++ ) {
      for( in_it=(*it).second.begin(); in_it != (*it).second.end(); in_it++)
      cout << (*it).first <<", "<< in_it->first <<": "<< in_it->second << endl;
                               }
          }
//13 fonction pour determiner si une chaine est un nombre
bool isNumb(const string& s)
{
    string::const_iterator it = s.begin();
    while ((it != s.end())&&(isdigit(*it))&&((*it)!='-')) ++it;
    return it == s.end();
}
//14 fonction pour diviser chaque ligne en tokens, avec ou sans porter-stemming, selon un ensemble de delimiteurs pre-determines
void tokenizer(const string& str, vector<string>& tokens, bool PortStemming, const string& delimiters = " ,.:;{}[]()\'\"$`_")
{
    int lastPos = str.find_first_not_of(delimiters, 0);
    int pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        string tok=str.substr(lastPos, pos - lastPos);
        if (PortStemming) {
              wstring wordl=StrToWStr(tok);
              stemming::english_stem<> StemEnglish;
              StemEnglish(wordl);
              tok=WStrToStr(wordl);
                     }

        if ((not(isNumb(tok)))&&(tok.length()>1)) {
        string::const_iterator it = tok.begin();
        if (isdigit(tok[0]))  {
        	int i=0;
            while ((i<tok.length())&&(isdigit(tok[i]))&&(tok[i]!='-')) ++i;
               if (i<tok.length()) tok.substr(i,tok.length()-i);
                                                 	 }
		   else tokens.push_back(tok);
		   }
		   lastPos = str.find_first_not_of(delimiters, pos);
           pos = str.find_first_of(delimiters, lastPos);
		 }

}

//15 fonction-Parser pour parcourir tous les documents de chaque fichier, utilise une liste d'etiquettes predeterminées tags1, (par default:"<HEAD>","<TEXT>")
//cette fonction utilise les fonctions secondaires: tokenizer, dictionary, SortDictionary , cofreq2

void document_parser(ifstream &infile, map<string, int>& termFreq, set<pairSI, cmp>& termFreqSF, map<string, coTerm>& coTerms, vector<int>& outpp, const set<string>& stoplist = set<string>(), int seuil=10, int dist=15, bool PorterStem=true, bool bseuil=true, bool bothSide=false) {

	 string tags1[]={"<HEAD>","<TEXT>","!@#$%^&*"};
     vector<string> tags(&tags1[0], &tags1[0]+sizeof(tags1)/sizeof(string));
	 vector<string> tokens;
	 int ltags=tags.size();
	 string *fintags;
	 fintags = new string[ltags];
	 for (int i=0; i<ltags;i++) {  //to build final tags from start tags
	 	string my_tag=tags[i];
	 	my_tag.insert(1, "/");
	 	fintags[i]=my_tag;
	  }
	 string line;
	 int textout=0, tagout=0;
	 int tma, tme, c, t, cc, tt;
	 string section;

	 while ( getline (infile,line) )
     {
      c=0,t=0, cc=0, tt=0;
      t=line.find(tags[c],0);
     // cout<<tags[c]<<endl;

	  if  (t==string::npos){
	  c=1;
      while ((c<ltags)&&(t==string::npos)){t=line.find(tags[c],0); c++; } //to find initial tags
      c=c-1;}

      tt=line.find(fintags[cc],0);
      if  (tt==string::npos){
	  cc=1;
      while ((cc<ltags)&&(tt==string::npos)){tt=line.find(fintags[cc],0); cc++; } //to find final tags
      cc=cc-1;}

	  tma=line.find(">",0);
	  tme=line.find("<",0);
	  if  ((tags[c]=="<TEXT>")||(textout==1)){
	  	if ((tags[c]=="<TEXT>")&&(textout==0)) {
	  	    textout=1;}
	  	else if ((fintags[cc]=="</TEXT>")&&(textout==1)){
		    textout=0;
		    tokenizer(section, tokens, PorterStem);
	  	    dictionary(outpp, tokens, termFreq, stoplist);
            termFreqSF=SortDictionary (termFreq, seuil, bseuil);
            cofreq2 (coTerms, tokens, termFreqSF, stoplist, dist, bothSide);
            section.clear();
			tokens.clear();  }
	  	else if ((((tma==string::npos)&&(tme==string::npos))||(tma<tme))&&(textout==1)){ //for processing the text sections
		     if (line[0]=='-') section=section+line;
		     else section=section+" "+line;

			    }
	         }
		else if (((tags[c]=="<HEAD>")||(tags[c]=="<BYLINE>")||(tags[c]=="<FIRST>")||(tags[c]=="<SECOND>")||(tags[c]=="<NOTE>")||(tags[c]=="<DATELINE>"))||(tagout==1))
	         {

				   if ((tt!=string::npos)&&(tagout==0)) { //for no-text sections when the initial and final tags are found in the same line
	           		tokenizer(line.substr(tags[c].length(), (line.length()-(tags[c].length()+fintags[cc].length()))), tokens, PorterStem); // in this case, c=cc
	           		dictionary(outpp, tokens, termFreq, stoplist);
                    termFreqSF=SortDictionary (termFreq, seuil, bseuil);
                    cofreq2  (coTerms, tokens, termFreqSF, stoplist,dist, bothSide);
                    tokens.clear();
	           	//	cout<<line.substr(tags[c].length(), (line.length()-(tags[c].length()+fintags[c].length())))<<endl;
	           		 }
	            else if ((tt==string::npos)&&(tagout==0)){  //for no-text sections when the initial tags are found in the line but not the final ones
	                section=section+" "+line.substr(tags[c].length(), (line.length()-tags[c].length()));
	                //  cout<<line.substr(tags[c].length(), (line.length()-tags[c].length()))<<endl;
           		    tagout=1;
	            	}
	            else if ((tt!=string::npos)&&(tagout==1)) {  //for no-text sections when only the final tags are found in the line but not the initial ones
					if (line[0]=='-') section=section+line.substr(0, (line.length()-fintags[cc].length()));
					else section=section+" "+line.substr(0, (line.length()-fintags[cc].length()));
					tokenizer(section, tokens, PorterStem);
	                dictionary(outpp, tokens, termFreq,stoplist);
                    termFreqSF=SortDictionary (termFreq, seuil, bseuil);
                    cofreq2  (coTerms, tokens, termFreqSF, stoplist, dist, bothSide);
                    section.clear();
					tokens.clear();
           		   // cout<<line.substr(0, (line.length()-fintags[c].length()))<<endl;
           		    tagout=0;
				}
				else if ((((tma==string::npos)&&(tme==string::npos))||(tma<tme))&&(tagout==1)){	// for for no-text section corpus whithout any tag
					 if (line[0]=='-') section=section+line;
		               else section=section+" "+line;
                   // cout<<line<<endl;
				}
			 }

	       }


     delete [] fintags;
}
