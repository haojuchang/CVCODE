#include "netlist.h"

// ===== pattern parser =====
void NETLIST::PrintPats(){
    if(_OUTLOG){   
        cout << "\n===== PrintPats =====\n";
        for(NODE* n: PInet){
            cout << n->name << " ";
            for(int i = 0; i < n->output.size(); i++){
                cout << n->output[i] << " ";
            }
            cout << endl;
        }
    }
}
void NETLIST::PatParser(string Dir){
    ifstream in(Dir);
    string str;
    vector<string> input;
    vector<vector<int>> pats;

    while(getline(in, str)){
        vector<string> tok = getToken(str, ", \t");
        if(tok.size() == 0) continue;
        if(tok[0] == "input"){
            for(int i = 1; i < tok.size(); i++){
                input.push_back(tok[i]);
            }
        }
        else if(tok[0] == ".end") break;
        else{
            vector<int> p;
            for(int i = 0; i < tok.size(); i++){
                p.push_back(stoi(tok[i]));
            }
            pats.push_back(p);
        }
    }

    for(int i = 0; i < input.size(); i++){
        NODE* n = allNode[input[i]];
        for(int j = 0; j < pats.size(); j++){
            n->output.push_back(pats[j][i]);
        }
    }    
    in.close();
}
// ===== lib parser =====
void NETLIST::PrintLibs(){
    if(_OUTLOG){
        cout << "\n===== PrintLibs =====\n";
        cout << "_INVX1" << endl;
        PrintLib(_INVX1);
        cout << "_NANDX1" << endl;
        PrintLib(_NANDX1);
        cout << "_NOR2X1" << endl;
        PrintLib(_NOR2X1);
    }
}
void NETLIST::PrintLib(int i){
    CELLLIB *lib;
    lib = LIB[i];

    cout << "capacitance\n";
    for(map<string, double>::iterator it = lib->capacitance.begin(); it != lib->capacitance.end(); it++)
        cout << it->first << " " << it->second << " ";
    cout << endl;

    cout << "cell_rise\n";
    for(int i = 0; i < lib->cell_rise.size(); i++){
        for(int j = 0; j < lib->cell_rise[i].size(); j++){
            cout << lib->cell_rise[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "cell_fall\n";
    for(int i = 0; i < lib->cell_fall.size(); i++){
        for(int j = 0; j < lib->cell_fall[i].size(); j++){
            cout << lib->cell_fall[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "rise_transition\n";
    for(int i = 0; i < lib->rise_transition.size(); i++){
        for(int j = 0; j < lib->rise_transition[i].size(); j++){
            cout << lib->rise_transition[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "fall_transition\n";
    for(int i = 0; i < lib->fall_transition.size(); i++){
        for(int j = 0; j < lib->fall_transition[i].size(); j++){
            cout << lib->fall_transition[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

}
void NETLIST::LibParser(string Dir){
    ifstream in(Dir);
    string str, multiRow = "";
    while(getline(in, str)){
        if(str.find("\r") != string::npos){
            str.erase(str.find("\r"));
        }
        Remove_SinCom(str);
        Remove_SpaceLine(str);
        Remove_Tab(str);
        multiRow += str;
        if(str.size() != 0 && str.find("\\") == string::npos)
            multiRow += "\n";
    }
    Remove_MulCom(multiRow);
    vector<string> lines = getToken(multiRow, "\n");
    int type = 0;
    string pin;
    vector<double> index_1, index_2;

    for(int i = 0; i < lines.size(); i++){
        vector<string> line = getToken(lines[i], " :;{}()\"\\,");
        if(line.size() == 0) continue;
        
        if(line[0] == "cell"){
            CELLLIB *lib = new CELLLIB;
            if(line[1] == "INVX1") {type = _INVX1; LIB[_INVX1] = lib;}
            else if(line[1] == "NANDX1") {type = _NANDX1; LIB[_NANDX1] = lib;}
            else if(line[1] == "NOR2X1") {type = _NOR2X1; LIB[_NOR2X1] = lib;}
        }

        if(line[0] == "pin")
            pin = line[1];
        
        if(line[0] == "index_1"){
            for(int j = 1; j < line.size(); j++)
                index_1.push_back(stod(line[j]));
        }
        else if(line[0] == "index_2"){
            for(int j = 1; j < line.size(); j++)
                index_2.push_back(stod(line[j]));
        }

        if(line[0] == "capacitance"){
            LIB[type]->capacitance[pin] = stod(line[1]);
        }

        if(line[0] == "cell_rise"){
            line = getToken(lines[++i], " :;{}()\"\\,");
            LibInitTalbe(index_1, index_2, line, LIB[type]->cell_rise);
        }
        else if(line[0] == "cell_fall"){
            line = getToken(lines[++i], " :;{}()\"\\,");
            LibInitTalbe(index_1, index_2, line, LIB[type]->cell_fall);
        }
        else if(line[0] == "rise_transition"){
            line = getToken(lines[++i], " :;{}()\"\\,");
            LibInitTalbe(index_1, index_2, line, LIB[type]->rise_transition);
        }
        else if(line[0] == "fall_transition"){
            line = getToken(lines[++i], " :;{}()\"\\,");
            LibInitTalbe(index_1, index_2, line, LIB[type]->fall_transition);
        }
    }

    in.close();
}
void NETLIST::LibInitTalbe(vector<double> &index_1, vector<double> &index_2, vector<string> &value, vector<vector<double>> &tab){
    vector<double> d;
    int row = index_1.size();

    d.push_back(0);
    for(int j = 0; j < index_1.size(); j++){
        d.push_back(index_1[j]);
    }
    tab.push_back(d);

    for(int i = 0; i < index_2.size(); i++){
        d.clear();
        d.push_back(index_2[i]);
        for(int j = i * row + 1; j < (i+1) *row + 1; j++){
            d.push_back(stod(value[j]));
        }
        tab.push_back(d);
    }
}
// ===== verilog parser =====
void NETLIST::PrintNodes(){
    cout << "\n===== PrintNodes =====\n";
    if(!_OUTLOG) return;
    for (auto& x: allNode){
        NODE *node = x.second;
        
        cout << " name: " << node->name;
        cout << " type: ";
        if(node->type == _INVX1) cout << "INV ";
        else if(node->type == _NANDX1) cout << "NAND";
        else if(node->type == _NOR2X1) cout << "NOR ";
        else if(node->type == _NET) cout << "NET ";

        cout << " FI: ";
        for(int i = 0; i < node->FI.size(); i++){
            cout << node->FI[i]->name << " ";
        }
        cout << " FO: ";
        for(int i = 0; i < node->FO.size(); i++){
            cout << node->FO[i]->name << " ";
        }
        cout << "\n";
    } 
}
void NETLIST::VerilogParser(string Dir){
    /* TODO
        read each net/gate from verilog code
        verilog has only  (NAND2, NOR2 , Inverter) 
    */
    ifstream in(Dir);
    string str;
    string multiRow = "";

    while(getline(in, str))
    {
        Remove_SinCom(str);
        Remove_SpaceLine(str);
        Remove_Tab(str);
        multiRow += str + "\n";
    }
    Remove_MulCom(multiRow);

    vector<string> lines = getToken(multiRow, "\n");
    for(int i = 0; i < lines.size(); i++){
        vector<string> line = getToken(lines[i], " (),.;");
        if(line.size() == 0) continue;
        if(line[0] == "module" || line[0] == "endmodule") continue;

        if(line[0] == "input") VerilogParserIntput(line);
        else if(line[0] == "output") VerilogParserOutput(line);
        else if(line[0] == "wire") VerilogParserWire(line);
        else VerilogParserGate(line);
    }
    in.close();
}
void NETLIST::VerilogParserIntput(vector<string> s){
    for(int i = 1; i < s.size(); i++){
        NODE *node = new NODE;
        node->name = s[i];
        node->type = _NET;
        allNode[node->name] = node;
        PInet.insert(node);
    }
}
void NETLIST::VerilogParserOutput(vector<string> s){
    for(int i = 1; i < s.size(); i++){
        NODE *node = new NODE;
        node->name = s[i];
        node->type = _NET;
        allNode[node->name] = node;
        POnet.insert(node);
    }
}
void NETLIST::VerilogParserWire(vector<string> s){
    for(int i = 1; i < s.size(); i++){
        NODE *node = new NODE;
        node->name = s[i];
        node->type = _NET;
        allNode[node->name] = node;
    }
}
void NETLIST::VerilogParserGate(vector<string> s){
    if(s[0] == "NOR2X1"){
        NODE *node = new NODE;
        node->name = s[1];
        node->type = _NOR2X1;
        NODE *A1 = allNode[s[3]];
        NODE *A2 = allNode[s[5]];
        NODE *ZN = allNode[s[7]];
        A1->FOpin.push_back(s[2]);
        A2->FOpin.push_back(s[4]);
        node->FI.push_back(A1);
        node->FI.push_back(A2);
        node->FO.push_back(ZN);
        A1->FO.push_back(node);
        A2->FO.push_back(node);
        ZN->FI.push_back(node);
        allNode[node->name] = node;
    }
    
    if(s[0] == "NANDX1"){
        NODE *node = new NODE;
        node->name = s[1];
        node->type = _NANDX1;
        NODE *A1 = allNode[s[3]];
        NODE *A2 = allNode[s[5]];
        NODE *ZN = allNode[s[7]];
        A1->FOpin.push_back(s[2]);
        A2->FOpin.push_back(s[4]);
        node->FI.push_back(A1);
        node->FI.push_back(A2);
        node->FO.push_back(ZN);
        A1->FO.push_back(node);
        A2->FO.push_back(node);
        ZN->FI.push_back(node);
        allNode[node->name] = node;
    }

    if(s[0] == "INVX1"){
        NODE *node = new NODE;
        node->name = s[1];
        node->type = _INVX1;
        NODE *I = allNode[s[3]];
        NODE *ZN = allNode[s[5]];
        I->FOpin.push_back(s[2]);
        node->FI.push_back(I);
        node->FO.push_back(ZN);
        I->FO.push_back(node);
        ZN->FI.push_back(node);
        allNode[node->name] = node;
    }
}
void NETLIST::Remove_MulCom(string &multiRow){
    // remove multirow comment
    while(multiRow.find("/*") != string::npos){
        multiRow.erase(multiRow.find("/*"), multiRow.find("*/") - multiRow.find("/*") + 2);
    }
}
void NETLIST::Remove_SinCom(string &str){
    // remove single row comment
    if(str.find("//") != string::npos){
        str = str.substr(0, str.find("//"));
    }
}
void NETLIST::Remove_SpaceLine(string &str){
    // skip space line
    if(str == string(str.size(), ' ')) str = "";
}
void NETLIST::Remove_Tab(string &str){
    string multiRow = "";
    for(int i = 0; i < str.size(); i++){
        if(str[i] != '\t'){
            multiRow.push_back(str[i]);
        }
    }
    str = multiRow;
}
// ===== parser end =====

vector<string> NETLIST::getToken(string str, string split){
    vector<string> tokens;
    char *token = strtok((char *)str.c_str(), (char *)split.c_str());
    while (token != NULL)
    {
        tokens.push_back(string(token));
        token = strtok(NULL, (char *)split.c_str());
    }
    return tokens;
}