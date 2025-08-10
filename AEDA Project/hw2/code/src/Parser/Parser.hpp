#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <unordered_map>
using namespace std;

class Parser{
public:
    Parser();
    vector<string> getToken(string str, string split);
private:
};