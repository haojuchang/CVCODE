#pragma once

enum TILETYPE{SPACE = -1, BLOCK = 1, TRASH = 2};

struct POS{
    int x, y;
    POS(){}
    POS(int ix, int iy) : x(ix), y(iy){}
};

struct TILE{
    int index;
    TILETYPE type;
    POS llc; // lower-left corner
    int width;
    int height;
    TILE *bl = NULL, *lb = NULL, *rt = NULL, *tr = NULL;


    TILE(){}
    TILE(int index, TILETYPE type, POS llc, int width, int height)
        : index(index), type(type), llc(llc), width(width), height(height) {}
    void setNeighbor(TILE *ibl, TILE *ilb, TILE *irt, TILE *itr){
        bl = ibl; lb = ilb; rt = irt; tr = itr;
    }
    // TILE(int index, TILETYPE type, POS llc, int width, int height, TILE *bl, TILE *lb, TILE *rt, TILE *tr)
    //     : index(index), type(type), llc(llc), width(width), height(height), bl(bl), lb(lb), rt(rt), tr(tr) {}
};
