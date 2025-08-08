#include "bstar.hpp"

Bstar::Bstar(FloorPlanData FPD)
    : FPD(FPD) {
    
    random_shuffle(FPD.macros.begin(), FPD.macros.end());
    InitTree(FPD.macros);
    // for(NODE *n: FPD.macros){
    //     cout << n->name << " ";
    //     if(n->parent != NULL) cout << n->parent->name << " ";
    //     else cout << "x ";
    //     if(n->left != NULL) cout << n->left->name << " ";
    //     else cout << "x ";
    //     if(n->right != NULL) cout << n->right->name << endl;
    //     else cout << "x " << endl;
    // }
    

}

void Bstar::dump(string dir, double alpha, double time){
    ofstream fout(dir);
    size_t area = bestW*bestH;
    size_t wire = ComputeWire();
    int cost = alpha * (double) area + (1-alpha) * (double) wire;

    fout << cost << endl;
    fout << wire << endl;
    fout << area << endl;
    fout << bestW << " " << bestH << endl;
    fout << time << endl;

    for(NODE *n : FPD.macros){
        fout << n->name << " " << n->x << " " << n->y << " " << n->x + n->w << " " << n->y + n->h << endl;
    }

    fout.close();
}

void Bstar::perturb(){
    int op = rand() % 3 + 1;
    
    if(op == 1){
        int target = rand() % FPD.macros.size();
        OP1_rotate(FPD.macros.at(target));
    }
    if(op == 2){
        NODE *target1; // find leaf
        do{
            target1 = FPD.macros[rand() % FPD.macros.size()];
        } while(target1->left != NULL || target1->right != NULL);
        
        NODE *target2; // find child has at least one null
        do{
            target2 = FPD.macros[rand() % FPD.macros.size()];
        } while(target2 == target1 || (target2->left != NULL && target2->right !=NULL));
        
        int LR;
        if(target2->left == NULL && target2->right == NULL) LR = (rand() % 2) == 0 ? _LEFT : _RIGHT; 
        else if(target2->right == NULL) LR = _RIGHT;
        else LR = _LEFT;

        OP2_move(target1, target2, LR);
    }
    if(op == 3){
        NODE *target; // find not leaf
        int LR;
        do{
            target = FPD.macros[rand() % FPD.macros.size()];
        }while(target->left == NULL && target->right == NULL);

        if(target->left == NULL)
            LR = _RIGHT;
        else if(target->right == NULL)
            LR = _LEFT;
        else
            LR = (rand() % 2 == 0) ? _LEFT : _RIGHT;

        OP3_swap(target, LR);
    }
}

void Bstar::copy(int mode){
    for(NODE *n : FPD.macros){
        if(mode == _OLD){
            n->old_w = n->w; n->old_h = n->h; n->old_x = n->x; n->old_y = n->y;
            n->old_parent = n->parent; n->old_left = n->left; n->old_right = n->right;
            old_root = root;
        }
        if(mode == _BEST){
            n->best_w = n->w; n->best_h = n->h; n->best_x = n->x; n->best_y = n->y;
            n->best_parent = n->parent; n->best_left = n->left; n->best_right = n->right;
            best_root = root;
        }
        if(mode == _RECOVER){
            n->w = n->old_w; n->h = n->old_h; n->x = n->old_x; n->y = n->old_y;
            n->parent = n->old_parent; n->left = n->old_left; n->right = n->old_right;
            root = old_root;
        }
        if(mode == _RESULT){
            n->w = n->best_w; n->h = n->best_h; n->x = n->best_x; n->y = n->best_y;
            n->parent = n->best_parent; n->left = n->best_left; n->right = n->best_right;
            root = best_root;
        }
    }
}
double Bstar::coss_function(size_t area, size_t wire){
    double cost = FPD.alpha * (double) area;
    if(best_wire != 0)
        cost += (1-FPD.alpha) * (double) wire;
    
    return cost;
}
double Bstar::overlap_coss(){
    double cost = 0;
    for(NODE *n : FPD.macros){
        if(n->x > FPD.outlineX) cost += n->w*n->h;
        else if(n->y > FPD.outlineY) cost += n->w*n->h;
        else{
            if(n->x + n->w > FPD.outlineX) cost += (n->x + n->w - FPD.outlineX)*n->h;
            else if(n->y + n->h > FPD.outlineY) cost += n->w*(n->y + n->h - FPD.outlineY);
        }
    }
    return cost;
}

