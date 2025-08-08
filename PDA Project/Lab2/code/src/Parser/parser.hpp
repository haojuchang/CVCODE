#pragma once

#include <iostream>
#include "Structure/structure.hpp"
using namespace std;

class Parser{
public:
    Parser(string BlockDir, string NetDir);
    void ParseBlock(string blockDir);
    void ParseNet(string netDir);
    FloorPlanData getFloorPlanData();



private:
    string BlockDir, NetDir;
    
    int macroCnt = 0;
    int outlineX, outlineY;
    vector<NODE*> macros;
    vector<NODE*> terminals;
    unordered_map<string, NODE*> nodes; // contain macro and terminal
    vector<NET> nets;

    vector<string> getToken(string str, string split);
};