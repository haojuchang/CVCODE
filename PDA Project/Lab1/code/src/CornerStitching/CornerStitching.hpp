#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "Struct/Struct.hpp"
using namespace std;

class CornerStiching{
public:
    CornerStiching(int outlineWidth, int outlineHeight);
    POS PointFinding(POS *point);
    void TileCreating(TILE* tile);
    void DumpLayout(string LayoutDir);
    void DumpBlock(string OutputDir);

private:
    TILE *LRtile; // lower right tile
    TILE* _point_finding(POS *point);
    vector<TILE*> _neighbor_finding(TILE *t);
    vector<TILE*> _neighbor_finding_left(TILE* t);
    vector<TILE*> _neighbor_finding_right(TILE* t);
    vector<TILE*> _neighbor_finding_top(TILE* t);
    vector<TILE*> _neighbor_finding_down(TILE* t);
    void _update_pointer_top(TILE *target, vector<TILE*> &nei);
    void _update_pointer_down(TILE *target, vector<TILE*> &nei);
    void _update_pointer_left(TILE *target, vector<TILE*> &nei);
    void _update_pointer_right(TILE *target, vector<TILE*> &nei);
    void _update_LRtile(TILE *target);
    void _horizontal_tile_split(POS p);
    void _vertical_tile_split(POS p);
    bool _merge_neighbor(TILE *&low, TILE *&height);
    TILE* _merge_center(vector<TILE*> &center, TILE *tile);

    int tileCnt = 0;

    vector<TILE*> tiles; // include space and block
    vector<TILE*> blocks; // only store block no space
    vector<TILE*> trash;
    int outlineWidth, outlineHeight;
};