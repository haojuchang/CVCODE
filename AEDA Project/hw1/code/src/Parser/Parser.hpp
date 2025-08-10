#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include "Struct/Struct.hpp"
#include <unordered_map>
using namespace std;

class Parser{
public:
    Parser(string inputDir, int &outlineWidth, int &outlineHeight, int &ViaCost, unordered_map<string, vector<TILE*>> &Metal_Layer, unordered_map<string, vector<TILE*>> &Via_Layer, vector<TILE*> &alltile);
private:
    int Spacing;
    vector<string> getToken(string str, string split);
};