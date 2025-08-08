#include "legalize.hpp"

Legalize::Legalize(vector<NODE*> Cells, vector<NODE*> Terminals, size_t Chip_Width, size_t Chip_Height, size_t Row_Height, size_t Row_Number)
    : Cells(Cells), Terminals(Terminals), Chip_Width(Chip_Width), Chip_Height(Chip_Height), Row_Height(Row_Height), Row_Number(Row_Number)
{
    if(Cells.size() == 0) return;
    InitRow();
    cout << "initrow down" << endl;
    Abacus();
    cout << "abacus down" << endl;
    // cout << "rows num. : " << rows.size() << endl;
    // print_Cells();
    // for(ROW *r : rows){
    //     cout << "c " << r->xmin << " " << r->xmax << " " << r->y << endl;
    // }

    // for(ROW *row : rows){
    //     // cout << row->xmax - row->xmin << endl;
    //     cout << row->xmax - row->xmin - row->total_cell_width << endl;
    // }
    // print_Cells();
}

// void Legalize::pp(size_t r, NODE *cell){
//     ROW *row = rows[r];
// 
//     if(row->total_cell_width + cell->w > row->xmax - row->xmin) {
//         costs[r] = UINT_MAX;
//         return;
//     }
// 
//     row->RCells.emplace_back(cell);
// 
//     PlaceRow(row);
//     costs[r] = Cost(row->RCells);
//     
//     row->RCells.pop_back();
// }

// ===== Abacus BEG =====

void Legalize::FindNearestRow(){

    for(NODE *n : Cells){
        size_t cost = UINT_MAX;
        size_t near = UINT_MAX;
        for(size_t i = 0; i < rows.size(); i++){
            size_t cur = abs((int) n->y - (int) rows[i]->y);
            if(cur < cost) {
                cost = cur;
                near = i;
            }
        }
        n->nearestRow = near;
    }
}

void Legalize::Abacus(){
    cout << "Chip: " << Cells.front()->locate << endl;

    struct CellSort {
        bool operator() (NODE *i, NODE *j) { return (i->x < j->x);}
    } CellSortObj;
    vector<NODE*> SortedCells = Cells;
    sort(SortedCells.begin(), SortedCells.end(), CellSortObj);

    FindNearestRow();
    cout << "FindNearestRow down" << endl;
    
    for(NODE *cell : SortedCells){
        size_t cbest = UINT_MAX;
        size_t rbest = UINT_MAX;

        // vector<thread> threads;
        // costs.resize(rows.size());

        // for(size_t r = 0; r < rows.size(); r++){
        //     thread th(&Legalize::pp, this, r, cell);
        //     threads.push_back(th);
        // }
        // for(size_t i = 0; i < threads.size(); i++){
        //     threads[i].join();
        // }
// 
        // for(size_t i = 0; i < costs.size(); i++){
        //     if(costs[i] < cbest){
        //         cbest = costs[i];
        //         rbest = i;
        //     }
        // }

        int near = cell->nearestRow;
        int up, dw;

        for(int offset = 0; ; offset++){
            if(offset > 10 && rbest != UINT_MAX) break;
            up = near + offset;
            dw = near - offset;

            if(up >= rows.size() && dw < 0) break;
            
            if(!(up >= rows.size())){
                ROW *row = rows[up];
                if(row->total_cell_width + cell->w > row->xmax - row->xmin) continue;

                row->RCells.emplace_back(cell);

                PlaceRow(row);
                size_t c = Cost(row->RCells);
            
                if(c < cbest){
                    cbest = c;
                    rbest = up;
                }

                row->RCells.pop_back();
            }

            if(offset == 0) continue;

            if(!(dw < 0)){
                ROW *row = rows[dw];
                if(row->total_cell_width + cell->w > row->xmax - row->xmin) continue;

                row->RCells.emplace_back(cell);

                PlaceRow(row);
                size_t c = Cost(row->RCells);
            
                if(c < cbest){
                    cbest = c;
                    rbest = dw;
                }

                row->RCells.pop_back();
            }
        }

        // for(size_t r = 0; r < rows.size(); r++){
        //     ROW *row = rows[r];
        //     if(row->total_cell_width + cell->w > row->xmax - row->xmin) continue;

        //     row->RCells.emplace_back(cell);

        //     PlaceRow(row);
        //     size_t c = Cost(row->RCells);
         
        //     if(c < cbest){
        //         cbest = c;
        //         rbest = r;
        //     }

        //     row->RCells.pop_back();
        // }

        if(rbest == UINT_MAX){
            failed_cell.emplace_back(cell);
            continue;
        }

        ROW *rowbest = rows[rbest];
        rowbest->RCells.emplace_back(cell);
        rowbest->total_cell_width += cell->w;
        PlaceRow(rowbest);
    }
    cout << "failed_cell: " << failed_cell.size() << endl;
    // for(ROW *row : rows){
    //     // if(row->RCells.size() == 0) continue;
    //     PlaceRow(row);
    // }
}

