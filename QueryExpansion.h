#ifndef QUERYEXPANSION_H
#define QUERYEXPANSION_H

struct cmp {
     bool operator() (const pairSI& l, const pairSI& r) const
	 {
	 	if (l.second!=r.second)
	 	   return l.second>r.second;
	 	return l.first<r.first;
    	}
	};
   struct find1
   {
        find1(const string& str) : str(str) {}
        bool operator()(const pair<string, int>& v) {
                return v.first == str;
        }
        const string& str;
    };

    void read_folder(const string& name, vector <string> & list);
    wstring StrToWStr(const string& s);
    string WStrToStr(const wstring& s);
    set<string> ReadStopList(ifstream &infile);
    set<pairSI, cmp> SortDictionary (map<string, int>& termFreq, int seuil, bool bseuil);
    void dictionary(vector <int>& outp,  vector<string>& tokens, map<string, int>& termFreq, const set<string>& stoplist = set<string>());
    void printDict (map<string, int> termFreqq);
    void printSDict (set<pair<string, int>, cmp>  SortedtermFreq, vector <int> pp);
    void cofreq2 (map<string, coTerm>& coTermFreq, vector<string>& tokens,  set<pairSI, cmp>& termFreqq, const set<string>& stoplist = set<string>(), int dist=15, bool bothSide=false);
    void OutFile(int uy, map<string, coTerm> coTermFreq, set<pair<string, int>, cmp>  SortedtermFreq, vector <int> pp, ofstream &OutputFile,  int cont=10, int seuil=10, bool porter=false);
    void printCoDict (map<string, coTerm> coTermFreq);
    bool isNumb(const string& s);
    void tokenizer(const string& str, vector<string>& tokens, bool PortStemming, const string& delimiters = " ,.:;{}[]()\'\"$`_");
    void document_parser(ifstream &infile, map<string, int>& termFreq, set<pairSI, cmp>& termFreqSF, map<string, coTerm>& coTerms, vector<int>& outpp, const set<string>& stoplist = set<string>(), int seuil=10, int dist=15, bool PorterStem=true, bool bseuil=true, bool bothSide=false);

#endif