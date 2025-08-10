#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <time.h>
using namespace std;

class MODEL{
public:
    MODEL(){};
    clock_t _START, _END;
    double _CPUTIME;
    string IO_REL, IN_PAT, IOGEN, FOLDER_POS;

    void creat_randomPat(int bitCount, vector<string> &pat, int inputN);
    void put_iogen(string iogen, string in_pat, string io_rel);
    void read_io_rel(string io_rel, vector<string> &rel);
    void gen_io_rel(string iogen, string relDir, string patDir, vector<string> &pat, vector<string> &rel, int inputN, vector<string> IOvariable);
    void testDiff(int inputN, int outputN, vector<string> IOvariable, vector<vector<pair<double,int>>> &iodiff, vector<int> &iodummyCount);
    void prepareQM(int inputN, vector<string> &pat, vector<pair<double,int>> &diff);
    bool iscover(string &in, string &qm);
    bool check(vector<string> &inpat, vector<string> &qmpat);

private:
    string creatOnePat(int bit);
};