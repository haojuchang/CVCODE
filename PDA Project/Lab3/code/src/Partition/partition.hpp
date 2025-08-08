#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <map>
#include <random>
#include <set>
#include <functional>
#include <fstream>
#include <stdlib.h>
#include "Structure/struct.hpp"

using namespace std;

class MyHashFunction {
public:
    size_t operator()(const NODE*) const
    {
        return std::hash<std::string>{}("MyString");
    }
};

class Partition{
public:
    Partition(vector<NODE*> Cells, size_t Chip_Height, size_t Row_Height);

private:
    vector<NODE*> Cells;
    string graphName = "tmp.graph";
    size_t edge_cnt = 0;
    size_t maxOverlap = 0;
    size_t Row_Height;
    size_t Chip_Height;
    vector<vector<pair<size_t, size_t>>> graph;
    // map<NODE*, map<NODE*, size_t, classcomp>, classcomp> graph;
    // for FM
    void FM(vector<NODE*> &nodes);
    NODE* InitGainMap(unordered_map<NODE*, int, MyHashFunction> &gainmap, vector<NODE*> &nodes);
    void InitTOPBOT(vector<NODE*> &nodes);
    void RandomDecideLocate();
    void FindGoodLocate();
    int FindOverlapArea();
    
    // for overlap graph
    void ConstructOverlapGraph();
    bool TwoCellOverlap(NODE *a, NODE *b);
    void PrintGraph();
    void ReadPartition();
};