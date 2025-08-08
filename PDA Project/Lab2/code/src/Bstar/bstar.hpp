#pragma once

#include <iostream>
#include <list>
#include <algorithm>
#include <fstream>
#include <limits.h>
#include <float.h>
#include <random>
#include "Structure/structure.hpp"
#include <cstdlib>      // std::rand, std::srand
#include <ctime>        // std::time
#include <chrono>       // std::chrono::system_clock
#include <functional>
using namespace std;

class Bstar{
public:
    Bstar(FloorPlanData FPD);
    void Visualize(string dir);
    void Area_SA();
    double Inline_SA();
    void dump(string dir, double alpha, double time);
    
private:
    void InitTree(vector<NODE*> &macros);
    void ComputeMaxWH(size_t &maxW, size_t &maxH);
    size_t ComputeWire();
    void Traverse(NODE *n, size_t parent_x, size_t parent_w, list<NODE*> &contour, size_t &maxW, size_t &maxH);
    void UpdateContour(NODE *n, list<NODE*> &contour);
    void OP1_rotate(NODE *n);
    void OP2_move(NODE *n, NODE *place, int LR);
    void OP3_swap(NODE *n, int LR);
    void OP3_swap_two_modules(NODE *n1, NODE *n2);
    void perturb();
    void copy(int mode);
    double overlap_coss();
    double coss_function(size_t area, size_t wire);
    double avg_area, avg_wire;
    double n = 1;
    void Packing();
    
    NODE *root = NULL; // only contain macros
    size_t bestW, bestH, best_wire;
    NODE *old_root, *best_root;
    FloorPlanData FPD;
};