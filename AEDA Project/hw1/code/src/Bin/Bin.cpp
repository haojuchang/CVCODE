#include "Bin.hpp"

Bin::Bin(int BinW, int BinH, int outlineWidth, int outlineHeight)
    : BinW(BinW), BinH(BinH), outlineWidth(outlineWidth), outlineHeight(outlineHeight)
{
    int iblockcnt = outlineHeight / BinH;
    iblockcnt++;
    int jblockcnt = outlineWidth / BinW;
    jblockcnt++;

    Block.resize(iblockcnt);
    for(vector<vector<TILE*>> &b : Block)
        b.resize(jblockcnt);

    // cout << Block.size() << " " << Block[0].size() << endl;
}

void Bin::AddTile2Bin(TILE* tile){
    int x = tile->llc.x, y = tile->llc.y, w = tile->width, h = tile->height;
    int icnt = (h == 0) ? 0 : h / BinH;
    icnt++;
    icnt++;

    int jcnt = (w == 0) ? 0 : w / BinW;
    jcnt++;
    jcnt++;

    int istart = y / BinH;
    if(istart > 0) istart--;
    int jstart = x / BinW;
    if(jstart > 0) jstart--;

    for(int i = istart; i < istart + icnt; i++){
        for(int j = jstart; j < jstart + jcnt; j++){
            vector<TILE*> &b = Block[i][j];

            for(TILE *t : b){
                if(overlap(t, tile)){
                    t->connect.emplace(tile);
                    tile->connect.emplace(t);
                }
            }
            
            b.emplace_back(tile);
        }
    }
}

bool Bin::overlap(TILE *t1, TILE *t2){

    if(t1->llc.x + t1->width < t2->llc.x || t2->llc.x + t2->width < t1->llc.x)
        return false;

    if(t1->llc.y + t1->height < t2->llc.y || t2->llc.y + t2->height < t1->llc.y)
        return false;

    // t1: rec, t2: rec
    // t1: point, t2: point
    // t1: line, t2: line

    // t1: point, t2: rec
    // t1: rec, t2: point

    // t1: line, t2: rec
    // t1: rec, t2: line
    
    // t1: line, t2: point
    // t1: point, t2: line

    return true;
}