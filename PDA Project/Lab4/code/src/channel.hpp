#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <vector>
#include <list>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <set>
#include <random>

using namespace std;

enum UPDN{UP = 0, DN};
#define LOG false

struct NODE{
    size_t num;
    unordered_map<size_t, NODE*> VCG_FI, VCG_FO;
    unordered_map<size_t, NODE*> VCG_FI_copy, VCG_FO_copy;
    unordered_map<size_t, NODE*> HCG; // key pin num, value pin node
    vector<pair<size_t, UPDN>> locate; // the position at uch/dch
    bool route = false;
    int leafpos = -1;
};

class ChannelRoute{
public:
    ChannelRoute();
    void Run();
    void Parse(string inputDir);
    void Dump(string outputDir);

private:
    vector<size_t> uch, dch; //up down channel
    list<vector<size_t>> vlayer, hlayer; // front is uch, back is dch
    list<NODE*> allnode;
    unordered_map<size_t, NODE*> nodemap;  // key pin num, value pin node

    void ConstructVCG();
    void ConstructHCG();
    bool CheckCycle();
    void RouteNoneCycle();
    void AddToTrack(vector<NODE*> nofinode);
    void AddToTrackAll(vector<NODE*> nofinode);
    void RouteVlayer();
    bool CycleRoute();

    vector<string> getToken(string str, string split);
};
