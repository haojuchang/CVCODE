#include "netlist.h"

int main(int argc, char *argv[])
{
    string VerilogDir = argv[1];
    string PatDir = argv[3];
    string LibDir = argv[5];
    string student_id = "410510026";
    string case_name = VerilogDir.substr(VerilogDir.find_last_of("/")+1, VerilogDir.find(".") - VerilogDir.find_last_of("/") - 1);

    logger("\n===== Program Parameter =====");
    logger("VerilogDir: " + VerilogDir);
    logger("PatDir: " + PatDir);
    logger("LibDir: " + LibDir);
    logger("student_id: " + student_id);
    logger("case_name: " + case_name);

    NETLIST netlist;

    // step0: parser
    netlist.VerilogParser(VerilogDir);
    netlist.LibParser(LibDir);
    netlist.PatParser(PatDir);
    // netlist.PrintLibs();
    // netlist.PrintNodes();
    // netlist.PrintPats();

    // step1: loading
    netlist.CalculateOutputLoading();
    netlist.WriteLoadFile(student_id+"_"+case_name+"_load.txt");

    // step2:
    netlist.CalculateDelayUnderPats();
    netlist.WriteDelayFile(student_id+"_"+case_name+"_delay.txt");

    // step3:
    netlist.WriteLongestPath(student_id+"_"+case_name+"_path.txt");
    return 0;
}