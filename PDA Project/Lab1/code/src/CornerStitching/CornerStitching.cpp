#include "CornerStitching.hpp"

CornerStiching::CornerStiching(int outlineWidth, int outlineHeight)
    : outlineWidth(outlineWidth), outlineHeight(outlineHeight)
{
    TILE *b = new TILE(0, SPACE, POS(0, 0), outlineWidth, outlineHeight);
    tiles.emplace_back(b);
    LRtile = b;
}

void CornerStiching::DumpLayout(string LayoutDir){
    ofstream fout(LayoutDir);

    fout << tiles.size() - trash.size() << endl;
    fout << outlineWidth << " " << outlineHeight << endl;
    for(TILE* t: tiles){
        if(t->type == TRASH) continue;
        fout << t->index * t->type << " ";
        fout << t->llc.x << " ";
        fout << t->llc.y << " ";
        fout << t->width << " ";
        fout << t->height << " ";
        
        if(t->bl == NULL) fout << 0 << " ";
        else fout << t->bl->index * t->bl->type << " ";

        if(t->lb == NULL) fout << 0 << " ";
        else fout << t->lb->index * t->lb->type << " ";

        if(t->rt == NULL) fout << 0 << " ";
        else fout << t->rt->index * t->rt->type << " ";

        if(t->tr == NULL) fout << 0 << endl;
        else fout << t->tr->index * t->tr->type << endl;        
    }

    fout.close();
}

bool _sort_block(TILE *a, TILE *b){
    return a->index < b->index;
}

vector<TILE*> CornerStiching::_neighbor_finding(TILE *t){
    vector<TILE*> nei;
    vector<TILE*> left = _neighbor_finding_left(t);
    vector<TILE*> right = _neighbor_finding_right(t);
    vector<TILE*> top = _neighbor_finding_top(t);
    vector<TILE*> down = _neighbor_finding_down(t);

    nei.insert(nei.end(), left.begin(), left.end());
    nei.insert(nei.end(), right.begin(), right.end());
    nei.insert(nei.end(), top.begin(), top.end());
    nei.insert(nei.end(), down.begin(), down.end());

    return nei;
}

void CornerStiching::DumpBlock(string OutputDir){
    ofstream fout(OutputDir);

    sort(blocks.begin(), blocks.end(), _sort_block);

    fout << tiles.size() - trash.size() << endl;

    for(TILE *b :blocks){
        vector<TILE*> nei = _neighbor_finding(b);
        int Bcnt = 0, Scnt = 0;
        for(TILE* n : nei){
            if(n->type == BLOCK) Bcnt++;
            if(n->type == SPACE) Scnt++;
        }
        fout << b->index << " " << Bcnt << " " << Scnt << endl;
    }

    fout.close();
}

POS CornerStiching::PointFinding(POS *point){
    TILE *find = _point_finding(point);
    return find->llc;
}

TILE* CornerStiching::_point_finding(POS *point){
    if(point->x >= outlineWidth || point->y >= outlineHeight){
        cout << "point finding error: 2. point overflow outline" << endl;
        exit(1);
    }

    TILE *cur = LRtile;
    bool found = false;
    bool goUP = true;

    while(!found && cur != NULL){
        if(goUP){
            if(point->y >= cur->llc.y + cur->height)
                cur = cur->rt;
            else if(cur->llc.x <= point->x && point->x < cur->llc.y + cur->height)
                found = true;
            else
                goUP = false;
        }
        else{ // go left
            if(point->x < cur->llc.x)
                cur = cur->bl;
            else if(cur->llc.y <= point->y && point->y < cur->llc.y + cur->height)
                found = true;
            else
                goUP = true;
        }
    }
    
    if(cur == NULL){
        cout << "point finding error: 1.tile not exist" << endl;
        cout << "point: " << point->x << " " << point->y << endl;
        cout << "LR: " << LRtile->llc.x << " " << LRtile->llc.y << endl;
        exit(1);
    }

    return cur;
}

void CornerStiching::TileCreating(TILE* tile){

    _horizontal_tile_split(tile->llc);
    _horizontal_tile_split(POS(tile->llc.x, tile->llc.y + tile->height));


    int height = 0;
    vector<TILE*> center; // push back from lower to height
    while(height != tile->height){
        _vertical_tile_split(POS(tile->llc.x, tile->llc.y + height));
        _vertical_tile_split(POS(tile->llc.x + tile->width, tile->llc.y + height));
        
        POS cur = POS(tile->llc.x, tile->llc.y + height);
        TILE *find = _point_finding(&cur);
        center.emplace_back(find);
        height += find->height;
    }

    TILE *target = _merge_center(center, tile);

    vector<TILE*> left;
    vector<TILE*> leftnei = _neighbor_finding_left(target);
    if(leftnei.size() != 0){
        left.emplace_back(leftnei.front()->lb);
        left.insert(left.end(), leftnei.begin(), leftnei.end());
        left.emplace_back(left.back()->rt);
    }
    for(size_t i = 1; i < left.size(); i++){
        TILE *low = left.at(i-1);
        TILE *height = left.at(i);
        bool merge = _merge_neighbor(low, height);
        if(merge) 
            left.at(i) = left.at(i-1);
    }
    
    vector<TILE*> right;
    vector<TILE*> rightnei = _neighbor_finding_right(target);
    if(rightnei.size()!=0){
        right.emplace_back(rightnei.front()->rt);
        right.insert(right.end(), rightnei.begin(), rightnei.end());
        right.emplace_back(right.back()->lb);
    }
    for(size_t i = right.size(); i >= 2; i--){
        TILE *low = right.at(i-1);
        TILE *height = right.at(i-2);
        bool merge = _merge_neighbor(low, height);
        if(merge){
            right.at(i-2) = right.at(i-1);
        }
    }
}



