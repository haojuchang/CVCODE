#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include "Struct/Struct.hpp"
using namespace std;

class Parser{
public:
    Parser(string inputDir);
    void getOutline(int &outlineWidth, int &outlineHeight);
    bool next(POS *&point, TILE *&tile);
private:
    vector<string> getToken(string str, string split);
    ifstream fin;

};