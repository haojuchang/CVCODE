#include "MBFF.hpp"
#include "Parser/Parser.hpp"
#include <fstream>

void MBFF::DetermineFFpos(NODE *n){
    int startx = n->FRegion.ll.x / GRID_SIZE.x;
    if(n->FRegion.ll.x % GRID_SIZE.x != 0) startx++;
    int endx = n->FRegion.tr.x / GRID_SIZE.x;

    int starty = n->FRegion.ll.y / GRID_SIZE.y;
    if(n->FRegion.ll.y % GRID_SIZE.y != 0) starty++;
    int endy = n->FRegion.tr.y / GRID_SIZE.y;
    
    for(int curx = startx; curx < endx; curx++){
        for(int cury = starty; cury < endy; cury++){
            POS real;
            real.x = (curx * GRID_SIZE.x - cury * GRID_SIZE.y) / 2;
            real.y = (curx * GRID_SIZE.x + cury * GRID_SIZE.y) / 2;
            if(real.x % GRID_SIZE.x != 0 || real.y % GRID_SIZE.y !=0) continue;
            if(real.x > CHIP_SIZE_W || real.y > CHIP_SIZE_H) continue;
            bool valid = true;
            for(NODE *pin : n->FFpin){
                int hpwl = real.HPWL(pin->pos);
                if(hpwl > pin->pinslack){
                    valid = false;
                    break;
                }
            }
            if(!valid) continue;
            n->pos = real;
        }
    }
}

void MBFF::Dump(string str){
    ofstream fout(str);
    fout << "[FLIP_FLOP_LIST]" << endl;
    for(NODE *n : FinalFF){
        DetermineFFpos(n);
        fout << n->FF_name << " " << n->NODE_name << " (" << n->pos.x << "," << n->pos.y << ")" << endl;
    }
    
    fout << "[END FLIP_FLOP_LIST]" << endl;
    fout << "[NET_LIST]" << endl;
    for(NODE *n : FinalFF){
        for(NODE *pin : n->FFpin){
            int hpwl = n->pos.HPWL(pin->pos);
            fout << pin->NODE_name << " " << n->NODE_name <<  " " << pin->pinslack - hpwl << endl;
        }
    }
    fout << "[END NET_LIST]" << endl;

    fout.close();
}

// ===== clique =====
void MBFF::rec_clique(NODE *n, vector<NODE*> &cli, int &curbit, int targetbit, vector<vector<NODE*>> &targetcli){
    if(n->merged) return;

    // check n is not in cli
    for(NODE *c : cli){
        if(c == n) return;
    }

    // check n has edge with all cli
    for(NODE *c : cli){
        auto got = c->neighbor.find(n->NODE_name);
        if(got == c->neighbor.end()) return;
    }

    curbit += LIBRARY[n->FF_name]->BIT_NUMBER;
    cli.emplace_back(n);

    if(curbit < targetbit){
        for(auto neipair : n->neighbor){
            NODE *nei = neipair.second;
            rec_clique(nei, cli, curbit, targetbit, targetcli);
        }
        cli.pop_back();
        curbit -= LIBRARY[n->FF_name]->BIT_NUMBER;
    }
    else{
        if(curbit == targetbit) targetcli.emplace_back(cli);

        cli.pop_back();
        curbit -= LIBRARY[n->FF_name]->BIT_NUMBER;
        return;
    }
}

vector<NODE*> MBFF::choosetargetcli(vector<vector<NODE*>> &targetcli){
    vector<NODE*> mincli;
    int mincnt = INT_MAX;
    for(vector<NODE*> target : targetcli){
        int neighborcnt = 0;
        for(NODE *t : target){
            neighborcnt += t->neighbor.size();
        }
        if(neighborcnt < mincnt){
            mincnt = neighborcnt;
            mincli = target;
        }
    }
    return mincli;
}

void MBFF::chooseallcli(vector<vector<NODE*>> &allcli){    
    struct MYCOMP {
        bool operator() (vector<NODE*> &i, vector<NODE*> &j) { 
            int inei = 0, jnei = 0;
            for(NODE *n : i) inei += n->neighbor.size();
            for(NODE *n : j) jnei += n->neighbor.size();
            return (inei < jnei);
        }
    } mycomp;
    sort(allcli.begin(), allcli.end(), mycomp);

    for(vector<NODE*> &cli : allcli){
        bool choose = true;
        for(NODE *n : cli) {
            if(n->merged) {
                choose = false;
                break;
            }
        }
        if(choose){
            cliques.emplace_back(cli);
            for(NODE *n : cli){
                n->merged = true;
            }
        }
    }
}


