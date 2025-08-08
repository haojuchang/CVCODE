#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>

using namespace std;

enum UPDN{UP = 0, DN};

struct NODE{
    size_t num;
    unordered_map<size_t, NODE*> VCG_FI, VCG_FO;
    unordered_map<size_t, NODE*> VCG_FI_copy, VCG_FO_copy;
    unordered_map<size_t, NODE*> HCG; // key pin num, value pin node
    vector<pair<size_t, UPDN>> locate; // the position at uch/dch
    bool route = false;
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
    void CheckCycle();
    void RouteNoneCycle();
    void AddToTrack(vector<NODE*> nofinode);
    void RouteVlayer();

    vector<string> getToken(string str, string split);
};