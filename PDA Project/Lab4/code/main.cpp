#include <iostream>
#include "src/channel.hpp"

using namespace std;

int main(int, char*argv[]){
    srand (time(NULL));
    
    string inputDir = argv[1];
    string outputDir = argv[2];

    ChannelRoute CR;
    CR.Parse(inputDir);
    CR.Run();
    CR.Dump(outputDir);

    return 0;
}