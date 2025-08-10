#include "Parser.hpp"

Parser::Parser(string inputDir, int &outlineWidth, int &outlineHeight, int &ViaCost, unordered_map<string, vector<TILE*>> &Metal_Layer, unordered_map<string, vector<TILE*>> &Via_Layer, vector<TILE*> &alltile){
    ifstream fin(inputDir);

    int index = 0;

    string str;
    while(getline(fin, str)){
        if(str[0] == '#') continue;
        vector<string> tok = getToken(str, " (,)");
        if(tok[0] == "ViaCost") ViaCost = stoi(tok[2]);
        else if(tok[0] == "Spacing") Spacing = stoi(tok[2]);
        else if(tok[0] == "Boundary")
        {
            outlineWidth = stoi(tok[4]) - stoi(tok[2]);
            outlineHeight = stoi(tok[5]) - stoi(tok[3]);            
        }
		else if(tok[0] == "Obstacle") {
			continue;
		}
        else if(tok[0] == "RoutedShape")
        {
            COMPTYPE comptype = ROUTEDSHAPE;
            
            string layer = tok[1];
            
            TILE *tile = new TILE;
            tile->index = index++;
            tile->layer = layer;
            tile->comptype = comptype;
            tile->llc = POS(stoi(tok[2]), stoi(tok[3]));
            tile->width = stoi(tok[4]) - stoi(tok[2]);
            tile->height = stoi(tok[5]) - stoi(tok[3]); 

            auto find = Metal_Layer.find(layer);
            if(find == Metal_Layer.end()){
                Metal_Layer[layer] = vector<TILE*>();
            }
            
            Metal_Layer[layer].emplace_back(tile);
            alltile.emplace_back(tile);
        }
        else if(tok[0] == "RoutedVia")
        {
            string layer = tok[1];

            TILE *tile = new TILE;
            tile->index = index++;
            tile->layer = layer;
            tile->comptype = ROUTEDVIA;
            tile->llc = POS(stoi(tok[2]), stoi(tok[3]));
            tile->width = 0;
            tile->height = 0;
            
            auto find = Via_Layer.find(layer);
            if(find == Via_Layer.end()){
                Via_Layer[layer] = vector<TILE*>();
            }

            Via_Layer[layer].emplace_back(tile);
            alltile.emplace_back(tile);
        }
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