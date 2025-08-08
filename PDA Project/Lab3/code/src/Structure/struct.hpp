#pragma once

#include <iostream>
using namespace std;

enum NODETYPE{_NONE = -1, _TERMINAL = 0, _CELL = 1};
enum CHIPLOCATE{_TOP = 0, _BOT = 1};

struct NODE{
    NODETYPE type;
    CHIPLOCATE locate;
    string name;
    size_t x, y, w, h, Lx, Ly;

    size_t nearestRow;
    bool fixed = false;
};