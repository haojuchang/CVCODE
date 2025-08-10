#pragma once

#include <iostream>
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <limits.h>

using namespace std;

struct POS{
    int x, y;
    double HPWL(POS p){
        return abs(x-p.x) + abs(y-p.y);
    }
};
struct Rect{
    POS ll, tr; // lower left, top right
    bool intersect;
};

struct FLIP_FLOP{
    string FF_name;
    int BIT_NUMBER;
    int POWER_CONSUMPTION;
    int AREA;
};

struct NODE{
    string FF_name; // FF1, FF2, ..., INPUT, OUPUT
    string NODE_name;
    POS pos;
    int pinslack;
    vector<NODE*> FFpin;    
    
    Rect FRegion;
    unordered_map<string, NODE*> neighbor;
    bool merged = false;
};

class MBFF{
public:
    MBFF() {}
    void Parser_ICCAD2010(string str);
    void FF_Feasible_Region();
    int PrintFF();
    void PrintLIB();
    void ConstructOverlapGraph();
    int Find_clique();
    void Dump(string str);
private:
    int nameidx = 0;
    int CHIP_SIZE_W, CHIP_SIZE_H;
    POS GRID_SIZE;
    int BIN_SIZE_BinW, BIN_SIZE_BinH;
    int PLACEMENT_DENSITY_CONSTRAINT;
    unordered_map<string, FLIP_FLOP*> LIBRARY;
    unordered_map<string, NODE*> FLIP_FLOP_LIST, PIN_LIST;
    vector<vector<NODE*>> cliques;
    vector<NODE*> FinalFF;
    
    void rec_clique(NODE *n, vector<NODE*> &cli, int &curbit, int targetbit, vector<vector<NODE*>> &targetcli);
    vector<NODE*> choosetargetcli(vector<vector<NODE*>> &targetcli);
    void chooseallcli(vector<vector<NODE*>> &allcli);
    void DetermineFFpos(NODE *n);
    void Rotate45(Rect &rect);
    Rect Intersect(Rect a, Rect b);
// ===== Parser =====
    void Parser_ICCAD2010_LIBRARY(ifstream &fin);
    void Parser_ICCAD2010_FLIP_FLOP_LIST(ifstream &fin);
    void Parser_ICCAD2010_PIN_LIST(ifstream &fin);
    void Parser_ICCAD2010_NET_LIST(ifstream &fin);
// ===== Parser =====
};