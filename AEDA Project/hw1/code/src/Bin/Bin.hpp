#pragma once

#include <iostream>
#include <vector>
#include <cstdio>
#include "Struct/Struct.hpp"

using namespace std;

class Bin{
public:
    Bin(int BinW, int BinH, int outlineWidth, int outlineHeight);
    void AddTile2Bin(TILE* tile);

private:
    int BinW, BinH, outlineWidth, outlineHeight;
    vector<vector<vector<TILE*>>> Block;

    bool overlap(TILE *t1, TILE *t2);
};