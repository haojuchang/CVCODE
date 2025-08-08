#include "floorplan.h"

FloorPlan::FloorPlan(double alpha){
    this->alpha = alpha;
}
void FloorPlan::ParseBlock(string blockDir){
    ifstream in(blockDir);
    string str;
    vector<string> tok;
    // vector<vector<string>> toks;
    while(getline(in, str)){
        getToken(str, tok, ": \t");
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
            terminals.insert(t);
            nodes[t->name] = t;
        }
        else { // macro
            NODE *m = new NODE;
            m->ID = macroCnt++;
            m->name = tok[0];
            m->type = _MACRO;
            m->w = stoi(tok[1]);
            m->h = stoi(tok[2]);
            macros.insert(m);
            nodes[m->name] = m;
        }
    }
    in.close();
}
void FloorPlan::ParseNet(string netDir){
    ifstream in(netDir);
    string str;
    vector<string> tok;
    int netNum;

    while(getline(in, str)){
        getToken(str, tok, ": ");
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
                getToken(str, tok, ": ");
                NODE *node = nodes[tok[0]];
                n.push_back(node);
            }
            nets.push_back(n);
        }
    }
    in.close();
}
void FloorPlan::getToken(string str, vector<string> &tok, string split){
    size_t r = str.find("\r");
    if( r != string::npos)
        str.erase(r, 1);

    tok.clear();
    char *token = strtok((char *)str.c_str(), (char *)split.c_str());
    while (token != NULL)
    {
        tok.push_back(string(token));
        token = strtok(NULL, (char *)split.c_str());
    }
}
void FloorPlan::LCS(int mode, vector<NODE*> &left, vector<NODE*> &right, vector<int> &pos, int &longest){
    int n = left.size();
    vector<int> L(n, 0), P(n, 0), match(n, 0);
    // init. match array
    for(int i = 0; i < n; i++){
        match[right[i]->ID] = i;
    }
    for(int i = 0; i < n; i++){
        NODE* bnode = left[i];
        int b = bnode->ID;
        int p = match[b];
        P[b] = L[p];
        int t = 0;
        if(mode == 0) { // width
            t = P[b] + bnode->w;
        }
        else if(mode == 1){ // height
            t = P[b] + bnode->h;
        }
        for(int j = p; j < n; j++){
            if(t > L[j]){
                L[j] = t;
            }
            else 
                break;
        }
    }
    pos = P;
    longest = L[n-1];
}
void FloorPlan::initSP(vector<NODE*> &left, vector<NODE*> &right){
    int size = macros.size();
    left.reserve(size);
    right.reserve(size);

    for(NODE* x: macros){
        left.push_back(x);
        right.push_back(x);
    }

    random_shuffle(left.begin(), left.end());
    random_shuffle(right.begin(), right.end());
}
void FloorPlan::getPos(vector<NODE*> &left, vector<NODE*> &right, vector<int> &posX, vector<int> &posY, int &width, int &height){
    LCS(0, left, right, posX, width);
    vector<NODE*> rev_left = left;
    reverse(rev_left.begin(), rev_left.end());
    LCS(1, rev_left, right, posY, height);
}
void FloorPlan::calcost(unsigned long int &cost, double alpha, unsigned long int A, unsigned long int W){
    cost = (int) (alpha * A + (1-alpha) * W);
}
void FloorPlan::calarea(unsigned long int &A, int width, int height){
    A = width * height;
}
void FloorPlan::calwire(unsigned long int &W, vector<int> &posX, vector<int> &posY){
    W = 0;
    for(size_t i = 0; i < nets.size(); i++){
        int minX = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
        for(size_t j = 0; j < nets[i].size(); j++){
            NODE *node = nets[i][j];
            int x = 0, y = 0;
            if(node->type == _MACRO){
                x = posX[node->ID] + node->w / 2;
                y = posY[node->ID] + node->h / 2;
            }
            else if(node->type == _TERMINAL){
                x = node->x;
                y = node->y;
            }

            if(x < minX) minX = x;
            if(x > maxX) maxX = x;
            if(y < minY) minY = y;
            if(y > maxY) maxY = y;
        }
        W += (maxX-minX + maxY-minY);
    }
}
void FloorPlan::writeOutput(string Dir, vector<int> &posX, vector<int> &posY, int width, int height, double cpu_time_used){
    ofstream out(Dir);

    unsigned long int A, W, cost;
    calarea(A, width, height);
    calwire(W, posX, posY);
    calcost(cost, alpha, A, W);

    out << cost << "\n";
    out << W << "\n";
    out << A << "\n";
    out << width << " " << height << "\n";
    out << cpu_time_used << "\n";

    for(NODE* n: macros){
        out << n->name << " ";
        out << posX[n->ID] << " ";
        out << posY[n->ID] << " ";
        out << posX[n->ID] + n->w << " ";
        out << posY[n->ID] + n->h << "\n";
    }

    out.close();
}
void FloorPlan::SA(vector<NODE*> &left, vector<NODE*> &right, vector<int> &posX, vector<int>& posY, int &width, int &height){
    unsigned long int A, W, cost;
    getPos(left, right, posX, posY, width, height);
    costfunc(cost, posX, posY);
    // calarea(A, width, height);
    // calwire(W, posX, posY);
    // calcost(cost, alpha, A, W);

    unsigned long int new_A, new_W, new_cost;
    vector<NODE*> new_left, new_right;
    vector<int> new_posX, new_posY;
    int new_width, new_height;

    unsigned long int best_A, best_W, best_cost = cost;
    vector<NODE*> best_left, best_right;
    vector<int> best_posX, best_posY;
    int best_width, best_height;

    //double T = 100;
    double T = 2147483647;
    double dt = 0.99;
    random_device rd;
    mt19937 gen = mt19937(rd());
    uniform_real_distribution<> dis(0, 1); //平均分佈
    auto randfun = bind(dis, gen);

    while(true){
        for(int i = 0; i < 10; i++){
            move(new_left, new_right, left, right);
            getPos(new_left, new_right, new_posX, new_posY, new_width, new_height);
            costfunc(new_cost, new_posX, new_posY);
            // calarea(new_A, new_width, new_height);
            // calwire(new_W, new_posX, new_posY);
            // calcost(new_cost, alpha, new_A, new_W);
            
            double proba=exp(-((double)new_cost-(double)cost)/T);
            if(proba>1)proba=1;
            if(new_cost < cost || proba > randfun()){//|| exp(-(new_cost-cost)/T) > 1
            
                A = new_A; W = new_W; cost = new_cost;
                left = new_left; right = new_right;
                posX = new_posX; posY = new_posY;
                width = new_width; height = new_height;
            }
            // else{
            //     if (action == 0){ // rotate
            //         int t;
            //         t = new_left[actionMacro]->w;
            //         new_left[actionMacro]->w = new_left[actionMacro]->h;
            //         new_left[actionMacro]->h = t;
            //     }
            // }
            if(new_cost < best_cost){
                best_A = new_A; best_W = new_W; best_cost = new_cost;
                best_left = new_left; best_right = new_right;
                best_posX = new_posX; best_posY = new_posY;
                best_width = new_width; best_height = new_height;
            }
        }
        // cout << cost << endl;
        if(cost == 0) break;
        T *= dt;
    }

    A = best_A; W = best_W; cost = best_cost;
    left = best_left; right = best_right;
    posX = best_posX; posY = best_posY;
    width = best_width; height = best_height;
}
void FloorPlan::move(vector<NODE*> &new_left, vector<NODE*> &new_right, vector<NODE*> &left, vector<NODE*> &right){
    new_left = left;
    new_right = right;

    int n = new_left.size();
    int r = rand() % 2+1;
    
    // action = r;
    // if(r == 0){ // rotate
    //     int m = rand() % n;
    //     actionMacro = m;
    //     // cout << n << " " << m << endl;
    //     int t;
    //     t = new_left[m]->w;
    //     new_left[m]->w = new_left[m]->h;
    //     new_left[m]->h = t;
    // }
    if(r == 1){ // exchange left
        int i = 0, j = 0;
        while(i == j){
            i = rand() % n;
            j = rand() % n;
        }
        NODE *t;
        t = new_left[i];
        new_left[i] = new_left[j];
        new_left[j] = t;
    }
    else if(r == 2){ // exchange right
        int i = 0, j = 0;
        while(i == j){
            i = rand() % n;
            j = rand() % n;
        }
        NODE *t;
        t = new_right[i];
        new_right[i] = new_right[j];
        new_right[j] = t;
    }
}
void FloorPlan::costfunc(unsigned long int &cost, vector<int> &posX, vector<int> &posY){
    cost = 0;
    for(NODE *n: macros){
        int Lx = posX[n->ID];
        int Ly = posY[n->ID];
        int Rx = Lx + n->w;
        int Ry = Ly + n->h;
        if(Lx > outlineX || Ly > outlineY){ // all area
            cost += n->w * n->h;
        }
        else if(Lx < outlineX && Rx > outlineX && Ly < outlineY && Ry > outlineY){
            cost += n->w * n->h - (outlineX - Lx) * (outlineY - Ly);
        }
        else if(Lx < outlineX && Rx > outlineX){
            cost += (Rx - outlineX) * n->h;
        }
        else if(Ly < outlineY && Ry > outlineY){
            cost += n->w * (Ry - outlineY);
        }
    }
}