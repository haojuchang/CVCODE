#include "channel.hpp"


ChannelRoute::ChannelRoute(){

}

void ChannelRoute::Run(){
    ConstructVCG();
    ConstructHCG();
    bool cycle = CheckCycle();
    if(!cycle){
        RouteNoneCycle();
        RouteVlayer();
    }
    else{
        bool routesuccess;
        do{
            routesuccess = CycleRoute();
        }while(!routesuccess);
    }
}

bool ChannelRoute::CycleRoute(){

    vector<vector<size_t>> vmap, hmap;
    map<size_t, size_t> upassign; // key: pin, value: track id

    set<size_t> uppinset;
    for(size_t pin : uch) uppinset.insert(pin);
    vector<size_t> uppinvec;
    for(size_t pin : uppinset) uppinvec.emplace_back(pin);
    random_shuffle(uppinvec.begin(), uppinvec.end());

    for(size_t i = 0; i < uppinvec.size(); i++){
        size_t pin = uppinvec[i];
        if(pin == 0) continue;
        if(upassign.find(pin) == upassign.end()){
            hmap.emplace_back(vector<size_t>(uch.size(), pin));
            upassign[pin] = hmap.size() - 1;
        }
    }
    
    set<size_t> dnpinset;
    for(size_t pin : dch) dnpinset.insert(pin);
    vector<size_t> dnpinvec;
    for(size_t pin : dnpinset) dnpinvec.emplace_back(pin);
    random_shuffle(dnpinvec.begin(), dnpinvec.end());

    map<size_t, size_t> dnassign; // key: pin, value: track id
    for(size_t i = 0; i < dnpinvec.size(); i++){
        size_t pin = dnpinvec[i];
        if(pin == 0) continue;
        if(dnassign.find(pin) == dnassign.end()){
            hmap.emplace_back(vector<size_t>(dch.size(), pin));
            dnassign[pin] = hmap.size() - 1;
        }
    }

    vmap.resize(hmap.size(), vector<size_t>(uch.size(), 0));

    for(size_t i = 0; i < uch.size(); i++){
        size_t pin = uch[i];
        if(pin == 0) continue;

        for(size_t j = 0; j <= upassign[pin]; j++){
            vmap[j][i] = pin;
        }
    }

    for(size_t i = 0; i < dch.size(); i++){
        size_t pin = dch[i];
        if(pin == 0) continue;

        for(size_t j = vmap.size() - 1; j >= dnassign[pin]; j--){
            vmap[j][i] = pin;
        }
    }
    
    // struct NODEsort {
    //     NODEsort(map<size_t, size_t> upassign, map<size_t, size_t> dnassign)
    //         : upassign(upassign), dnassign(dnassign){}
    //     bool operator() (size_t i, size_t j, map<size_t, size_t> upassign, map<size_t, size_t> dnassign) {
    //         return (upassign[i] < dnassign[j]);
    //     }
    //     map<size_t, size_t> upassign, dnassign;
    // } nodesort(upassign, dnassign);

    // vector<size_t> allpin;
    // for(NODE *n : allnode){
    //     allpin.emplace_back(n->num);
    // }
    // sort(allpin.begin(), allpin.end(), nodesort);

    // start connect top down
    list<NODE*> allnodecopy = allnode;
    map<size_t, int> pincolmap;

    for(NODE *n : allnodecopy){
    // while(allnodecopy.size() != 0){
    //     NODE *n = *allnodecopy.begin();
    //     allnodecopy.pop_front();

        size_t pin = n->num;
        if(upassign.find(pin) == upassign.end() || dnassign.find(pin) == dnassign.end()) continue;
        size_t uptrack = upassign[pin];
        size_t dntrack = dnassign[pin];
        if(LOG) cout << pin << " " << uptrack << " " << dntrack << endl;
        bool route = false;
        for(size_t i = 0; i < uch.size(); i++){
            // if((vmap[uptrack+1][i] == 0 || vmap[uptrack+1][i] == pin) && ((vmap[dntrack-1][i] == 0) || vmap[dntrack-1][i] == pin)){
                bool ocupy = false;
                for(size_t j = uptrack; j <= dntrack; j++){
                    if(vmap[j][i] != 0 && vmap[j][i] != pin) {
                        ocupy = true;
                        break;
                    }
                }
                if(ocupy) continue;

                for(size_t j = uptrack; j <= dntrack; j++){
                    vmap[j][i] = pin;
                }
                pincolmap[pin] = i;
                if(LOG) cout << "i" << i << endl;
                route = true;
                break;
            // }
        }
        if(route) if(LOG) cout << "route success" << endl;
        if(!route){
            if(LOG) cout << "route failed " << n->num << endl;
            return false;
            // if(LOG) cout << pincolmap.size() << endl;
            // size_t nodeidx = rand() % pincolmap.size();
            // auto it = pincolmap.begin();
            // for(int i = 0; i < nodeidx; i++) ++it;
            // size_t ripnode = (*it).first;
            // if(LOG) cout << "rip node: " << ripnode << endl;

        }
    }

    vlayer.emplace_back(uch);
    for(vector<size_t> v : vmap)
        vlayer.emplace_back(v);
    vlayer.emplace_back(dch);

    hlayer.emplace_back(uch);
    for(vector<size_t> h : hmap)
        hlayer.emplace_back(h);
    hlayer.emplace_back(dch);

    // print map
    for(size_t pin : uch) if(LOG) cout << setw(3) << pin << " ";
    if(LOG) cout << endl << endl;
    for(size_t i = 0; i < hmap.size(); i++){
        for(size_t j = 0; j < hmap[i].size(); j++){
            if(LOG) cout << setw(3) << hmap[i][j] << " ";
        }
        if(LOG) cout << endl;
    }
    if(LOG) cout << endl;
    for(size_t pin : dch) if(LOG) cout << setw(3) << pin << " ";
    if(LOG) cout << endl;
    if(LOG) cout << "\n=====\n";
    for(size_t pin : uch) if(LOG) cout << setw(3) << pin << " ";
    if(LOG) cout << endl;
    if(LOG) cout << endl;
    for(size_t i = 0; i < vmap.size(); i++){
        for(size_t j = 0; j < vmap[i].size(); j++){
            if(LOG) cout << setw(3) << vmap[i][j] << " ";
        }
        if(LOG) cout << endl;
    }
    if(LOG) cout << endl;
    for(size_t pin : dch) if(LOG) cout << setw(3) << pin << " ";
    if(LOG) cout << endl;

    return true;
}