int MBFF::Find_clique(){
    // sort by neighbor size from large to small
    struct MYCOMP {
        bool operator() (NODE* i, NODE* j) { return (i->neighbor.size() < j->neighbor.size());}
    } mycomp;

    vector<NODE*> fflist;
    fflist.reserve(FLIP_FLOP_LIST.size());
    for(auto FLIP_FLOP1: FLIP_FLOP_LIST){
        fflist.emplace_back(FLIP_FLOP1.second);
    }
    sort(fflist.begin(), fflist.end(), mycomp);

    // 4-bit clique
    // vector<vector<NODE*>> allcli;
    for(int i = 0; i < fflist.size(); i++){
        NODE *n = fflist[i];
        vector<NODE*> cli;
        vector<vector<NODE*>> targetcli;
        int curbit = 0, targetbit = 4;
        rec_clique(n, cli, curbit, targetbit, targetcli);
        
        // allcli.reserve(allcli.size() + targetcli.size());
        // for(auto vec : targetcli) allcli.emplace_back(vec);

        cli = choosetargetcli(targetcli);
        for(NODE *c : cli){
            c->merged = true;
        }
        if(cli.size() != 0)
            cliques.emplace_back(cli);
    }

    // chooseallcli(allcli);

    cout << "ori. cliques cnt: " << cliques.size() << endl;
    for(vector<NODE*> cli : cliques){
        NODE *newn = new NODE;
        newn->FF_name = "FF4";
        newn->NODE_name = "FF4_" + to_string(nameidx++);
        for(NODE *c : cli){
            for(NODE *pin : c->FFpin){
                newn->FFpin.emplace_back(pin);
            }
        }

        Rect inter = cli[0]->FRegion;
        for(size_t i = 1; i < cli.size(); i++){
            inter = Intersect(inter, cli[i]->FRegion);
            if(inter.intersect == false){
                cout << "FF_Feasible_Region rect error!!" << endl;
                exit(1);
            }
        }
        newn->FRegion = inter;
        FinalFF.emplace_back(newn);
    }
    cliques.clear();
    // allcli.clear();

    // 2-bit clique with ff1
    for(NODE *n : fflist){
        if(LIBRARY[n->FF_name]->BIT_NUMBER != 1) continue;
        vector<NODE*> cli;
        vector<vector<NODE*>> targetcli;
        int curbit = 0, targetbit = 2;
        rec_clique(n, cli, curbit, targetbit, targetcli);
        cli = choosetargetcli(targetcli);

        // allcli.reserve(allcli.size() + targetcli.size());
        // for(auto vec : targetcli) allcli.emplace_back(vec);
        for(NODE *c : cli){
            c->merged = true;
        }
        if(cli.size() != 0)
            cliques.emplace_back(cli);
    }
    
    // chooseallcli(allcli);

    for(vector<NODE*> cli : cliques){
        NODE *newn = new NODE;
        newn->FF_name = "FF2";
        newn->NODE_name = "FF2_" + to_string(nameidx++);
        for(NODE *c : cli){
            for(NODE *pin : c->FFpin){
                newn->FFpin.emplace_back(pin);
            }
        }

        Rect inter = cli[0]->FRegion;
        for(size_t i = 1; i < cli.size(); i++){
            inter = Intersect(inter, cli[i]->FRegion);
            if(inter.intersect == false){
                cout << "FF_Feasible_Region rect error!!" << endl;
                exit(1);
            }
        }
        newn->FRegion = inter;
        FinalFF.emplace_back(newn);
    }
    cliques.clear();
    // allcli.clear();

    // un-merge node    
    for(NODE *n : fflist){
        if(n->merged) continue;
        
        FinalFF.emplace_back(n);
    }

    int power = 0;
    for(NODE *n : FinalFF){
        auto got = LIBRARY.find(n->FF_name);
        if(got == LIBRARY.end()) cout << "miss" << endl;
        power += LIBRARY[n->FF_name]->POWER_CONSUMPTION;
    }
    cout << "opt. power: " << power << endl;
    return power;
}
// ===== clique =====


