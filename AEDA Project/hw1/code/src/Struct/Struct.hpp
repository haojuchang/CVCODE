#pragma once

#include <iostream>
#include <set>
enum TILETYPE{SPACE = -1, BLOCK = 1, TRASH = 2};
enum COMPTYPE{ROUTEDSHAPE = 0, OBSTACLE, ROUTEDVIA}; // component type

struct POS{
    int x, y;
    POS(){}
    POS(int ix, int iy) : x(ix), y(iy){}
};

struct TILE{
    int index;
    std::string layer;
    COMPTYPE comptype;
    POS llc; // lower-left corner
    int width;
    int height;
    std::set<TILE*> connect;
    bool visit = false;

    TILE(){}
};
