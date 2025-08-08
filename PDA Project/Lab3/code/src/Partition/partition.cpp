#include "partition.hpp"

Partition::Partition(vector<NODE*> Cells, size_t Chip_Height, size_t Row_Height)
    : Cells(Cells), Chip_Height(Chip_Height), Row_Height(Row_Height)
{
    srand (time(NULL));
    // size_t half = Cells.size() / 2;
    // for(size_t i = 0; i < Cells.size(); i++){
    //     if(i < half) Cells.at(i)->locate = _TOP;
    //     else Cells.at(i)->locate = _BOT;
    // }

    ConstructOverlapGraph();
    RandomDecideLocate();
    FindGoodLocate();
    // PrintGraph();
    // // string command = "./gpmetis " + graphName + " 2 -objtype=cut";
    // string command = "./gpmetis " + graphName + " " + to_string(Cells.size() / 2) + " -objtype=vol";
    // system(command.c_str());
    // ReadPartition();
}

int Partition::FindOverlapArea(){
    int TotalOverlapArea = 0;

    for(size_t c = 0; c < Cells.size(); c++){
        NODE *n = Cells[c];
        for(size_t i = 0; i < graph[c].size(); i++)
        {
            NODE *nei = Cells[graph[c][i].first];
            int overlap = graph[c][i].second;
            
            if(nei->locate == n->locate){
                TotalOverlapArea += overlap;
            }
        }
    }
    return TotalOverlapArea/2;
}

void Partition::FindGoodLocate(){ 
    int OverlapArea = FindOverlapArea();
    cout << "Init area: " << OverlapArea << endl;

    random_device rd;
    mt19937 gen = mt19937(rd());
    uniform_real_distribution<> dis(0, 1); //平均分佈
    auto randfun = bind(dis, gen);
    
    
    double T = 100, dt = 0.999;
    size_t movecelln = 10;
    size_t times = 0;
    while(T > 1){
        int gain = 0;

        set<size_t> moved;

        for(size_t mv = 0; mv < movecelln; mv++){
            size_t r = rand() % Cells.size();
            moved.insert(r);
        }

        for (size_t r : moved){
            // size_t r = rand() % Cells.size();
            NODE *n = Cells[r];

            for(size_t i = 0; i < graph[r].size(); i++){
                NODE *nei = Cells[graph[r][i].first];
                int overlap = graph[r][i].second;
                if(nei->locate == n->locate){
                    gain += overlap;
                }
                else{
                    gain -= overlap;
                }
            }
            n->locate == _TOP ? n->locate = _BOT : n->locate = _TOP;
        }

        double ran = randfun();
        double ran2 = randfun() * T;
        double proba=exp(gain/1000/T);
        if(proba>1)proba=1;
        // cout << gain << " " << ran << " ";
        if(gain > 0 || ran < proba){
            // cout << gain << " ";
            OverlapArea -= gain;
            times++;
        }
        else{
            for (size_t r : moved){
                NODE *n = Cells[r];
                
                n->locate == _TOP ? n->locate = _BOT : n->locate = _TOP;
            }
        }

        if(times % 100 == 0){
            T *= dt;
        }
    }

    times = 10000;
    while(times != 0){
        size_t ran = rand() % Cells.size();
        NODE *n = Cells[ran];
        int gain = 0;

        for(size_t i = 0; i < graph[ran].size(); i++){
            NODE *nei = Cells[graph[ran][i].first];
            int overlap = graph[ran][i].second;
            if(nei->locate == n->locate){
                gain += overlap;
            }
            else{
                gain -= overlap;
            }
        }
        if(gain > 0){
            n->locate == _TOP ? n->locate = _BOT : n->locate = _TOP;
            OverlapArea -= gain;
        }

        times--;
    }
    cout << "Final area: " << OverlapArea << endl;
}

void Partition::RandomDecideLocate(){
    size_t ran;
    for(size_t i = 0; i < Cells.size(); i++){
        ran = rand() % 2;
        if(ran == 0){
            Cells[i]->locate = _TOP;
        }
        else{
            Cells[i]->locate = _BOT;
        }
    }
}