// ===== pre-process =====
void MBFF::FF_Feasible_Region(){
    for(auto FLIP_FLOP: FLIP_FLOP_LIST){
        NODE *ff = FLIP_FLOP.second;
        vector<Rect> rects;
        for(NODE *pin : ff->FFpin){
            int slack = pin->pinslack;
            int maxdis = slack + ff->pos.HPWL(pin->pos);
            pin->pinslack = maxdis;
            Rect rect;
            rect.ll.x = pin->pos.x;
            rect.ll.y = pin->pos.y - maxdis;
            rect.tr.x = pin->pos.x;
            rect.tr.y = pin->pos.y + maxdis;
            Rotate45(rect);
            rects.emplace_back(rect);
        }
        Rect inter = rects[0];
        for(size_t i = 1; i < rects.size(); i++){
            inter = Intersect(inter, rects[i]);
            
            if(inter.intersect == false){
                cout << "~FF_Feasible_Region rect error!!" << endl;
                exit(1);
            }
        }
        ff->FRegion = inter;
    }
}    

Rect MBFF::Intersect(Rect a, Rect b){
    Rect res;

    res.ll.x = max(a.ll.x, b.ll.x);
    res.tr.x = min(a.tr.x, b.tr.x);
    res.ll.y = max(a.ll.y, b.ll.y);
    res.tr.y = min(a.tr.y, b.tr.y);
    
    if (res.ll.x <= res.tr.x && res.ll.y <= res.tr.y){
        res.intersect = true;
    }
    else{        
        res.intersect = false;
    }

    return res;
}

void MBFF::Rotate45(Rect &rect){
    POS ll = rect.ll;
    POS tr = rect.tr;
    rect.ll.x = ll.x + ll.y;
    rect.ll.y = ll.y - ll.x;
    rect.tr.x = tr.x + tr.y;
    rect.tr.y = tr.y - tr.x;
}

void MBFF::ConstructOverlapGraph(){
    vector<NODE*> fflist;
    fflist.reserve(FLIP_FLOP_LIST.size());
    for(auto FLIP_FLOP1: FLIP_FLOP_LIST){
        fflist.emplace_back(FLIP_FLOP1.second);
    }
    for(int i = 0; i < (int) fflist.size() - 1; i++){
        NODE *ff1 = fflist[i];
        for(int j = i + 1; j < (int) fflist.size(); j++){
            NODE *ff2 = fflist[j];
            Rect inter = Intersect(ff1->FRegion, ff2->FRegion);
            if(inter.intersect == true){
                if(inter.tr.x - inter.ll.x < GRID_SIZE.x) continue;
                if(inter.tr.y - inter.ll.y < GRID_SIZE.y) continue;
                ff1->neighbor[ff2->NODE_name] = ff2;
                ff2->neighbor[ff1->NODE_name] = ff1;
            }
        }
    }
}
// ===== pre-process =====

// ===== logger =====
int MBFF::PrintFF(){
    cout << "\nFFcount: " << FLIP_FLOP_LIST.size() << " ";
    unordered_map<string, int> LIBcnt;
    int power = 0;
    for(auto LIB : LIBRARY){
        LIBcnt[LIB.first] = 0;
    }

    for(auto FLIP_FLOP: FLIP_FLOP_LIST){
        NODE *ff = FLIP_FLOP.second;
        if(ff->merged) continue;
        LIBcnt[ff->FF_name]++;
        power += LIBRARY[ff->FF_name]->POWER_CONSUMPTION;
    }
    for(auto cnt : LIBcnt){
        cout << cnt.first << ":" << cnt.second << " ";
    }
    cout << endl;
    cout << "power: " << power << endl;
    return power;
}
void MBFF::PrintLIB(){
    cout << "\nLIBRARY count: " << LIBRARY.size() << " -- ";
    for(auto LIB : LIBRARY){
        cout << LIB.first << " ";
    }
    cout << endl;
}
// ===== logger =====

