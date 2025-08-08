#include "channel.hpp"


ChannelRoute::ChannelRoute(){

}

void ChannelRoute::Run(){
    ConstructVCG();
    ConstructHCG();
    CheckCycle();
    RouteNoneCycle();
    RouteVlayer();
}

void ChannelRoute::CheckCycle(){
    cout << "===== CheckCycle start =====" << endl;
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
                cout << n->num << endl;
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
            cout << "breaknode: " << (*breakit)->num << " ficount:" << ficount << endl;
            
            for(pair<size_t, NODE*> fo : (*breakit)->VCG_FO_copy){ // erase n from it po
                fo.second->VCG_FI_copy.erase((*breakit)->num);
            }
            VCG.erase(breakit);
            
            // break;
        }
    }
    cout << "===== CheckCycle end =====" << endl;
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
        cout << endl;

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
            cout << "breaknode: " << (*breakit)->num << " ficount:" << ficount << endl;
            
            for(pair<size_t, NODE*> fo : (*breakit)->VCG_FO_copy){ // erase n from it po
                fo.second->VCG_FI_copy.erase((*breakit)->num);
            }
            VCG.erase(breakit);
            
            // break;
        }
        else{            
            AddToTrack(nofinode);
            for(int i = nofit.size()-1; i >= 0; i--){ 
                NODE *n = *nofit[i];
                n->route = true;
                cout << n->num << " ";
                for(pair<size_t, NODE*> fo : n->VCG_FO_copy){ // erase n from it po
                    fo.second->VCG_FI_copy.erase(n->num);
                }
                VCG.erase(nofit[i]);
            }
            cout << endl;
        }
    }


    cout << "hlayer" << endl;
    for(auto it = hlayer.begin(); it != hlayer.end(); it++){
        for(size_t h : *it){
            cout << setw(3) << h << " ";
        }
        cout << endl;
    }
    cout << "=====" << endl;

    cout << "hlayer size: " << hlayer.size() << endl;
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

    cout << "vlayer" << endl;
    for(auto it = vlayer.begin(); it != vlayer.end(); it++){
        for(size_t h : *it){
            cout << setw(3) << h << " ";
        }
        cout << endl;
    }
    cout << endl;
    cout << "vlayer size: " << vlayer.size() << endl;
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

    // bool addtrack = true;
    // for(auto it = htrack; it != hlayer.end(); it++){
    //     vector<size_t> &track = (*it);
    //     bool placethistrack = true;
    //     for(size_t i = n->locate.front().first; i <= n->locate.back().first; i++){
    //         if(track[i] != 0){
    //             placethistrack = false;
    //             break;
    //         }
    //     }
    //     if(placethistrack){
    //         addtrack = false;
    //         htrack = it;
    //         break;
    //     }
    // }

    // if(addtrack){
    //     htrack = --hlayer.end();
    //     hlayer.insert(htrack, vector<size_t>(uch.size(), 0));
    //     --htrack;
    // }

    // for(size_t i = n->locate.front().first; i <= n->locate.back().first; i++){
    //     (*htrack)[i] = n->num;
    // }

    // auto vtrack = --vlayer.end();
    // vlayer.insert(vtrack, vector<size_t>(uch.size(), 0));
    // --vtrack;
    // auto prevtrack = vtrack;
    // --prevtrack;
    // for(size_t i = 0; i < dch.size(); i++){
    //     (*vtrack)[i] = (*prevtrack)[i];
    // }


    // for(pair<size_t, UPDN> pin : n->locate){
    //     size_t pos = pin.first;
    //     UPDN updn = pin.second;
    //     if(updn == UP){
    //         for(auto it = vlayer.begin(); it != vtrack;){
    //             ++it;
    //             (*it)[pos] = n->num;
    //         }
    //     }
    //     else if(updn == DN){
    //         for(auto it = vtrack; it != vlayer.end(); ++it){
    //             (*it)[pos] = n->num;
    //         }
    //     }
    // }
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
    cout << "===== ConstructHCG start =====" << endl;
    for(NODE *n : allnode){
        cout << "num: " << n->num << endl;
        for(pair<size_t, NODE*> hcg : n->HCG){
            cout << hcg.first << " ";
        }
        cout << endl;

    }
    cout << "===== ConstructHCG end =====" << endl;
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
    cout << "===== ConstructVCG start =====" << endl;
    for(NODE *n : allnode){
        cout << "pin: " << n->num << endl;
        cout << "VCG_FI: ";
        for(pair<size_t, NODE*> fi : n->VCG_FI){
            cout << fi.first << " ";
        }
        cout << endl;
        cout << "VCG_FO: ";
        for(pair<size_t, NODE*> fo : n->VCG_FO){
            cout << fo.first << " ";
        }
        cout << endl;
    }
    cout << "===== ConstructVCG end =====" << endl;
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

        for(int i = 1; i < hmap.size() - 1; i++){
            int start = -1, end = -1;
            for(size_t j = 0; j < hmap[i].size(); j++){
                if(hmap[i][j] == n->num){
                    if(start == -1) start = j;
                    else end = j;
                }
            }
            if(start == -1 || end == -1) continue;
            fout << ".H " << start << " " << abs(i - (int) hmap.size()+1) << " " << end << endl;
        }

        for(int j = 0; j < vmap[0].size(); j++){
            int start = -1, end = -1;
            for(size_t i = 0; i < vmap.size(); i++){
                if(vmap[i][j] == n->num){
                    if(start == -1) start = i;
                    else end = i;
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
        vector<string> tok = getToken(str, " ");
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
    cout << "===== Parser start =====" << endl;
    cout << "uch & dch" << endl;
    for(size_t &p : uch){
        cout << p << " ";
    }
    cout << endl;
    for(size_t &p : dch){
        cout << p << " ";
    }
    cout << endl;
    cout << "allnode" << endl;
    for(NODE *n : allnode){
        cout << n->num << ": ";
        for(pair<size_t, UPDN> & pin : n->locate){
            cout << pin.first << " ";
        }
        cout << endl;
    }
    cout << "===== Parser end =====" << endl;
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