void Partition::ReadPartition(){
    cout <<graphName + ".part." + to_string(Cells.size() / 2) << endl;
    ifstream fin(graphName + ".part." + to_string(Cells.size() / 2));

    unordered_map<size_t, bool> check;

    size_t chip;
    for(size_t i = 0; i < Cells.size(); i++){
        fin >> chip;
        auto it = check.find(chip);
        if(it == check.end()){
            check[chip] = true;
             Cells[i]->locate = _TOP;
        }
        else
            Cells[i]->locate = _BOT;
        // if(chip == 0)
        //     Cells[i]->locate = _TOP;
        // else
        //     Cells[i]->locate = _BOT;
    }

    fin.close();
}
// void Partition::FM(vector<NODE*> &nodes)
// {
//     InitTOPBOT(nodes);

//     unordered_map<NODE*, int, MyHashFunction> gainmap;
//     NODE* maxgain = InitGainMap(gainmap, nodes);

// }

// NODE* Partition::InitGainMap(unordered_map<NODE*, int, MyHashFunction> &gainmap, vector<NODE*> &nodes)
// {
//     NODE* maxgain;
//     for(NODE *n : nodes){
//         int gain = 0;

//         for(auto it = graph[n].begin(); it != graph[n].end(); it++){
//             NODE *neighbor = (*it).first;
//             size_t overlap = (*it).second;

//             if(neighbor->locate = n->locate) gain -= (int) overlap; // n and neighbor at same side
//             else gain += (int) overlap;
//         }
//     }
// }

// void Partition::InitTOPBOT(vector<NODE*> &nodes)
// {
//     random_shuffle(nodes.begin(), nodes.end());
//     size_t half = nodes.size() / 2;

//     for(size_t i = 0; i < nodes.size(); i++){
//         if(i < half) nodes.at(i)->locate = _TOP;
//         else nodes.at(i)->locate = _BOT;
//     }
// }

void Partition::ConstructOverlapGraph()
{
    size_t maxRow = Chip_Height / Row_Height;
    graph.resize(Cells.size());

    for(vector<pair<size_t, size_t>> &row : graph){
        row.reserve(Cells.size()/maxRow);
    }

    

    for(size_t i = 0; i < Cells.size(); i++)
    {
        NODE *a = Cells.at(i);
        for(size_t j = i + 1; j < Cells.size(); j++)
        {
            NODE *b = Cells.at(j);
            if(a == b) continue;

            if(TwoCellOverlap(a, b))
            {
                edge_cnt++;
                size_t x_dis = min(a->x + a->w, b->x + b->w) - max(a->x, b->x);
                size_t y_dis = min(a->y + a->h, b->y + b->h) - max(a->y, b->y);
                size_t overlap = x_dis * y_dis;
                graph[i].emplace_back(j, overlap);
                graph[j].emplace_back(i, overlap);
                if(overlap > maxOverlap) maxOverlap = overlap;
            }
        }
    }
}

void Partition::PrintGraph(){
    ofstream fout(graphName);
    fout << Cells.size() << " " << edge_cnt << " 001" << endl;

    for(size_t i = 0; i < graph.size(); i++){
        for(size_t j = 0; j < graph[i].size(); j++){
            // fout << graph[i][j].first + 1 << " " << graph[i][j].second << " "; // because metis node id is from 1
            fout << graph[i][j].first + 1 << " " << maxOverlap + 1 - graph[i][j].second << " "; // because metis node id is from 1
        }
        fout << endl;
    }

    fout.close();
}


bool Partition::TwoCellOverlap(NODE *a, NODE *b){
    if(!(   (a->x         >= b->x + b->w)       || // a at b right
            (a->x + a->w  <= b->x       )       || // a at b left
            (a->y         >= b->y + b->h)       || // a at b top
            (a->y + a->h  <= b->y       )          // a at b bottom
        )
    ) return true;

    return false;
}