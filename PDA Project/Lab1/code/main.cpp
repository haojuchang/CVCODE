#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include "Parser/Parser.hpp"
#include "CornerStitching/CornerStitching.hpp"

using namespace std;

string LayoutDir = "NULL";
string InputDIR;
string OutputDir;

void getarg(int argc, char *argv[]){
    if(argc < 3){
        cout << "argument error!" << endl;
        exit(1);
    }

    InputDIR = argv[1];
    OutputDir = argv[2];

    int ch;
    while((ch = getopt(argc, argv, "L:")) != -1){
        switch(ch){
            case 'L':
                LayoutDir = optarg;
        }
    }
}

int main(int argc, char* argv[]){
    getarg(argc, argv);

    int outlineWidth, outlineHeight;
    POS *point = NULL;
    TILE* tile = NULL;

    Parser PS(InputDIR);
    PS.getOutline(outlineWidth, outlineHeight);

    CornerStiching CS(outlineWidth, outlineHeight);
    vector<POS> points;
    int t = 0;
    while(PS.next(point, tile)){
        t++;
        // if(t == 29) break;
        // cout << point << " " << tile << endl;
        if(point != NULL){
            POS p = CS.PointFinding(point);
            points.emplace_back(p);
        }
        
        if(tile != NULL){
            // cout << tile->index << endl;
            CS.TileCreating(tile);
        }
    }

    if (LayoutDir != "NULL")
        CS.DumpLayout(LayoutDir);
    CS.DumpBlock(OutputDir);

    ofstream fout(OutputDir, ios::app);
    for(POS &p : points){
        fout << p.x << " " << p.y << endl;
    }
    fout.close();

    return 0;
}