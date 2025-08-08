#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <time.h>
#include <limits.h>
#include <stdlib.h> 
#include <math.h>
#include<functional>
#include <random>
#include<utility>
#include "Structure/structure.hpp"
using namespace std;

#define OUTLOG false

struct BLOCK;
struct MACRO;
struct COR;
struct TERMINAL;


// ====================================================================

class FloorPlan{
public:
    FloorPlan(double alpha);
    void ParseBlock(string blockDir);
    void ParseNet(string netDir);
    void getPos(vector<NODE*> &left, vector<NODE*> &right, vector<int> &posX, vector<int> &posY, int &width, int &height);
    void initSP(vector<NODE*> &left, vector<NODE*> &right);
    void SA(vector<NODE*> &left, vector<NODE*> &right, vector<int> &posX, vector<int>& posY, int &width, int &height);
    void costfunc(unsigned long int &cost, vector<int> &posX, vector<int> &posY);
    void move(vector<NODE*> &new_left, vector<NODE*> &new_right, vector<NODE*> &left, vector<NODE*> &right);
    void writeOutput(string Dir, vector<int> &posX, vector<int> &posY,  int width, int height, double cpu_time_used);
private:
    int macroCnt = 0;
    int outlineX, outlineY;
    double alpha;
    unordered_set<NODE*> macros;
    unordered_set<NODE*> terminals;
    unordered_map<string, NODE*> nodes;
    vector<NET> nets;
    // int action;
    // int actionMacro;

    void calarea(unsigned long int &A, int width, int height);
    void calwire(unsigned long int &W, vector<int> &posX, vector<int> &posY);
    void calcost(unsigned long int &cost, double alpha, unsigned long int A, unsigned long int W);
    void LCS(int mode, vector<NODE*> &left, vector<NODE*> &right, vector<int> &pos, int &longest);
    void getToken(string str, vector<string> &tok, string split);
};


