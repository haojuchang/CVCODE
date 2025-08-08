#include "Parser/parser.hpp"
#include "Partition/partition.hpp"
#include "Legalize/legalize.hpp"
#include <thread>

void dump(string outputDir, vector<NODE*> &Cells);

int main(int , char* argv[]){
    // ==== start program =====
    string inputDir = argv[1];
    string outputDir = argv[2];

    cout << "===== Start Parser =====" << endl;
    Parser PR(inputDir);
    PR.Parse();

    vector<NODE*> Cells = PR.Get_Cells();
    vector<NODE*> Terminals = PR.Get_Terminals();
    size_t Chip_Width = PR.Get_Chip_Width();
    size_t Chip_Height = PR.Get_Chip_Height();
    size_t Row_Height = PR.Get_Row_Height();
    size_t Row_Number = PR.Get_Row_Number();
    
    cout << "===== Start Partition =====" << endl;
    Partition PT(Cells, Chip_Height, Row_Height); // change each cell locate
    vector<NODE*> topCells, botCells;
    for(NODE *n : Cells){
        if(n->locate == _TOP) topCells.emplace_back(n);
        if(n->locate == _BOT) botCells.emplace_back(n);
    }

    cout << "===== Start Legalize =====" << endl;
    vector<NODE*> failed_cell_top, failed_cell_bot;
    Legalize LGtop(topCells, Terminals, Chip_Width, Chip_Height, Row_Height, Row_Number);
    Legalize LGbot(botCells, Terminals, Chip_Width, Chip_Height, Row_Height, Row_Number);

    // LGtop.Abacus();
    // LGbot.Abacus();
    // thread first(&Legalize::Abacus, &LGtop);
    // thread second(&Legalize::Abacus, &LGbot);
    // first.join();
    // second.join();

    failed_cell_top = LGtop.GetFailedCell();
    failed_cell_bot = LGbot.GetFailedCell();

    if(failed_cell_top.size() == 0 && failed_cell_bot.size() == 0){
        cout << "success legalize" << endl;
    }
    else if(failed_cell_top.size() != 0 && failed_cell_bot.size() != 0){
        cout << "both chip have failed cell" << endl;
    }
    else{ // put failed to another chip, and then legalize agains
        vector<NODE*> &failed = (failed_cell_top.size() != 0) ? failed_cell_top : failed_cell_bot;
        vector<NODE*> &freeCells = (failed_cell_top.size() != 0) ? botCells : topCells;
        CHIPLOCATE chip = (failed_cell_top.size() != 0) ? _BOT : _TOP;
        for(NODE *n : failed){
            n->locate = chip;
            freeCells.emplace_back(n);
        }
        
        Legalize LG(freeCells, Terminals, Chip_Width, Chip_Height, Row_Height, Row_Number);
        LG.Abacus();
        failed_cell_top = LG.GetFailedCell();
    }

    dump(outputDir, Cells);

    return 0;
}


void dump(string outputDir, vector<NODE*> &Cells){
    ofstream fout(outputDir);

    for(NODE *n : Cells){
        fout << n->name << " " << n->Lx << " " << n->Ly << " " << n->locate << endl;
    }

    fout.close();
}