double Bstar::Inline_SA(){    
    random_device rd;
    mt19937 gen = mt19937(rd());
    uniform_real_distribution<> dis(0, 1); //平均分佈
    auto randfun = bind(dis, gen);

    size_t maxW, maxH, area, wire;
    ComputeMaxWH(maxW, maxH);
    area = maxW * maxH;
    wire = ComputeWire();
    avg_area = area;
    avg_wire = wire;
    
    double cost = coss_function(area, wire);
    
    double old_cost = cost, best_cost = cost;

    size_t i = 0, acctime = 0;

    double T = 100, dt = 0.99;
    while(T > 1){
        copy(_OLD);
        old_cost = cost;

        perturb();
        ComputeMaxWH(maxW, maxH);        
        area = maxW * maxH;
        wire = ComputeWire();
        
        cost = coss_function(area, wire);
        double ran = randfun();
        double proba=exp(-((double)cost-(double)old_cost)/T);
        if(proba>1)proba=1;
        if(overlap_coss() == 0 && (cost < old_cost || ran < proba)){
            acctime++;
            if(cost < best_cost){
                best_cost = cost;
                copy(_BEST);
            }
        }            
        else{
            cost = old_cost;
            copy(_RECOVER);
        }
        
        i++;
        if(i % 1000 == 0){
            i = 0;
        }
        if(i % 100 == 0){
            T *= dt;
        }
    }
    copy(_RESULT);
    ComputeMaxWH(maxW, maxH);
    bestW = maxW;
    bestH = maxH;
    wire = ComputeWire();
    return coss_function(area, wire);
}

void Bstar::Area_SA(){
    random_device rd;
    mt19937 gen = mt19937(rd());
    uniform_real_distribution<> dis(0, 1); //平均分佈
    auto randfun = bind(dis, gen);

    size_t maxW, maxH, area, wire;
    ComputeMaxWH(maxW, maxH);
    area = maxW * maxH;
    wire = ComputeWire();
    avg_area = area;
    avg_wire = wire;
    
    double cost = overlap_coss();
    double old_cost = cost, best_cost = cost;

    size_t i = 0, acctime = 0;

    double T = 100, dt = 0.99;
    while(T > 1){
        copy(_OLD);
        old_cost = cost;

        perturb();
        ComputeMaxWH(maxW, maxH);        
        area = maxW * maxH;
        wire = ComputeWire();
        cost = overlap_coss();
        
        double ran = randfun();
        double proba=exp(-((double)cost-(double)old_cost)/T);
        if(proba>1)proba=1;
        if(cost < old_cost || ran < proba){
            acctime++;
            if(cost < best_cost){
                best_cost = cost;
                copy(_BEST);
            }
        }            
        else{
            cost = old_cost;
            copy(_RECOVER);
        }
        
        if(i % 100 == 0){
            T *= dt;
        }
        if(!(T>1)){
            if(cost != 0){
                T = 100;
                Packing();
                ComputeMaxWH(maxW, maxH);
                cost = overlap_coss();
            }
        }
    }
    copy(_RESULT);
    ComputeMaxWH(maxW, maxH);
    bestW = maxW;
    bestH = maxH;
    cost = overlap_coss();
    wire = ComputeWire();
    best_wire = wire;
    area = maxW * maxH;
    cout << "area sa: " << coss_function(maxW * maxH, wire) << endl;
}

void Bstar::Packing(){
    for(NODE* n : FPD.macros){
        NODE *parent, *gran;
        parent = n->parent;
        if(parent == NULL) continue; // no parent, is root
        if(parent->left == n) continue; // same y

        gran = parent->parent;
        if(gran == NULL) continue; // no gran
        if(gran->right != NULL) continue; // gran has top module
        if(n->y < gran->y + gran->h) continue; // gran is taller than n

        parent->right = NULL;
        n->parent = gran;
        gran->right = n;
    }
}

void Bstar::OP1_rotate(NODE *n){
    swap(n->w, n->h);
}
void Bstar::OP3_swap(NODE *n, int LR){
    if(LR == _LEFT) {
        NODE *c = n->left;
        NODE *nP = n->parent;
        NODE *nR = n->right;
        NODE *cL = c->left;
        NODE *cR = c->right;

        if(cL != NULL) cL->parent = n;
        if(cR != NULL) cR->parent = n;
        if(nR != NULL) nR->parent = c;

        if(nP != NULL){
            if(nP->left == n) nP->left = c;
            if(nP->right == n) nP->right = c;
        }

        n->parent = c;
        n->left = cL;
        n->right = cR;

        c->parent = nP;
        c->left = n;
        c->right = nR;

        if(n == root) root = c;
    }
    if(LR == _RIGHT) {
        NODE *c = n->right;
        NODE *nP = n->parent;
        NODE *nL = n->left;
        NODE *cL = c->left;
        NODE *cR = c->right;

        if(cL != NULL) cL->parent = n;
        if(cR != NULL) cR->parent = n;
        if(nL != NULL) nL->parent = c;

        if(nP != NULL){
            if(nP->left == n) nP->left = c;
            if(nP->right == n) nP->right = c;
        }

        n->parent = c;
        n->left = cL;
        n->right = cR;

        c->parent = nP;
        c->left = nL;
        c->right = n;
        
        if(n == root) root = c;
    }


}
void Bstar::OP2_move(NODE *n, NODE *place, int LR){
    // n is leaf, place has null child at LR
    if(n->parent->left == n) n->parent->left = NULL;
    if(n->parent->right == n) n->parent->right = NULL;

    if(LR == _LEFT){
        place->left = n;
        n->parent = place;
    }
    if(LR == _RIGHT){
        place->right = n;
        n->parent = place;
    }
}