TILE* CornerStiching::_merge_center(vector<TILE*> &center, TILE *tile){
    // merge center
    vector<TILE*> leftNeis, rightNeis, topNeis;
    topNeis = _neighbor_finding_top(center.back());
    for(TILE* t : center){
        vector<TILE*> left = _neighbor_finding_left(t);
        vector<TILE*> right = _neighbor_finding_right(t);
        leftNeis.insert(leftNeis.end(), left.begin(), left.end());
        rightNeis.insert(rightNeis.end(), right.begin(), right.end());
    }
    TILE* target = center.at(0);
    target->rt = center.back()->rt;
    target->tr = center.back()->tr;
    target->height = tile->height;
    target->type = BLOCK;
    target->index = tile->index;
    
    _update_pointer_left(target, leftNeis);
    _update_pointer_right(target, rightNeis);
    _update_pointer_top(target, topNeis);
    for(size_t i = 1; i < center.size(); i++){
        center.at(i)->type = TRASH;
        trash.emplace_back(center.at(i));
    }

    blocks.emplace_back(target);
    return target;
}

bool _sort_TILE(TILE *a, TILE *b){
    return a->llc.y < b->llc.y;
}

bool CornerStiching::_merge_neighbor(TILE *&low, TILE *&height){
    if(low == NULL || height == NULL) {
        return false;
    }

    if(low->type == BLOCK || height->type == BLOCK){
        return false;
    }

    if(low->llc.x != height->llc.x){
        return false;
    }

    if(low->width != height->width){
        return false;
    }

    // represent low & height 1. both space tile 2. width are equal
    vector<TILE*> left = _neighbor_finding_left(low);
    vector<TILE*> right = _neighbor_finding_right(low);
    vector<TILE*> top = _neighbor_finding_top(height);
    vector<TILE*> leftTmp = _neighbor_finding_left(height);
    vector<TILE*> rightTmp = _neighbor_finding_right(height);
    left.insert(left.end(), leftTmp.begin(), leftTmp.end());
    right.insert(right.end(), rightTmp.begin(), rightTmp.end());

    height->type = TRASH;
    trash.emplace_back(height);

    low->height += height->height;
    low->rt = height->rt;
    low->tr = height->tr;
    _update_pointer_left(low, left);
    _update_pointer_right(low, right);
    _update_pointer_top(low, top);

    return true;
}

void CornerStiching::_vertical_tile_split(POS p){
    if(p.x >= outlineWidth) return;

    TILE *t = _point_finding(&p);
    if(t->llc.x == p.x) return;
    
    vector<TILE*> top = _neighbor_finding_top(t);
    vector<TILE*> down = _neighbor_finding_down(t);
    vector<TILE*> right = _neighbor_finding_right(t);

    TILE *trt = NULL, *tlb = NULL, *nrt = NULL, *nlb = NULL;

    TILE *newT = new TILE(++tileCnt, SPACE, p, t->llc.x + t->width - p.x, t->height);
    t->width = p.x - t->llc.x;

    // update t and newT pointer
    for(TILE *T : top){
        if(T->llc.x < newT->llc.x + newT->width && newT->llc.x + newT->width <= T->llc.x + T->width)
            nrt = T;
        if(T->llc.x < t->llc.x + t->width && t->llc.x + t->width <= T->llc.x + T->width)
            trt = T;
    }

    for(TILE *D : down){
        if(D->llc.x <= newT->llc.x && newT->llc.x < D->llc.x + D->width)
            nlb = D;
        if(D->llc.x <= t->llc.x && t->llc.x < D->llc.x + D->width)
            tlb = D;
    }

    newT->setNeighbor(t, nlb, nrt, t->tr);
    t->setNeighbor(t->bl, tlb, trt, newT);
    
    _update_pointer_top(t, top);
    _update_pointer_top(newT, top);
    _update_pointer_down(t, down);
    _update_pointer_down(newT, down);
    _update_pointer_right(newT, right);

    tiles.emplace_back(newT);

    _update_LRtile(newT);
    _update_LRtile(t);
}

