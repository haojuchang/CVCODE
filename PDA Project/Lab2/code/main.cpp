#include "Floorplan/floorplan.h"
#include "Parser/parser.hpp"
#include "Bstar/bstar.hpp"

int main(int , char* argv[]){
    srand(time(NULL));

    clock_t start, end;
    double cpu_time_used;
    start = clock();

    // ==== start program =====
    double alpha = stod(argv[1]);
    string blockDir = argv[2];
    string netDir = argv[3];
    string outDir = argv[4];

    Parser pr(blockDir, netDir);
    FloorPlanData FPD = pr.getFloorPlanData();
    FPD.alpha = alpha;

    Bstar bs(FPD);
    bs.Area_SA();
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    bs.dump(outDir, alpha, cpu_time_used);

    size_t not_good = 0;
    double cost = DBL_MAX;
    while(cpu_time_used < 290){
        double new_cost = bs.Inline_SA();
        if(new_cost < cost){
            cost = new_cost;
            end = clock();
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            bs.dump(outDir, alpha, cpu_time_used);
            cout << new_cost << endl;
            not_good = 0;
        }
        else{
            not_good++;
        }
        if(not_good > 100){
            break;
        }

    }
    
    bs.Visualize("result/vis.txt");
    return 0;
}