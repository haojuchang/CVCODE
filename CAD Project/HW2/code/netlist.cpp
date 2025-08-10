#include "netlist.h"




NETLIST::NETLIST(){
    
}


// step1: loading of each gate
void NETLIST::CalculateOutputLoading(){
    for (auto& x: allNode){
        NODE *node = x.second;
        if(node->type == _NET) continue;

        NODE *net = node->FO[0];
        
        for(int i = 0; i < net->FO.size(); i++){
            NODE *FOgate = net->FO[i];
            string pin = net->FOpin[i];
            node->outputLoading += GetInputCapacitance(FOgate, pin);
        }
        
        if(POnet.find(net) != POnet.end()){
            node->outputLoading += _POoutload;
        }
    }
}
bool cmp(NODE* a, NODE* b){
    return a->name < b->name;
}
void NETLIST::GetSortedGate(vector<NODE*> &nodes){    
    for (auto& x: allNode){
        NODE *node = x.second;
        if(node->type == _NET) continue;
        nodes.push_back(node);
    }
    sort(nodes.begin(), nodes.end(), cmp);
}

void NETLIST::WriteLoadFile(string Dir){
    ofstream out(Dir);

    vector<NODE*> nodes;
    GetSortedGate(nodes);

    for(int i = 0; i < nodes.size(); i++){
        out << nodes[i]->name<< " " << nodes[i]->outputLoading;
        if(i != nodes.size()-1) out << "\n";
    }
    out.close();
}
double NETLIST::GetInputCapacitance(NODE *gate, string pin){
    return LIB[gate->type]->capacitance[pin];
}

