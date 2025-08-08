#include "parser.hpp"

Parser::Parser(string inputDir) 
    : inputDir(inputDir)
{
}

void Parser::Parse(){
    ifstream fin(inputDir);

    size_t id = 0;

    string str;
    NODETYPE type = _NONE;
    while(getline(fin, str)){
        vector<string> tok = getToken(str, " ");
        if(tok.size() == 0) continue;

        if(tok[0] == "DieSize") {
            Chip_Width = stoi(tok[1]);
            Chip_Height = stoi(tok[2]);
            continue;
        }
        else if(tok[0] == "DieRows") {
            Row_Height = stoi(tok[1]);
            Row_Number = stoi(tok[2]);
            continue;
        }
        else if(tok[0] == "Terminal") {
            type = _TERMINAL;
            continue;
        }
        else if(tok[0] == "NumCell") {
            type = _CELL;
            continue;
        }

        NODE *n = new NODE;
        n->type = type;
        n->name = tok[0];
        n->x = stoi(tok[1]);
        n->y = stoi(tok[2]);
        n->w = stoi(tok[3]);
        n->h = stoi(tok[4]);
        if(type == _TERMINAL) Terminals.emplace_back(n);
        if(type == _CELL) Cells.emplace_back(n);
    }

    fin.close();
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