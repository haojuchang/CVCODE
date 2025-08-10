#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <cstdio>
#include "Parser/Parser.hpp"
#include "Bin/Bin.hpp"

using namespace std;

string LayoutDir = "NULL";
string InputDIR;
string OutputDir;
int BinW = 2000;
int BinH = 1000;

void getarg(int argc, char *argv[]){
    if(argc < 2){
        cout << "argument error!" << endl;
        exit(1);
    }

    InputDIR = argv[1];
    OutputDir = argv[2];
    if(argc == 5){
        BinW = stoi(argv[3]);
        BinH = stoi(argv[4]);
    }
}

void Group(TILE *tile, vector<TILE*> &component);
void Dump(vector<vector<TILE*>> &components);

int main(int argc, char* argv[]){
    getarg(argc, argv);

    // ===== parse testcase =====
    int outlineWidth, outlineHeight, ViaCost;
    unordered_map<string, vector<TILE*>> Metal_Layer;
    unordered_map<string, vector<TILE*>> Via_Layer;
    vector<TILE*> alltile;

    Parser PS(InputDIR, outlineWidth, outlineHeight, ViaCost, Metal_Layer, Via_Layer, alltile);

    if(argc != 5){
        if(outlineWidth == 7000 && outlineHeight == 3000){
            BinW = 300;
            BinH = 700;
        }
        if(outlineWidth == 40000 && outlineHeight == 15000){
            BinW = 800;
            BinH = 300;
        }
        if(outlineWidth == 500000 && outlineHeight == 150000){
            BinW = 1900;
            BinH = 1000;
        }
        if(outlineWidth == 350000 && outlineHeight == 150000){
            BinW = 1400;
            BinH = 700;
        }
        if(outlineWidth == 7000 && outlineHeight == 4000){
            BinW = 400;
            BinH = 200;
        }
        if(outlineWidth == 35000 && outlineHeight == 20000){
            BinW = 1800;
            BinH = 700;
        }
        if(outlineWidth == 400000 && outlineHeight == 100000){
            BinW = 1600;
            BinH = 1000;
        }
    }

    // ===== initial each layer bin =====
    vector<Bin*> Bins;
    for(int i = 0; i < (int) Metal_Layer.size() + 1; i++){
        Bin *bin = new Bin(BinW, BinH, outlineWidth, outlineHeight);
        Bins.emplace_back(bin);
    }

    // ===== insert routedshape into each bin =====
    // ===== check overlap during insert =====
    int totalshapecnt = 0, totalobstaclecnt = 0;
    for(auto layer : Metal_Layer)
    {
        string metal = layer.first;
        vector<TILE*> &tiles = layer.second;

        int L = stoi(metal.substr(1));
        Bin *bin = Bins[L];

        int shapecnt = 0, obstaclecnt = 0;
        for(TILE* tile : tiles){
            if(tile->comptype == OBSTACLE) {
                obstaclecnt++;
                continue;
            }

            shapecnt++;

            bin->AddTile2Bin(tile);
        }
        totalshapecnt += shapecnt;
        totalobstaclecnt += obstaclecnt;
        // printf("metal: %s, shapecnt: %d, obstaclecnt: %d\n", metal.c_str(), shapecnt, obstaclecnt);
    }

    // ===== insert routedvia into each bin[L] and bin[L+1] =====
    int totalviacnt = 0;
    for(auto layer : Via_Layer)
    {
        string via = layer.first;
        vector<TILE*> &tiles = layer.second;

        int L = stoi(via.substr(1));
        Bin *bin1 = Bins[L];
        Bin *bin2 = Bins[L+1];
        
        for(TILE* tile : tiles){
            bin1->AddTile2Bin(tile);
            bin2->AddTile2Bin(tile);
        }
        
        totalviacnt += tiles.size();
        // printf("via: %s, viacnt: %ld\n", via.c_str(), tiles.size());
    }

    //===== find net components =====
    vector<vector<TILE*>> components;
    for(TILE *tile : alltile){
        if(tile->comptype == OBSTACLE) continue;
        if(tile->visit) continue;
        vector<TILE*> comp;
        Group(tile, comp);
        components.emplace_back(comp);
    }

    Dump(components);

    printf("|W|: %6d, |H|: %6d, |L|: %1ld, |N|: %6d, |O|: %5d, |V|: %2d, Via Cost: %3d, |C|: %5ld\n", outlineWidth, outlineHeight, Metal_Layer.size(), totalshapecnt, totalobstaclecnt, totalviacnt, ViaCost, components.size());

    return 0;
}

void Dump(vector<vector<TILE*>> &components){
    ofstream fout(OutputDir);

    fout << components.size() << endl;

    for(int i = 0; i < (int) components.size(); i++){
        fout << "C" << i + 1 << endl;
        for(TILE *t : components[i]){
            if(t->comptype == ROUTEDSHAPE) {
                fout << "RoutedShape <" << t->layer << "> <";
                fout << t->llc.x << ", " << t->llc.y << "> <";
                fout << t->llc.x + t->width << ", " << t->llc.y + t->height << ">" << endl;
            }
            if(t->comptype == ROUTEDVIA) {                
                fout << "RoutedVia <" << t->layer << "> <";
                fout << t->llc.x << ", " << t->llc.y << ">" << endl;
            }
        }
    }

    fout.close();
}

void Group(TILE *tile, vector<TILE*> &component){
    component.emplace_back(tile);
    tile->visit = true;
    for(TILE *t : tile->connect){
        if(t->visit) continue;
        Group(t, component);
    }
}