#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <fstream>
#include <limits.h>
#include <math.h>
#include <thread>
#include "Structure/struct.hpp"

using namespace std;


struct CLUSTER{
    size_t ec, wc, qc; // ec:total weight(area or pin), wc:total width, qc: 
    size_t xc; // optimal position of cluster
    size_t nc; // number of cells in cluster
    size_t nfirst, nlast; // are the index in ROW->RCells
};

struct ROW{
    vector<NODE*> RCells;
    list<CLUSTER> clusters;
    size_t y, h;
    size_t xmax, xmin;
    size_t total_cell_width = 0;
};

class Legalize{
public:
    Legalize(vector<NODE*> Cells, vector<NODE*> Terminals, size_t Chip_Width, size_t Chip_Height, size_t Row_Height, size_t Row_Number);

    void Abacus();
    vector<NODE*> GetFailedCell(){return failed_cell;}

    // test code
    void print_Cells();
    void print_Terminals();

private:
    vector<NODE*> Cells, Terminals;
    size_t Chip_Width, Chip_Height, Row_Height, Row_Number;

    vector<size_t> costs;
    
    vector<ROW*> rows;
    vector<NODE*> failed_cell;
    void InitRow();
    void PlaceRow(ROW *row);
    void AddCell(CLUSTER &c, ROW *row, size_t i);
    void AddCluster(CLUSTER &c, CLUSTER &c_);
    void Collapse(ROW *row);
    size_t Cost(vector<NODE*> &Cells);
    void FindNearestRow();
    // void pp(size_t r, NODE *cell, vector<size_t> &costs);
};