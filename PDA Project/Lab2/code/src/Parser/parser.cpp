#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

#include "string.h"
#include "parser.hpp"
#include "Structure/structure.hpp"
using namespace std;

Parser::Parser(string BlockDir, string NetDir) 
    : BlockDir(BlockDir), NetDir(NetDir) 
{
    ParseBlock(BlockDir);
    ParseNet(NetDir);
}

void Parser::ParseBlock(string blockDir){
    ifstream in(blockDir);
    string str;
    vector<string> tok;
    // vector<vector<string>> toks;
    while(getline(in, str)){
        tok = getToken(str, ": \t");
        if(tok.size() == 0)
            continue;
       
        if(tok[0] == "Outline"){
            outlineX = stoi(tok[1]);
            outlineY = stoi(tok[2]);
        }
        else if(tok[0] == "NumBlocks" || tok[0] == "NumTerminals")
            continue;
        else if(tok[1] == "terminal"){
            NODE *t = new NODE;
            t->name = tok[0];
            t->type = _TERMINAL;
            t->x = stoi(tok[2]);
            t->y = stoi(tok[3]);
            terminals.emplace_back(t);
            nodes[t->name] = t;
        }
        else { // macro
            NODE *m = new NODE;
            m->ID = macroCnt++;
            m->name = tok[0];
            m->type = _MACRO;
            m->w = stoi(tok[1]);
            m->h = stoi(tok[2]);
            macros.emplace_back(m);
            nodes[m->name] = m;
        }
    }
    in.close();
}

void Parser::ParseNet(string netDir){
    ifstream in(netDir);
    string str;
    vector<string> tok;
    int netNum;

    while(getline(in, str)){
        tok = getToken(str, ": ");
        if(tok.size() == 0) 
            continue;

        if(tok[0] == "NumNets"){
            netNum = stoi(tok[1]);
            nets.reserve(netNum);
        }
        else if(tok[0] == "NetDegree"){
            int degree = stoi(tok[1]);
            NET n;
            n.reserve(degree);

            for(int i = 0; i < degree; i++){
                getline(in, str);
                tok = getToken(str, ": ");
                NODE *node = nodes[tok[0]];
                n.emplace_back(node);
            }
            nets.emplace_back(n);
        }
    }
    in.close();
}

FloorPlanData Parser::getFloorPlanData(){
    FloorPlanData FPD;
    FPD.outlineX = outlineX;
    FPD.outlineY = outlineY;
    FPD.macros = macros;
    FPD.terminals = terminals;
    FPD.nets = nets;
    return FPD;
}

vector<string> Parser::getToken(string str, string split){
    vector<string> tok;
    size_t r = str.find("\r");
    if( r != string::npos)
        str.erase(r, 1);

    tok.clear();
    char *token = strtok((char *)str.c_str(), (char *)split.c_str());
    while (token != NULL)
    {
        tok.emplace_back(string(token));
        token = strtok(NULL, (char *)split.c_str());
    }
    return tok;
}