void CornerStiching::_update_LRtile(TILE *target){
    if(target->llc.y != 0) return;
    if(target->llc.x > LRtile->llc.x)
        LRtile = target;
}

void CornerStiching::_horizontal_tile_split(POS p){
    if(p.y >= outlineHeight) return;

    TILE *t = _point_finding(&p);
    if(t->llc.y == p.y) return;
    
    vector<TILE*> left = _neighbor_finding_left(t);
    vector<TILE*> right = _neighbor_finding_right(t);
    vector<TILE*> top = _neighbor_finding_top(t);
    TILE *nbl, *ttr;

    TILE *newT = new TILE(++tileCnt, SPACE, POS(t->llc.x, p.y), t->width, t->height+t->llc.y-p.y);
    
    // update new tile bl pointer(by left neighbor of t)
    if(left.size() == 0) nbl = NULL;
    else{
        for(TILE *L : left){
            if( L->llc.y <= newT->llc.y && newT->llc.y < L->llc.y + L->height){
                nbl = L;
                break;
            }
        }
    }
    newT->setNeighbor(nbl, t, t->rt, t->tr);

    // update t tile tr pointer(by right neighbor of t)
    t->height = p.y - t->llc.y;
    if(right.size() == 0) ttr = NULL;
    else{
        for(TILE *R : right){
            if( R->llc.y < t->llc.y + t->height && t->llc.y + t->height <= R->llc.y + R->height){
                ttr = R;
                break;
            }
        }
    }
    t->setNeighbor(t->bl, t->lb, newT, ttr);
    
    _update_pointer_top(newT, top);
    _update_pointer_left(newT, left);
    _update_pointer_left(t, left);


    // update right neighbor pointer
    _update_pointer_right(newT, right);
    _update_pointer_right(t, right);
    
    tiles.emplace_back(newT);
    _update_LRtile(newT);
    _update_LRtile(t);
}

void CornerStiching::_update_pointer_top(TILE *target, vector<TILE*> &nei){    
    for(TILE *t : nei)
        if(target->llc.x <= t->llc.x && t->llc.x < target->llc.x + target->width){
            if(t->type != TRASH)
                t->lb = target;
            else{
                cout << "TRASH error" << endl;
                exit(1);
            }
        }
}

void CornerStiching::_update_pointer_down(TILE *target, vector<TILE*> &nei){
    for(TILE *t : nei)
        if(target->llc.x < t->llc.x + t->width && t->llc.x + t->width <= target->llc.x + target->width){
            if(t->type != TRASH)
                t->rt = target;
            else{
                cout << "TRASH error" << endl;
                exit(1);
            }
        }
}
void CornerStiching::_update_pointer_left(TILE *target, vector<TILE*> &nei){
    for(TILE *t : nei)
        if(target->llc.y < t->llc.y + t->height && t->llc.y + t->height <= target->llc.y + target->height){
            if(t->type != TRASH)
                t->tr = target;
            else{
                cout << "TRASH error" << endl;
                exit(1);
            }
        }

}
void CornerStiching::_update_pointer_right(TILE *target, vector<TILE*> &nei){
    for(TILE *t : nei)
        if(target->llc.y <= t->llc.y && t->llc.y < target->llc.y + target->height){
            if(t->type != TRASH)
                t->bl = target;
            else{
                cout << "TRASH error" << endl;
                exit(1);
            }
        }
}

vector<TILE*> CornerStiching::_neighbor_finding_left(TILE* t){
    vector<TILE*> neighbor;
    TILE* cur = t->bl;
    while(cur != NULL){
        neighbor.emplace_back(cur);
        if(cur->llc.y + cur->height < t->llc.y + t->height)
            cur = cur->rt;
        else
            cur = NULL;
    }
    return neighbor;
}

vector<TILE*> CornerStiching::_neighbor_finding_right(TILE* t){
    vector<TILE*> neighbor;
    TILE* cur = t->tr;
    while(cur != NULL){
        neighbor.emplace_back(cur);
        if(cur->llc.y > t->llc.y)
            cur = cur->lb;
        else
            cur = NULL;
    }
    return neighbor;
}

vector<TILE*> CornerStiching::_neighbor_finding_top(TILE* t){
    vector<TILE*> neighbor;
    TILE* cur = t->rt;
    while(cur != NULL){
        neighbor.emplace_back(cur);
        if(cur->llc.x > t->llc.x)
            cur = cur->bl;
        else
            cur = NULL;
    }
    return neighbor;
}

vector<TILE*> CornerStiching::_neighbor_finding_down(TILE* t){
    vector<TILE*> neighbor;
    TILE* cur = t->lb;
    while(cur != NULL){
        neighbor.emplace_back(cur);
        if(cur->llc.x + cur->width < t->llc.x + t->width)
            cur = cur->tr;
        else
            cur = NULL;
    }
    return neighbor;
}