bool ChannelRoute::CheckCycle(){
    if(LOG) cout << "===== CheckCycle start =====" << endl;
    list<NODE*> VCG = allnode;

    for(NODE *n : VCG){
        n->VCG_FI_copy = n->VCG_FI;
        n->VCG_FO_copy = n->VCG_FO;
    }

    while(VCG.size() != 0){
        // find no fanin node
        bool cycle = true;
        for(auto it = VCG.begin(); it != VCG.end(); it++){
            NODE *n = *it;
            if(n->VCG_FI_copy.size() == 0){ // erase n
                if(LOG) cout << n->num << endl;
                for(pair<size_t, NODE*> fo : n->VCG_FO_copy){ // erase n from it po
                    fo.second->VCG_FI_copy.erase(n->num);
                }
                VCG.erase(it);
                cycle = false;
                break;
            }
        }
        if(cycle){
            // find most fi node
            auto breakit = VCG.end();
            size_t ficount = 0;
            for(auto it = VCG.begin(); it != VCG.end(); it++){
                NODE *n = *it;
                if(n->VCG_FI_copy.size() > ficount){
                    ficount = n->VCG_FI_copy.size();
                    breakit = it;
                }
            }
            if(LOG) cout << "breaknode: " << (*breakit)->num << " ficount:" << ficount << endl;
            
            for(pair<size_t, NODE*> fo : (*breakit)->VCG_FO_copy){ // erase n from it po
                fo.second->VCG_FI_copy.erase((*breakit)->num);
            }
            VCG.erase(breakit);
            
            // break;
            return true;
        }
    }
    if(LOG) cout << "===== CheckCycle end =====" << endl;
    return false;
}

void ChannelRoute::RouteNoneCycle(){
    list<NODE*> VCG = allnode;

    for(NODE *n : VCG){
        n->VCG_FI_copy = n->VCG_FI;
        n->VCG_FO_copy = n->VCG_FO;
    }

    // init layer
    // vlayer can only route vertical
    // hlayer can only route horizontal
    vlayer.emplace_back(uch);
    vlayer.emplace_back(dch);
    hlayer.emplace_back(uch);
    hlayer.emplace_back(dch);

    while(VCG.size() != 0){
        // find no fanin node
        bool cycle = true;
        vector<NODE*> nofinode;
        vector<list<NODE*>::iterator> nofit;

        for(auto it = VCG.begin(); it != VCG.end(); it++){
            NODE *n = *it;
            if(n->VCG_FI_copy.size() == 0){ // erase n
                nofinode.emplace_back(n);
                nofit.emplace_back(it);
                cycle = false;
            }
        }
        if(LOG) cout << endl;

        if(cycle){
            // find most fi node
            auto breakit = VCG.end();
            size_t ficount = 0;
            for(auto it = VCG.begin(); it != VCG.end(); it++){
                NODE *n = *it;
                if(n->VCG_FI_copy.size() > ficount){
                    ficount = n->VCG_FI_copy.size();
                    breakit = it;
                }
            }

            if(LOG) cout << "breaknode: " << (*breakit)->num << " ficount:" << ficount << endl;
            
            for(pair<size_t, NODE*> fo : (*breakit)->VCG_FO_copy){ // erase n from it po
                fo.second->VCG_FI_copy.erase((*breakit)->num);
            }
            VCG.erase(breakit);
            
            // break;
        }
        else{            
            AddToTrack(nofinode);
            // AddToTrackAll(nofinode);
            for(int i = nofit.size()-1; i >= 0; i--){ 
                NODE *n = *nofit[i];
                n->route = true;
                if(LOG) cout << n->num << " ";
                for(pair<size_t, NODE*> fo : n->VCG_FO_copy){ // erase n from it po
                    fo.second->VCG_FI_copy.erase(n->num);
                }
                VCG.erase(nofit[i]);
            }
            if(LOG) cout << endl;
        }
    }


    if(LOG) cout << "hlayer" << endl;
    for(auto it = hlayer.begin(); it != hlayer.end(); it++){
        for(size_t h : *it){
            if(LOG) cout << setw(3) << h << " ";
        }
        if(LOG) cout << endl;
    }
    if(LOG) cout << "=====" << endl;

    if(LOG) cout << "hlayer size: " << hlayer.size() << endl;
}