void Bstar::OP3_swap_two_modules(NODE *n1, NODE *n2){
    swap(n1->w, n2->w); swap(n1->h, n2->h); swap(n1->x, n2->x); swap(n1->y, n2->y);
    swap(n1->old_w, n2->old_w); swap(n1->old_h, n2->old_h); swap(n1->old_x, n2->old_x); swap(n1->old_y, n2->old_y);
    swap(n1->best_w, n2->best_w); swap(n1->best_h, n2->best_h); swap(n1->best_x, n2->best_x); swap(n1->best_y, n2->best_y);
}

void Bstar::InitTree(vector<NODE*> &macros){
    // construct complete tree
    root = macros.at(0);
    for(size_t i = 0; i < macros.size(); i++){
        NODE *n = macros.at(i);
        n->x = 0;
        n->y = 0;
        size_t indexL = i*2+1, indexR = i*2+2;
        if(indexL < macros.size()){
            n->left = macros.at(indexL);
            macros.at(indexL)->parent = n;
        }
        if(indexR < macros.size()){
            n->right = macros.at(indexR);
            macros.at(indexR)->parent = n;
        }
    }
}

void Bstar::ComputeMaxWH(size_t &maxW, size_t &maxH){
    maxW = 0;
    maxH = 0;
    root->x = 0;
    root->y = 0;
    list<NODE*> contour;    
    Traverse(root, 0, 0, contour, maxW, maxH);
}
size_t Bstar::ComputeWire(){
    size_t hpwl = 0;
    for(NET& net : FPD.nets){
        size_t minX = UINT_MAX, minY = UINT_MAX;
        size_t maxX = 0, maxY = 0;

        for(NODE *n : net){
            size_t x = 0, y = 0;
            if(n->type == _TERMINAL){
                x = n->x;
                y = n->y;
            }
            else if(n->type == _MACRO){
                x = n->x + n->w/2;
                y = n->y + n->h/2;
            }
            minX = min(minX, x);
            minY = min(minY, y);
            maxX = max(maxX, x);
            maxY = max(maxY, y);
        }
        hpwl += (maxX-minX + maxY-minY);
    }
    return hpwl;
}

void Bstar::UpdateContour(NODE *n, list<NODE*> &contour){
    if(contour.size() == 0) {
        contour.push_back(n);
        return;
    }

    list<NODE*>::iterator start = contour.end();

    for(auto it = contour.begin(); it != contour.end(); it++){
        NODE *cur = *it;
        if(n->x == cur->x){
            start = it;
            break;
        }
        if(n->x == cur->x + cur->w){
            start = ++it;
            break;
        }
    }
    
    // remove cover
    size_t maxY = 0;
    if(n->parent->left == n) maxY = n->parent->y;
    if(n->parent->right == n) maxY = n->parent->y + n->parent->h;

    while(start != contour.end()){
        if(n->x + n->w >= (*start)->x + (*start)->w){
            maxY = max(maxY, (*start)->y + (*start)->h);
            start = contour.erase(start);
        }
        else{
            break;
        }
    }

    if(start != contour.end()){
        if(n->x + n->w > (*start)->x){
            maxY = max(maxY, (*start)->y + (*start)->h);
        }
    }

    contour.insert(start, n);

    n->y = maxY;
    

}

void Bstar::Traverse(NODE *n, size_t parent_x, size_t parent_w, list<NODE*> &contour, size_t &maxW, size_t &maxH){
    if(n == NULL) return;
    
    n->x = parent_x + parent_w;
    UpdateContour(n, contour); // y is set in this function

    maxW = max(maxW, n->x + n->w);
    maxH = max(maxH, n->y + n->h);

    Traverse(n->left, n->x, n->w, contour, maxW, maxH);
    Traverse(n->right, n->x, 0, contour, maxW, maxH);
}


void Bstar::Visualize(string dir){
    ofstream fout(dir);
    fout << FPD.macros.size() << endl;

    size_t maxX = 0, maxY = 0;
    for(NODE *n : FPD.macros){
        maxX = max(n->x + n->w, maxX);
        maxY = max(n->y + n->h, maxY);
    }

    fout << FPD.outlineX << " " << FPD.outlineY << " " << maxX << " " << maxY << endl;
    for(NODE *n : FPD.macros){
        fout << n->name << " " << n->x << " " << n->y << " " << n->w << " " << n->h << endl;
    }

    fout.close();
}