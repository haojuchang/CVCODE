#include "Parser.hpp"

Parser::Parser(string inputDir){
    fin.open(inputDir);
}

void Parser::getOutline(int &outlineWidth, int &outlineHeight){
    string str;
    getline(fin, str);
    vector<string> tok = getToken(str, " ");
    outlineWidth = stoi(tok[0]);
    outlineHeight = stoi(tok[1]);
}

bool Parser::next(POS *&point, TILE *&tile){
    if(fin.eof()) return false;
    point = NULL;
    tile = NULL;

    string str;
    getline(fin, str);
    vector<string> tok = getToken(str, " ");

    if(tok.size() == 0){
        return true;
    }
    else if(tok[0] == "P"){
        point = new POS(stoi(tok[1]), stoi(tok[2]));
    }
    else {
        tile = new TILE;
        tile->index = stoi(tok[0]);
        tile->type = BLOCK;
        tile->llc = POS(stoi(tok[1]), stoi(tok[2]));
        tile->width = stoi(tok[3]);
        tile->height = stoi(tok[4]);
    }

    return true;
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