void Legalize::PlaceRow(ROW *row)
{
    row->clusters.clear();
    list<CLUSTER>::reverse_iterator rit;
    
    for(size_t i = 0; i < row->RCells.size(); i++){
        NODE *cell = row->RCells[i];
        size_t x_ = cell->x; // cell's global position
        rit = row->clusters.rbegin();
        CLUSTER &c = *rit;
        
        if(i == 0 || c.xc + c.wc < x_){
            CLUSTER nc;
            nc.ec = 0; nc.wc = 0; nc.qc = 0;
            nc.xc = x_;
            nc.nfirst = i;
            AddCell(nc, row, i);            

            row->clusters.emplace_back(nc);
            Collapse(row);
        }
        else{
            AddCell(c, row, i);
            Collapse(row);
        }
    }

    for(CLUSTER c : row->clusters){
        size_t x = c.xc;
        // if(row->RCells.size() == 0) continue;
        size_t i = c.nfirst;
        while(i <= c.nlast){
            NODE *cell = row->RCells[i];
            cell->Lx = x;
            cell->Ly = row->y;
            x += cell->w;
            ++i;
        }
    }
}
void Legalize::AddCell(CLUSTER &c, ROW *row, size_t i){
    NODE *cell = row->RCells[i];
    size_t ei = cell->w * cell->h;
    // size_t ei = 1;
    size_t x_ = cell->x;
    size_t wi = cell->w;

    c.nlast = i;
    c.ec += ei;
    c.qc += ei * (x_ - c.wc);
    c.wc += wi;
    
    // if(c.xc < row->xmin) c.xc = row->xmin;
    // if(c.xc > row->xmax - c.wc) c.xc = row->xmax - c.wc;
}
void Legalize::AddCluster(CLUSTER &c, CLUSTER &c_){
    c.nlast = c_.nlast;
    c.ec += c_.ec;
    c.qc += (c_.qc - c_.ec*c.wc);
    c.wc += c_.wc;
}
void Legalize::Collapse(ROW *row){
    list<CLUSTER>::reverse_iterator rit = row->clusters.rbegin();
    CLUSTER &c = *rit;

    c.xc = c.qc / c.ec;
    if(c.xc < row->xmin) c.xc = row->xmin;
    if(c.xc > row->xmax - c.wc) c.xc = row->xmax - c.wc;
    ++rit;
    CLUSTER &c_ = *rit;
    if(rit != row->clusters.rend() && c_.xc + c_.wc >= c.xc){
        AddCluster(c_, c);
        row->clusters.pop_back();
        Collapse(row);
    }
}

// ===== Abacus END =====

void Legalize::InitRow(){
    list<ROW*> CandidateRow;
    for(size_t i = 0; i < Row_Number; i++){
        ROW *r = new ROW;
        r->y = i * Row_Height;
        r->h = Row_Height;
        r->xmin = 0;
        r->xmax = Chip_Width;
        CandidateRow.emplace_back(r);
    }

    // cout << "init row count: " << CandidateRow.size() << endl;

    for(NODE *term : Terminals){
        vector<list<ROW*>::iterator> split;
        bool overlap = true;
        while(overlap){
            overlap = false;
            for(auto it = CandidateRow.begin(); it != CandidateRow.end(); it++){
                ROW *row = *it;
                if(!(   (term->x            >= row->xmax)       || // term at row right
                        (term->x + term->w  <= row->xmin)       || // term at row left
                        (term->y            >= row->y + row->h) || // term at row top
                        (term->y + term->h  <= row->y)             // term at row bottom
                    )
                ){
                    overlap = true;
                    if(term->x <= row->xmin && term->x + term->w >= row->xmax){ // terminal cover row
                        CandidateRow.erase(it);
                    }
                    else if(term->x <= row->xmin){ // terminal at row left
                        row->xmin = term->x + term->w;
                    } 
                    else if(term->x + term->w >= row->xmax){ // terminal at row right
                        row->xmax = term->x;
                    }
                    else{ // terminal at row middle
                        ROW *nrow = new ROW; // at terminal right
                        nrow->y = row->y;
                        nrow->h = row->h;
                        nrow->xmax = row->xmax;
                        nrow->xmin = term->x + term->w;
                        CandidateRow.emplace_back(nrow);

                        row->xmax = term->x; // at terminal left

                        if(nrow->xmax < nrow->xmin){
                            cout << "fatal error!!" << endl;
                        }
                    }

                    break;
                }
            }
        }
    }

    size_t initRCellsize = Cells.size() / CandidateRow.size();
    for(ROW *row : CandidateRow){
        row->RCells.reserve(initRCellsize);
        rows.emplace_back(row);
    }

    struct RowSort {
        bool operator() (ROW *i, ROW *j) { return (i->y < j->y);}
    } RowSortObj;
    sort(rows.begin(), rows.end(), RowSortObj);

    // cout << "final row count: " << rows.size() << endl;
}

size_t Legalize::Cost(vector<NODE*> &Cells){
    size_t offset = 0;
    for(NODE *cell : Cells){
        offset += (size_t)abs((int)cell->Lx - (int)cell->x);
        offset += (size_t)abs((int)cell->Ly - (int)cell->y);
    }
    return offset;
}

void Legalize::print_Cells(){
    cout << "\n===== print_Cells =====" << endl;
    for(NODE *n : Cells){
        cout << n->name << " type:" << n->type << " locate:" << n->locate << " x:" << n->x << " y:" << n->y << " w:" << n->w << " h:" << n->h << " Lx:" << n->Lx << " Ly:" << n->Ly << endl;
    }
}
void Legalize::print_Terminals(){
    cout << "\n===== print_Terminals =====" << endl;
    for(NODE *n : Terminals){
        cout << n->name << " type:" << n->type << " locate:" << n->locate << " x:" << n->x << " y:" << n->y << " w:" << n->w << " h:" << n->h << endl;
    }
}