// step2: calculate delay under patterns
void NETLIST::CalculateDelayUnderPats(){
    for(NODE* n: POnet){
        GateOutputInference(n);
    }
    for(NODE* n: POnet){
        GateDelayInference(n);
    }
    
}
void NETLIST::GateDelayInference(NODE *n){
    
    
    if(n->type == _NET){
        if(n->FI.size() == 0) return; // PInet
        GateDelayInference(n->FI[0]);
    }
    else{ // gates
        if(n->propagation.size() != 0) return;
        if (n->type == _INVX1){
            NODE *f0 = n->FI[0];
            
            if(f0->FI.size() != 0)
                if(f0->FI[0]->propagation.size() == 0) GateDelayInference(f0);
            vector<double> iptTrans;
            GetIptTrans(f0, iptTrans);
            vector<double> iptDelay0;
            GetIptDelay(f0, iptDelay0);
            double trans, pre_delay;

            for(int i = 0; i < n->output.size(); i++){
                trans = iptTrans[i];
                pre_delay = iptDelay0[i];

                vector<vector<double>> cell_tab, trans_tab;
                if(n->output[i] == 0){
                    cell_tab = LIB[_INVX1]->cell_fall;
                    trans_tab = LIB[_INVX1]->fall_transition;
                }
                else{ // n->output[i] == 1
                    cell_tab = LIB[_INVX1]->cell_rise;
                    trans_tab = LIB[_INVX1]->rise_transition;
                }
                
                double p = CheckTable(cell_tab, trans, n->outputLoading);
                double s = CheckTable(trans_tab, trans, n->outputLoading);

                n->propagation.push_back(p);
                n->transition.push_back(s);
                n->delay.push_back(pre_delay+p);
                n->path.push_back(f0);
            }
            
        }
        else if (n->type == _NOR2X1){
            NODE *f0 = n->FI[0], *f1 = n->FI[1], *pt;
            if(f0->FI.size() != 0)
                if(f0->FI[0]->propagation.size() == 0) GateDelayInference(f0);
            if(f1->FI.size() != 0)
                if(f1->FI[0]->propagation.size() == 0) GateDelayInference(f1);
            vector<double> iptTrans0, iptTrans1;
            GetIptTrans(f0, iptTrans0);
            GetIptTrans(f1, iptTrans1);
            vector<double> iptDelay0, iptDelay1;
            GetIptDelay(f0, iptDelay0);
            GetIptDelay(f1, iptDelay1);
            double trans, pre_delay;

            for(int i = 0; i < n->output.size(); i++){
                vector<vector<double>> cell_tab, trans_tab;
                if(f0->output[i] == 0 && f1->output[i] == 0){ // 00
                    trans = (iptDelay0[i] > iptDelay1[i]) ? iptTrans0[i] : iptTrans1[i];
                    pre_delay = (iptDelay0[i] > iptDelay1[i]) ? iptDelay0[i] : iptDelay1[i];
                    pt = (iptDelay0[i] > iptDelay1[i]) ? f0 : f1;
                    
                    if(f0->FI.size() == 0 && f1->FI.size() == 0) {
                        // both input is PO
                        pt = (f0->name < f1->name)? f0 : f1;
                    }
                }
                else if(f0->output[i] == 0 && f1->output[i] == 1){ // 01
                    trans = iptTrans1[i];
                    pre_delay = iptDelay1[i];
                    pt = f1;
                }
                else if(f0->output[i] == 1 && f1->output[i] == 0){ // 10
                    trans = iptTrans0[i];
                    pre_delay = iptDelay0[i];
                    pt = f0;
                }
                else if(f0->output[i] == 1 && f1->output[i] == 1){ // 11
                    trans = (iptDelay0[i] < iptDelay1[i]) ? iptTrans0[i] : iptTrans1[i];
                    pre_delay = (iptDelay0[i] < iptDelay1[i]) ? iptDelay0[i] : iptDelay1[i];
                    pt = (iptDelay0[i] < iptDelay1[i]) ? f0 : f1;

                    if(f0->FI.size() == 0 && f1->FI.size() == 0) {
                        // both input is PO
                        pt = (f0->name < f1->name)? f0 : f1;
                    }
                }

                if(n->output[i] == 0){
                    cell_tab = LIB[_NOR2X1]->cell_fall;
                    trans_tab = LIB[_NOR2X1]->fall_transition;
                }
                else{ // n->output[i] == 1
                    cell_tab = LIB[_NOR2X1]->cell_rise;
                    trans_tab = LIB[_NOR2X1]->rise_transition;
                }

                double p = CheckTable(cell_tab, trans, n->outputLoading);
                double s = CheckTable(trans_tab, trans, n->outputLoading);

                n->propagation.push_back(p);
                n->transition.push_back(s);
                n->delay.push_back(pre_delay+p);
                n->path.push_back(pt);
            }
            
        }
        else if (n->type == _NANDX1){
            NODE *f0 = n->FI[0], *f1 = n->FI[1], *pt;
            if(f0->FI.size() != 0)
                if(f0->FI[0]->propagation.size() == 0) GateDelayInference(f0);
            if(f1->FI.size() != 0)
                if(f1->FI[0]->propagation.size() == 0) GateDelayInference(f1);
            vector<double> iptTrans0, iptTrans1;
            GetIptTrans(f0, iptTrans0);
            GetIptTrans(f1, iptTrans1);
            vector<double> iptDelay0, iptDelay1;
            GetIptDelay(f0, iptDelay0);
            GetIptDelay(f1, iptDelay1);
            double trans, pre_delay;

            for(int i = 0; i < n->output.size(); i++){
                vector<vector<double>> cell_tab, trans_tab;
                if(f0->output[i] == 0 && f1->output[i] == 0){ // 00
                    trans = (iptDelay0[i] < iptDelay1[i]) ? iptTrans0[i] : iptTrans1[i];
                    pre_delay = (iptDelay0[i] < iptDelay1[i]) ? iptDelay0[i] : iptDelay1[i];                    
                    pt = (iptDelay0[i] < iptDelay1[i]) ? f0 : f1;
                    
                    if(f0->FI.size() == 0 && f1->FI.size() == 0) {
                        // both input is PO
                        pt = (f0->name < f1->name)? f0 : f1;
                    }
                }
                else if(f0->output[i] == 0 && f1->output[i] == 1){ // 01
                    trans = iptTrans0[i];
                    pre_delay = iptDelay0[i];
                    pt = f0;
                }
                else if(f0->output[i] == 1 && f1->output[i] == 0){ // 10
                    trans = iptTrans1[i];
                    pre_delay = iptDelay1[i];
                    pt = f1;
                }
                else if(f0->output[i] == 1 && f1->output[i] == 1){ // 11
                    trans = (iptDelay0[i] > iptDelay1[i]) ? iptTrans0[i] : iptTrans1[i];
                    pre_delay = (iptDelay0[i] > iptDelay1[i]) ? iptDelay0[i] : iptDelay1[i];
                    pt = (iptDelay0[i] > iptDelay1[i]) ? f0 : f1;

                    if(f0->FI.size() == 0 && f1->FI.size() == 0) {
                        // both input is PO
                        pt = (f0->name < f1->name)? f0 : f1;
                    }
                }

                if(n->output[i] == 0){
                    cell_tab = LIB[_NANDX1]->cell_fall;
                    trans_tab = LIB[_NANDX1]->fall_transition;
                }
                else{ // n->output[i] == 1
                    cell_tab = LIB[_NANDX1]->cell_rise;
                    trans_tab = LIB[_NANDX1]->rise_transition;
                }

                double p = CheckTable(cell_tab, trans, n->outputLoading);
                double s = CheckTable(trans_tab, trans, n->outputLoading);

                n->propagation.push_back(p);
                n->transition.push_back(s);
                n->delay.push_back(pre_delay+p);
                n->path.push_back(pt);
            }
        }
    }
}
double NETLIST::CheckTable(vector<vector<double>> &tab, double trans, double load){
    int t0, t1, c0, c1;
    int transSize = tab.size() - 1;
    int loadSize = tab[0].size() - 1;
    
    if(trans < tab[1][0]){
        t0 = 1;
        t1 = 2;
    }
    else if (trans > tab[transSize][0]){
        t0 = transSize - 1;
        t1 = transSize;
    }
    else {
        for(int i = 1; i < transSize; i++){
            if(trans >= tab[i][0] && trans <= tab[i + 1][0]){
                t0 = i;
                t1 = i + 1;
            }
        }
    }

    if(load < tab[0][1]){
        c0 = 1;
        c1 = 2;
    }
    else if (load > tab[0][loadSize]){
        c0 = loadSize - 1;
        c1 = loadSize;
    }
    else {
        for(int i = 1; i < loadSize; i++){
            if(load >= tab[0][i] && load <= tab[0][i + 1]){
                c0 = i;
                c1 = i + 1;
                break;
            }
        }
    }
    
    double a = Interpolation(tab[t0][0], tab[t1][0], tab[t0][c0], tab[t1][c0], trans);
    double b = Interpolation(tab[t0][0], tab[t1][0], tab[t0][c1], tab[t1][c1], trans);
    double y = Interpolation(tab[0][c0], tab[0][c1], a, b, load);
    return y;

}
double NETLIST::Interpolation(double x1, double x2, double y1, double y2, double x){
    double y = (y2 - y1) / (x2 - x1) * (x - x1) + y1;
    return y;
}
void NETLIST::GetIptTrans(NODE *n, vector<double> &iptTrans){
    // n is net type
    if(n->FI.size() == 0){
        iptTrans = vector<double>(n->output.size(), _PItrans);
    }
    else{        
        NODE *FIgate = n->FI[0];
        iptTrans = FIgate->transition;
    }
}
void NETLIST::GetIptDelay(NODE *n, vector<double> &iptTrans){
    // n is net type
    if(n->FI.size() == 0){
        iptTrans = vector<double>(n->output.size(), _PItrans);
    }
    else{        
        NODE *FIgate = n->FI[0];
        iptTrans = FIgate->delay;
    }
}
void NETLIST::GateOutputInference(NODE *n){
    if(n->output.size() != 0) return;

    for(int i = 0; i < n->FI.size(); i++){
        NODE* fi = n->FI[i];
        if(fi->output.size() == 0) GateOutputInference(fi);
    }

    if(n->type == _NET){
        NODE *fi = n->FI[0];
        n->output = fi->output;
    }
    else if(n->type == _INVX1){
        NODE *fi = n->FI[0];
        for(int i = 0; i < fi->output.size(); i++){
            int a = fi->output[i];
            int f = a ^ 1;
            n->output.push_back(f);
        }
    }
    else if(n->type == _NANDX1){
        NODE *f0 = n->FI[0];
        NODE *f1 = n->FI[1];
        for(int i = 0; i < f0->output.size(); i++){
            int a = f0->output[i];
            int b = f1->output[i];
            int f = (a & b) ^ 1;
            n->output.push_back(f);
        }
        
    }
    else if(n->type == _NOR2X1){
        NODE *f0 = n->FI[0];
        NODE *f1 = n->FI[1];
        for(int i = 0; i < f0->output.size(); i++){
            int a = f0->output[i];
            int b = f1->output[i];
            int f = (a | b) ^ 1;
            n->output.push_back(f);
        }        
    }
}
void NETLIST::WriteDelayFile(string Dir){
    ofstream out(Dir);

    vector<NODE*> nodes;
    GetSortedGate(nodes);
    
    int patCnt = nodes[0]->output.size();
    for(int p = 0; p < patCnt; p++){
        for(int i = 0; i < nodes.size(); i++){
            out << nodes[i]->name<< " ";
            out << nodes[i]->output[p] << " ";
            out << nodes[i]->propagation[p] << " ";
            out << nodes[i]->transition[p];
            if (i != nodes.size() - 1) out << "\n";
        }

        if(p != patCnt-1) out << "\n\n";
    }

    out.close();
}
void NETLIST::WriteLongestPath(string Dir){
    ofstream out(Dir);

    int outSize = (*POnet.begin())->output.size();
    for(int i = 0; i < outSize; i++){
        vector<string> pathName;
        NODE *cur = NULL;
        for(NODE *n: POnet){
            if(cur == NULL) cur = n;
            if(n->FI[0]->delay[i] > cur->FI[0]->delay[i]) cur = n;
        }

        out << "Longest delay = " << cur->FI[0]->delay[i] << ", the path is: ";

        while(true){
            if(cur->type == _NET){
                pathName.push_back(cur->name);
                if(cur->FI.size() != 0)
                    cur = cur->FI[0];
                else break;
            }
            else cur = cur->path[i];
        }
        for(int j = pathName.size() - 1; j >= 0; j--){
            out << pathName[j];
            if(j != 0)
                out << " -> ";
        }
        if(i != outSize - 1)
            out << "\n";
    }
    out.close();
}