void ChannelRoute::RouteVlayer(){
    while(hlayer.size() != vlayer.size()){        
        auto vtrack = --vlayer.end();
        vlayer.insert(vtrack, vector<size_t>(uch.size(), 0));
    }    
    

    for(size_t i = 0; i < uch.size(); i++){
        size_t pin = uch[i];
        if(pin == 0) continue;
        if(nodemap[pin]->route == false) continue;

        auto vit = ++vlayer.begin();
        auto hit = ++hlayer.begin();

        while(hit != hlayer.end()){
            (*vit)[i] = pin;
            if((*hit)[i] == pin){
                break;
            }
            else{
                vit++;
                hit++;
            }
        }
    }

    
    for(size_t i = 0; i < dch.size(); i++){
        size_t pin = dch[i];
        if(pin == 0) continue;
        if(nodemap[pin]->route == false) continue;

        auto vit = --(--vlayer.end());
        auto hit = --(--hlayer.end());

        while(hit != hlayer.begin()){
            (*vit)[i] = pin;
            if((*hit)[i] == pin){
                break;
            }
            else{
                vit--;
                hit--;
            }
        }
    }

    if(LOG) cout << "vlayer" << endl;
    for(auto it = vlayer.begin(); it != vlayer.end(); it++){
        for(size_t h : *it){
            if(LOG) cout << setw(3) << h << " ";
        }
        if(LOG) cout << endl;
    }
    if(LOG) cout << endl;
    if(LOG) cout << "vlayer size: " << vlayer.size() << endl;
}

void ChannelRoute::AddToTrackAll(vector<NODE*> nofinode){

    while(nofinode.size() != 0){
        auto htrack = --hlayer.end();
        hlayer.insert(htrack, vector<size_t>(uch.size(), 0));
        --htrack;

        NODE *n = nofinode.back();

        for(size_t i = n->locate.front().first; i <= n->locate.back().first; i++){
            (*htrack)[i] = n->num;
        }
        nofinode.pop_back();
    }
}

void ChannelRoute::AddToTrack(vector<NODE*> nofinode){
    auto htrack = --hlayer.end();
    hlayer.insert(htrack, vector<size_t>(uch.size(), 0));
    --htrack;

    while(nofinode.size() != 0){
        NODE *n = nofinode.back();        
        bool addtrack = false;
        vector<size_t> &track = *htrack;

        for(size_t i = n->locate.front().first; i <= n->locate.back().first; i++){
            if(track[i] != 0){
                addtrack = true;
                break;
            }
        }

        if(addtrack){
            hlayer.insert(htrack, vector<size_t>(uch.size(), 0));
            --htrack;
        }
        else{
            for(size_t i = n->locate.front().first; i <= n->locate.back().first; i++){
                (*htrack)[i] = n->num;
            }
            nofinode.pop_back();
        }
    }
}

void ChannelRoute::ConstructHCG(){
    struct HCGsort {
        bool operator() (pair<size_t, UPDN> i,pair<size_t, UPDN> j) { return (i.first<j.first);}
    } hcgsort;
    
    for(NODE *n : allnode){
        sort(n->locate.begin(), n->locate.end(), hcgsort);
        size_t min = n->locate.front().first;
        size_t max = n->locate.back().first;
        
        for(size_t i = min; i <= max; i++){
            size_t u = uch[i];
            size_t d = dch[i];
            if(u != 0 && u != n->num) n->HCG[u] = nodemap[u];
            if(d != 0 && d != n->num) n->HCG[d] = nodemap[d];
        }
    }

    // verify
    if(LOG) cout << "===== ConstructHCG start =====" << endl;
    for(NODE *n : allnode){
        if(LOG) cout << "num: " << n->num << endl;
        for(pair<size_t, NODE*> hcg : n->HCG){
            if(LOG) cout << hcg.first << " ";
        }
        if(LOG) cout << endl;

    }
    if(LOG) cout << "===== ConstructHCG end =====" << endl;
}