// ===== Parser =====
void MBFF::Parser_ICCAD2010(string Inputstr){
    ifstream fin(Inputstr);
    Parser PS;
    string str;

    while(getline(fin, str)){        
        vector<string> tok = PS.getToken(str, " ");
        if(tok[0] == "CHIP_SIZE"){
            CHIP_SIZE_W = stoi(tok[1]);
            CHIP_SIZE_H = stoi(tok[3]);
        }
        else if(tok[0] == "GRID_SIZE"){
            GRID_SIZE.x = stoi(tok[1]);
            GRID_SIZE.y = stoi(tok[3]);
        }
        else if(tok[0] == "BIN_SIZE"){
            BIN_SIZE_BinW = stoi(tok[1]);
            BIN_SIZE_BinH = stoi(tok[3]);
        }
        else if(tok[0] == "PLACEMENT_DENSITY_CONSTRAINT"){
            PLACEMENT_DENSITY_CONSTRAINT = stoi(tok[1]);
        }
        else if(str == "[LIBRARY]"){
            Parser_ICCAD2010_LIBRARY(fin);
        }
        else if(str == "[FLIP_FLOP_LIST]"){
            Parser_ICCAD2010_FLIP_FLOP_LIST(fin);
        }
        else if(str == "[PIN_LIST]"){
            Parser_ICCAD2010_PIN_LIST(fin);
        }
        else if(str == "[NET_LIST]"){
            Parser_ICCAD2010_NET_LIST(fin);            
        }
    }


    fin.close();
}
void MBFF::Parser_ICCAD2010_LIBRARY(ifstream &fin){   
    Parser PS; 
    string str;
    while(getline(fin, str)){
        if(str == "[END LIBRARY]") break;

        vector<string> tok = PS.getToken(str, " []");
        if(tok[0] == "FLIP_FLOP"){
            FLIP_FLOP *ff = new FLIP_FLOP;
            ff->FF_name = string(tok[1]);
            LIBRARY[ff->FF_name] = ff;
            
            getline(fin, str);
            tok = PS.getToken(str, " ");
            if(tok[0] == "BIT_NUMBER"){
                ff->BIT_NUMBER = stoi(tok[1]);
            }

            getline(fin, str);
            tok = PS.getToken(str, " ");
            if(tok[0] == "POWER_CONSUMPTION"){
                ff->POWER_CONSUMPTION = stoi(tok[1]);
            }

            getline(fin, str);
            tok = PS.getToken(str, " ");
            if(tok[0] == "AREA"){
                ff->AREA = stoi(tok[1]);
            }
        }
    }
}
void MBFF::Parser_ICCAD2010_FLIP_FLOP_LIST(ifstream &fin){
    Parser PS;
    string str;
    while(getline(fin, str)){
        if(str == "[END FLIP_FLOP_LIST]") break;
        
        vector<string> tok = PS.getToken(str, " (,)");
        NODE *node = new NODE;
        node->FF_name = string(tok[0]);
        node->NODE_name = string(tok[1]);
        node->pos.x = stoi(tok[2]);
        node->pos.y = stoi(tok[3]);
        FLIP_FLOP_LIST[node->NODE_name] = node;
    }
}
void MBFF::Parser_ICCAD2010_PIN_LIST(ifstream &fin){
    Parser PS;
    string str;
    while(getline(fin, str)){
        if(str == "[END PIN_LIST]") break;
        
        vector<string> tok = PS.getToken(str, " (,)");
        NODE *node = new NODE;
        node->FF_name = string(tok[0]);
        node->NODE_name = string(tok[1]);
        node->pos.x = stoi(tok[2]);
        node->pos.y = stoi(tok[3]);
        PIN_LIST[node->NODE_name] = node;
    }
}
void MBFF::Parser_ICCAD2010_NET_LIST(ifstream &fin){
    Parser PS;
    string str;
    while(getline(fin, str)){
        if(str == "[END NET_LIST]") break;
        
        vector<string> tok = PS.getToken(str, " ");
        string pin_name = tok[0];
        string ff_name = tok[1];
        int slack = stoi(tok[2]);

        NODE *pin = PIN_LIST[pin_name];
        pin->pinslack = slack;
        NODE *ff = FLIP_FLOP_LIST[ff_name];
        ff->FFpin.emplace_back(pin);
    }
}
// ===== Parser =====