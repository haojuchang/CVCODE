#ifndef FM_H
#define FM_H

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
using namespace std;

#define _INVX1 1
#define _NANDX1 2
#define _NOR2X1 3
#define _NET 4
#define _POoutload 0.03 // primary output loading = 0.03pF
#define _PItrans 0.0
#define _OUTLOG false


struct NODE;
struct CELLLIB;

class NETLIST{
public:
    NETLIST();
    // step0: 3 parser
    void VerilogParser(string Dir);
    void LibParser(string Dir);
    void PatParser(string Dir);
    void PrintNodes();
    void PrintLibs();
    void PrintPats();

    // step1: loading of each gate
    void CalculateOutputLoading();
    void WriteLoadFile(string Dir);

    // step2:
    void CalculateDelayUnderPats();
    void WriteDelayFile(string Dir);
    // step3:
    void WriteLongestPath(string Dir);


private:
    // element
    unordered_set<NODE*> PInet, POnet;
    unordered_map<string, NODE*> allNode;
    map<int, CELLLIB*> LIB;

    // some func
    double GetInputCapacitance(NODE *gate, string pin);
    void GateOutputInference(NODE *n);
    void GateDelayInference(NODE *n);
    void GetSortedGate(vector<NODE*> &nodes);
    double CheckTable(vector<vector<double>> &tab, double trans, double load);
    double Interpolation(double x1, double x2, double y1, double y2, double x);
    void GetIptTrans(NODE *n, vector<double> &iptTrans);
    void GetIptDelay(NODE *n, vector<double> &iptDelay);

    // parser function
    void VerilogParserIntput(vector<string> s);
    void VerilogParserOutput(vector<string> s);
    void VerilogParserWire(vector<string> s);
    void VerilogParserGate(vector<string> s);
    void Remove_MulCom(string &s);
    void Remove_SinCom(string &s);
    void Remove_SpaceLine(string &s);
    void Remove_Tab(string &s);
    void PrintLib(int i);
    void LibInitTalbe(vector<double> &index_1, vector<double> &index_2, vector<string> &value, vector<vector<double>> &tab);
    vector<string> getToken(string str, string split);
};



struct NODE{ // has gate and net in it
    string name;
    int type; //_NET, _INVX1, _NANDX1, _NOR2X1
    vector<NODE*> FI, FO; // fanin, fanout, net fanout to gate pin
    vector<string> FOpin;
    double outputLoading = 0.;
    vector<int> output;
    vector<double> propagation, transition, delay;
    vector<NODE*> path;
};

struct CELLLIB{
    string name; // NOR2X1
    map<string, double> capacitance; // input pin capacitance (A1, A2)
    vector<vector<double>> cell_rise;
    vector<vector<double>> cell_fall;
    vector<vector<double>> rise_transition;
    vector<vector<double>> fall_transition;
};


class logger{
public:
    logger(string str){
        if(_OUTLOG){
            cout << str << "\n";
        }
    }
};


#endif