void ChannelRoute::ConstructVCG(){
    size_t pincount = uch.size();
    for(size_t i = 0; i < pincount; i++){
        size_t up = uch[i], dn = dch[i];

        if(up == 0 || dn == 0) continue;
        if(up == dn) continue;

        NODE *node_up = nodemap[up], *node_dn = nodemap[dn];
        node_up->VCG_FO[dn] = node_dn;
        node_dn->VCG_FI[up] = node_up;
    }

    // verify
    if(LOG) cout << "===== ConstructVCG start =====" << endl;
    for(NODE *n : allnode){
        if(LOG) cout << "pin: " << n->num << endl;
        if(LOG) cout << "VCG_FI: ";
        for(pair<size_t, NODE*> fi : n->VCG_FI){
            if(LOG) cout << fi.first << " ";
        }
        if(LOG) cout << endl;
        if(LOG) cout << "VCG_FO: ";
        for(pair<size_t, NODE*> fo : n->VCG_FO){
            if(LOG) cout << fo.first << " ";
        }
        if(LOG) cout << endl;
    }
    if(LOG) cout << "===== ConstructVCG end =====" << endl;
}

void ChannelRoute::Dump(string outputDir){
    ofstream fout(outputDir);

    vector<vector<size_t>> vmap, hmap;
   
    for(auto track : vlayer){
        vmap.emplace_back(track);
    }
    
    for(auto track : hlayer){
        hmap.emplace_back(track);
    }

    for(NODE *n : allnode){
        fout << ".begin " << n->num << endl;

        for(size_t i = 1; i < hmap.size() - 1; i++){
            int start = -1, end = -1;
            for(size_t j = 0; j < hmap[i].size(); j++){
                if(hmap[i][j] == n->num){
                    if(start == -1) start = j;
                    else end = j;
                }
            }
            if(start == -1 || end == -1) continue;
            fout << ".H " << start << " " << abs((int) i - (int) hmap.size()+1) << " " << end << endl;
        }

        for(size_t j = 0; j < vmap[0].size(); j++){
            int start = -1, end = -1;
            for(size_t i = 0; i < vmap.size(); i++){
                if(vmap[i][j] == n->num){
                    if(start == -1) start = i;
                    else end = i;
                }
                else{
                    if(start != -1 && end != -1){
                        fout << ".V " << j << " " << abs(end - (int) vmap.size()+1) << " " << abs(start - (int) vmap.size()+1) << endl;
                        start = -1;
                        end = -1;
                    }
                }
            }
            if(start == -1 || end == -1) continue;
            fout << ".V " << j << " " << abs(end - (int) vmap.size()+1) << " " << abs(start - (int) vmap.size()+1) << endl;

        }
        fout << ".end" << endl;

    }
    fout.close();
}

void ChannelRoute::Parse(string inputDir){
    ifstream fin(inputDir);

    string str;
    size_t upi = 0;
    while(getline(fin, str)){
        vector<string> tok = getToken(str, " \t");
        for(size_t i = 0; i < tok.size(); i++){
            size_t n = stoi(tok[i]);
            if(nodemap.find(n) == nodemap.end() && n != 0){
                NODE *node = new NODE;
                node->num = n;
                allnode.emplace_back(node);
                nodemap[n] = node;
            }
            if(upi == 0) {
                uch.emplace_back(n);
                if(n != 0)
                nodemap[n]->locate.emplace_back(i, UP);
            }
            else {
                dch.emplace_back(n);
                if(n != 0)
                nodemap[n]->locate.emplace_back(i, DN);
            }
        }
        upi++;        
    }

    fin.close();

    // verify
    if(LOG) cout << "===== Parser start =====" << endl;
    if(LOG) cout << "uch & dch" << endl;
    for(size_t &p : uch){
        if(LOG) cout << p << " ";
    }
    if(LOG) cout << endl;
    for(size_t &p : dch){
        if(LOG) cout << p << " ";
    }
    if(LOG) cout << endl;
    if(LOG) cout << "allnode" << endl;
    for(NODE *n : allnode){
        if(LOG) cout << n->num << ": ";
        for(pair<size_t, UPDN> & pin : n->locate){
            if(LOG) cout << pin.first << " ";
        }
        if(LOG) cout << endl;
    }
    if(LOG) cout << "===== Parser end =====" << endl;
}

vector<string> ChannelRoute::getToken(string str, string split){
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