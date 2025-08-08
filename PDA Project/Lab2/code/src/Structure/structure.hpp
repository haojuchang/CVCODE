#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>

using namespace std;

enum NODETYPE{_MACRO = 0, _TERMINAL, _INFINITE, _RIGHT, _LEFT, _OLD, _BEST, _RECOVER, _RESULT};

struct NODE{
    string name;
    NODETYPE type;
    size_t ID;
    size_t w, h, x, y; // width, height, x-coor, y-coor
    size_t old_w, old_h, old_x, old_y; // width, height, x-coor, y-coor
    size_t best_w, best_h, best_x, best_y; // width, height, x-coor, y-coor

    NODE *parent = NULL, *left = NULL, *right = NULL;
    NODE *old_parent = NULL, *old_left = NULL, *old_right = NULL;
    NODE *best_parent = NULL, *best_left = NULL, *best_right = NULL;
};

typedef vector<NODE*> NET;

struct FloorPlanData{    
    double alpha;
    size_t outlineX, outlineY;
    vector<NODE*> macros;
    vector<NODE*> terminals;
    vector<NET> nets;
};