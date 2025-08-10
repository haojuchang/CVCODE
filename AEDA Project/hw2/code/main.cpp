#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <cstdio>
#include "MBFF/MBFF.hpp"

using namespace std;

void getarg(int argc, char *argv[]);

string InputDIR;
string OutputDir;

int main(int argc, char* argv[]){
    getarg(argc, argv);

    MBFF mbff;
    mbff.Parser_ICCAD2010(InputDIR);
    int power = mbff.PrintFF();
    mbff.FF_Feasible_Region();
    mbff.ConstructOverlapGraph();
    int optpower = mbff.Find_clique();
    mbff.Dump(OutputDir);
    cout << "Power Reduction: " << (double) optpower / (double) power << endl;

    cout << endl;
    return 0;
}

void getarg(int argc, char *argv[]){
    if(argc < 3){
        cout << "argument error!" << endl;
        exit(1);
    }

    InputDIR = argv[1];
    OutputDir = argv[2];
}