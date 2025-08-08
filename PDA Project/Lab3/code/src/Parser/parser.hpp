#pragma once

#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>
#include "Structure/struct.hpp"
using namespace std;

class Parser{
public:
    Parser(string inputDir);
    void Parse();

    // getter
    vector<NODE*> Get_Cells(){return Cells;}
    vector<NODE*> Get_Terminals(){return Terminals;}
    size_t Get_Chip_Width() { return Chip_Width;}
    size_t Get_Chip_Height() { return Chip_Height;}
    size_t Get_Row_Height() { return Row_Height;}
    size_t Get_Row_Number() { return Row_Number;}

private:
    string inputDir;
    size_t Chip_Width, Chip_Height, Row_Height, Row_Number;
    vector<NODE*> Cells, Terminals;

    vector<string> getToken(string str, string split);
};