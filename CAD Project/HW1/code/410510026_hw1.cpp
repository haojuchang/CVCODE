#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <string.h>

using namespace std;

class LOGGER{
public:
    LOGGER();
    void n();
    void log(string str);
    void log_n(string str);
    void n_log(string str);
private:
    bool PRINT_LOG = false;
};

LOGGER::LOGGER(){};
void LOGGER::n(){if(this->PRINT_LOG) cout << "\n";}
void LOGGER::log(string str){if(this->PRINT_LOG) cout << str;}
void LOGGER::log_n(string str){if(this->PRINT_LOG) cout << str << "\n";}
void LOGGER::n_log(string str){if(this->PRINT_LOG) cout << "\n" << str;}


typedef pair<string, bool> TERM;
typedef vector<TERM> ONES;
typedef vector<ONES> ONESTABLE;
typedef vector<ONESTABLE> QMTABLE;

class Quine_McCluskey {
public:
    Quine_McCluskey(vector<string> &table, int inputN); // 直接化簡輸入的 truth table
    Quine_McCluskey(vector<string> table); // 找 truth table 中 0/1 那個少作化簡
    vector<string> Solve(); // return Prime_Impli

    QMTABLE init_QMtable();
    ONESTABLE init_onesTable();
    bool Impli();
    ONES findMerges(string term, int onesCnt);
    void detGetSet(); // determine getSet

    vector<string> Get_Truth_Table(); // output table
    vector<string> Get_getSetTable();
    char Get_getSet(); // return getSet
private:
    vector<string> table; // init 存 truth table
    vector<string> getSetTable; // 之後存 getSet table
    char getSet; // 如果 output = 1 多，getSet = 1
    int inputN; // input 個數 (=有幾個變數)
    QMTABLE QMtable;
};

struct NODE{
    string imp;
    list<NODE*> fanin;
    list<NODE*> fanout;
};

class Minimize_Prime_Impli{
public:
    Minimize_Prime_Impli(vector<string> &set, vector<string> &Prime_Impli);
    vector<string> Solve();

private:
    void removePInode(NODE *pi);
    vector<string> openDC(string prime); // 把dc打開
    list<NODE*> PI, PO;
    map<string, NODE*> mapPO;
};

Quine_McCluskey::Quine_McCluskey(vector<string> &table, int inputN)
{
    this->getSetTable = table;
    this->inputN = inputN;
}

Quine_McCluskey::Quine_McCluskey(vector<string> table)
{
    this->table = table;
    this->inputN = table[0].size() - 1;
    this->detGetSet();
}

vector<string> Quine_McCluskey::Get_Truth_Table()
{
    return this->table;
}

vector<string> Quine_McCluskey::Get_getSetTable()
{
    return this->getSetTable;
}

char Quine_McCluskey::Get_getSet()
{
    return this->getSet;
}

void Quine_McCluskey::detGetSet()
{
    int _1 = 0, _0 = 0; // the number of 1/0
    vector<string> _1vec, _0vec;
    for (int i = 0; i < this->table.size(); i++)
    {
        if (this->table[i][this->inputN] == '1')
        {
            _1++;
            string str = this->table[i];
            str.pop_back();
            _1vec.push_back(str);
        }
        else
        {
            _0++;
            string str = this->table[i];
            str.pop_back();
            _0vec.push_back(str);
        }
    }
    if(_1 == 0){ // output 全都是0
        this->getSet = '0';
    }
    else if (_0 == 0){ // output 全都是1
        this->getSet = '1';
    }
    else if (_1 <= _0)
    {
        this->getSet = '1';
        this->getSetTable = _1vec;
    }
    else
    {
        this->getSet = '0';
        this->getSetTable = _0vec;
    }
}

ONESTABLE Quine_McCluskey::init_onesTable()
{
    // ex: 4 input 有 5 個 ones: 0個1 ~ 4個1
    ONESTABLE onesTable;
    for (int i = 0; i < this->inputN + 1; i++)
    {
        ONES ones;
        onesTable.push_back(ones);
    }
    return onesTable;
}

QMTABLE Quine_McCluskey::init_QMtable()
{
    QMTABLE QMtable;

    ONESTABLE onesTable = this->init_onesTable();

    for (int i = 0; i < this->getSetTable.size(); i++)
    {
        int onesCnt = 0;
        for (int j = 0; j < inputN; j++)
        {
            if (this->getSetTable[i][j] == '1')
                onesCnt++;
        }
        TERM term(this->getSetTable[i], false);
        onesTable[onesCnt].push_back(term);
    }

    QMtable.push_back(onesTable);
    return QMtable;
}

ONES Quine_McCluskey::findMerges(string term, int onesCnt)
{

    set<TERM> tmp;
    int last = this->QMtable.size() - 1;
    for (int i = 0; i < this->QMtable[last][onesCnt].size(); i++)
    {
        int diff = -1;
        for (int j = 0; j < this->inputN; j++)
        { // 找兩個 term 唯一的不一樣
            if (term[j] != this->QMtable[last][onesCnt][i].first[j])
            {
                if (diff == -1)
                    diff = j;
                else
                { // 代表有兩個以上不一樣
                    diff = -1;
                    break;
                }
            }
        }
        if (diff != -1)
        {
            this->QMtable[last][onesCnt][i].second = true;
            string s = term;
            s[diff] = '-';
            TERM t(s, false);
            // ones.push_back(t);
            tmp.insert(t);
        }
    }
    ONES ones(tmp.begin(), tmp.end());
    return ones;
}

bool Quine_McCluskey::Impli()
{
    bool hasImpli = false;

    ONESTABLE onesTable = this->init_onesTable();

    int last = this->QMtable.size() - 1;

    for (int onesCnt = 0; onesCnt < this->inputN; onesCnt++)
    { // 1 的個數
        for (int i = 0; i < this->QMtable[last][onesCnt].size(); i++)
        { // 一個 term
            // 找這一個 term 可以和下一個"一的個數" merge 後的 term
            ONES ones = this->findMerges(this->QMtable[last][onesCnt][i].first, onesCnt + 1);

            if (ones.size() != 0)
            {
                hasImpli = true;
                this->QMtable[last][onesCnt][i].second = true;
                for (int j = 0; j < ones.size(); j++)
                    onesTable[onesCnt].push_back(ones[j]);
            }
        }
    }
    this->QMtable.push_back(onesTable);
    return hasImpli;
}

vector<string> Quine_McCluskey::Solve()
{
    vector<string> Prime_Impli;
    set<string> tmp;

    this->QMtable = this->init_QMtable();

    // 控制 merge 幾個
    // double start = time(NULL);
    while (Impli())
    {
        // double end = time(NULL);
        // if(difftime(end, start) > 2) break;
        // break;
    }

    for (int i = 0; i < QMtable.size(); i++)
    { // iterate onestable
        for (int j = 0; j < QMtable[i].size(); j++)
        { // iterate ones
            for (int k = 0; k < QMtable[i][j].size(); k++)
            { // iterate term
                if (QMtable[i][j][k].second == false)
                {
                    tmp.insert(QMtable[i][j][k].first);
                }
            }
        }
    }

    set<string>::iterator it;
    for (it = tmp.begin(); it != tmp.end(); ++it)
    {
        Prime_Impli.push_back(*it);
    }

    return Prime_Impli;
}

// int main(){
//     vector<string>table;
//     table.push_back("00001"); // PI=4, PO=1
//     table.push_back("00010");
//     table.push_back("00100");
//     table.push_back("00110");
//     table.push_back("01000");
//     table.push_back("01010");
//     table.push_back("01101");
//     table.push_back("01111");
//     table.push_back("10000");
//     table.push_back("10010");
//     table.push_back("10100");
//     table.push_back("10110");
//     table.push_back("11001");
//     table.push_back("11011");
//     table.push_back("11100");
//     table.push_back("11110");
//     // table.push_back("0000");
//     // table.push_back("0010");
//     // table.push_back("0100");
//     // table.push_back("0111");
//     // table.push_back("1001");
//     // table.push_back("1010");
//     // table.push_back("1101");
//     // table.push_back("1110");
//     // table.push_back("000");
//     // table.push_back("010");
//     // table.push_back("100");
//     // table.push_back("110");

//     Quine_McCluskey qm = Quine_McCluskey(table);
//     cout << "getSet: " << qm.Get_getSet() << endl;
//     cout << "==getSetTable==" << endl;
//     vector<string> getSetTable = qm.Get_getSetTable();
//     for(int i = 0; i < getSetTable.size(); i++){
//         cout << getSetTable[i] << endl;
//     }

//     vector<string> Prime_Impli = qm.Solve();
//     cout << "==Prime_Impli==" << endl;
//     for(int i = 0; i < Prime_Impli.size(); i++){
//         cout << Prime_Impli[i] << endl;
//     }
//     return 0;
// }

// =================================================================
Minimize_Prime_Impli::Minimize_Prime_Impli(vector<string> &set, vector<string> &Prime_Impli){
    // init PO and mapPO
    for(int i = 0; i < set.size(); i++){
        NODE *node = new NODE;
        node->imp = set[i];
        this->PO.push_back(node);
        mapPO[node->imp] = node;
    }

    // init PI
    for(int i = 0; i < Prime_Impli.size(); i++){
        NODE *node = new NODE;
        node->imp = Prime_Impli[i];
        vector<string> open = this->openDC(node->imp);
        for(int j = 0; j < open.size(); j++){
            map<string, NODE*>::iterator it = this->mapPO.find(open[j]);
            if(it != this->mapPO.end()){
                node->fanout.push_back(it->second);
                it->second->fanin.push_back(node);
            }
        }
        this->PI.push_back(node);
    }
    // cout << endl;
    // for (list<NODE*>::iterator it = PI.begin(); it != PI.end(); ++it){
    //     NODE *node = *it;
    //     cout << node->imp << endl;
    //     for (list<NODE*>::iterator it2 = node->fanout.begin(); it2 != node->fanout.end(); ++it2){
    //         NODE *node2 = *it2;
    //         cout << node2->imp << endl;
    //     }
    //     cout << endl;
    // }
    // for (list<NODE*>::iterator it = PO.begin(); it != PO.end(); ++it){
    //     NODE *node = *it;
    //     cout << node->imp << endl;
    //     for (list<NODE*>::iterator it2 = node->fanin.begin(); it2 != node->fanin.end(); ++it2){
    //         NODE *node2 = *it2;
    //         cout << node2->imp << endl;
    //     }
    //     cout << endl;
    // }
}

vector<string> Minimize_Prime_Impli::openDC(string prime){
    vector<string> open;
    open.push_back("");
    for(int i = 0; i < prime.size(); i++){
        if(prime[i] == '-'){
            int size = open.size();
            for(int j = 0; j < size; j++){ // duplicate open
                open.push_back(open[j]);
            }
            for(int j = 0; j < size; j++){ // half append 0
                open[j].push_back('0');
            }
            for(int j = size; j < open.size(); j++){ // half append 0
                open[j].push_back('1');
            }
        }
        else{
            for(int j = 0; j < open.size(); j++){
                open[j].push_back(prime[i]);
            }
        }
    }
    return open;
}

vector<string> Minimize_Prime_Impli::Solve(){
    vector<string> min;
    while(PO.size() > 0){
        vector<NODE*> ess;
        for(list<NODE*>::iterator it = this->PO.begin(); it != this->PO.end(); ++it){
            NODE *node = *it;
            if(node->fanin.size() == 1){
                ess.push_back(node->fanin.front());
            }
        }
        for(int i = 0; i < ess.size(); i++){
            min.push_back(ess[i]->imp);
            this->removePInode(ess[i]);
        }

        for (list<NODE*>::iterator it = PI.begin(); it != PI.end(); ++it){
            NODE *node = *it;
            for (list<NODE*>::iterator it2 = node->fanout.begin(); it2 != node->fanout.end(); ++it2){
                NODE *node2 = *it2;
            }
        }
        for (list<NODE*>::iterator it = PO.begin(); it != PO.end(); ++it){
            NODE *node = *it;
            for (list<NODE*>::iterator it2 = node->fanin.begin(); it2 != node->fanin.end(); ++it2){
                NODE *node2 = *it2;
            }
        }
        
        if(PO.size() == 0) break;
        
        NODE* max = this->PI.front();
        for(list<NODE*>::iterator it = this->PI.begin(); it != this->PI.end(); ++it){
            NODE *node = *it;
            if(node->fanout.size() > max->fanout.size())
                max = node;
        }
        min.push_back(max->imp);
        this->removePInode(max);
    }
    return min;
}

void Minimize_Prime_Impli::removePInode(NODE *pi){
    vector<NODE*> outs;
    for(list<NODE*>::iterator it = pi->fanout.begin(); it != pi->fanout.end(); ++it){
        NODE *out = *it;
        for(list<NODE*>::iterator it2 = out->fanin.begin(); it2 != out->fanin.end(); ++it2){
            if((*it2) != pi)
                (*it2)->fanout.remove(out);
        }
        outs.push_back(out);
    }
    for(int i = 0; i < outs.size(); i++){
        PO.remove(outs[i]);
    }
    PI.remove(pi);
}

struct DATA{
    size_t varNums;
    vector<int> onset, dcset;
};

void read_input(string inputDir, DATA &data);
vector<string> getToken(string str, string split);
string toBit(int n, int digit);
bool vecSort(string a, string b);
void writePrimeFile(vector<string> &prime, DATA &data, string dir);
void writeMiniFile(vector<string> &mini, string dir);

int main(int argc, char* argv[]){
    LOGGER LOG;
    LOG.log("\n===== start program =====\n");

    string inputDir = argv[1];
    string implicantDir = argv[2];
    string outputDir = argv[3];

    LOG.log("inputDir: "+inputDir+"\n");
    LOG.log("implicantDir: "+implicantDir+"\n");
    LOG.log("outputDir: "+outputDir+"\n\n");

    
    LOG.log("\n===== case info. =====\n");
    DATA data;
    read_input(inputDir, data);
    LOG.log_n("varNums: "+to_string(data.varNums));
    LOG.log("onset: ");
    for(int i = 0; i < data.onset.size(); i++){
        LOG.log(to_string(data.onset[i]) + " ");
    }
    LOG.n();
    LOG.log("dcset: ");
    for(int i = 0; i < data.dcset.size(); i++){
        LOG.log(to_string(data.dcset[i]) + " ");
    }
    LOG.n();

    vector<string> table, ontable;
    for(int i = 0; i < data.onset.size(); i++){
        table.push_back(toBit(data.onset[i], data.varNums));
        ontable.push_back(toBit(data.onset[i], data.varNums));
    }
    for(int i = 0; i < data.dcset.size(); i++){
        table.push_back(toBit(data.dcset[i], data.varNums));
    }

    LOG.log_n("\n===== int to bit string =====");
    for(int i = 0; i < table.size(); i++){
        LOG.log_n(table[i]);
    }

    Quine_McCluskey qm = Quine_McCluskey(table, data.varNums);
    LOG.log_n("\n===== Prime_Impli =====");
    vector<string> Prime_Impli = qm.Solve();
    sort(Prime_Impli.begin(), Prime_Impli.end(), vecSort);
    for(int i = 0; i < Prime_Impli.size(); i++){
        LOG.log_n(Prime_Impli[i]);
    }
    writePrimeFile(Prime_Impli, data, implicantDir);

    Minimize_Prime_Impli mpi = Minimize_Prime_Impli(ontable, Prime_Impli);
    vector<string> mini = mpi.Solve();
    sort(mini.begin(), mini.end(), vecSort);
    LOG.log_n("\n===== mini =====");
    for(int i = 0; i < mini.size(); i++){
        LOG.log_n(mini[i]);
    }
    writeMiniFile(mini, outputDir);

    return 0;
}

void writeMiniFile(vector<string> &mini, string dir){
    ofstream out(dir);
    out << ".mc " << mini.size() << "\n";
    for(int i = 0; i < mini.size(); i++)
        out << mini[i] << "\n";
    
    int lit = 0;
    for(int i = 0; i < mini.size(); i++){
        for(int j = 0; j < mini[i].size(); j++){
            if(mini[i][j] != '-')
                lit++;
        }
    }
    out << "literal=" << lit;
}
void writePrimeFile(vector<string> &prime, DATA &data, string dir){
    ofstream out(dir);
    out << ".i " << data.varNums << "\n";
    out << ".m\n";
    for(int i = 0; i < data.onset.size(); i++)
        out << data.onset[i] << " ";
    out << "\n.d\n";
    for(int i = 0; i < data.dcset.size(); i++)
        out << data.dcset[i] << " ";
    out << "\n.p " << prime.size() << "\n";
    
    int outcount = (prime.size() > 20) ? 20 : prime.size();
    for(int i = 0; i < outcount; i++){
        out << prime[i];
        if(i != outcount - 1)
            out << "\n";
    }
}

bool vecSort(string a, string b){
    int a_ = 0, b_ = 0;
    for(int i = 0; i < a.size(); i++)
        if(a[i] == '-')
            a_++;
    for(int i = 0; i < b.size(); i++)
        if(b[i] == '-')
            b_++;
    if(a_ == b_) 
        return a > b;
    else 
        return a_ > b_;
}

string toBit(int n, int digit)
{
    string r;
    while (n != 0){
        r = to_string(n & 1) + r;
        n = n >> 1;
    }

    int diff = digit - r.size();
    string zeros(diff, '0');
    r = zeros + r;
    return r;
}

void read_input(string inputDir, DATA &data){
    LOGGER LOG;
    ifstream in(inputDir);
    string str;
    vector<string> allinput;
    while(getline(in, str)){
        if(str.size() == 0) continue;
        allinput.push_back(str);
    }

    for(int i = 0; i < allinput.size(); i++){
        str = allinput[i];
        vector<string> tok = getToken(str, " ");
        if(tok.size() == 0) continue;

        if(tok[0] == ".i") data.varNums = stoi(tok[1]);

        if(tok[0] == ".m"){
            str = allinput[++i];
            tok = getToken(str, " ");
            for(int i = 0; i < tok.size(); i++)
                data.onset.push_back(stoi(tok[i]));
        }

        if(tok[0] == ".d"){
            str = allinput[++i];
            tok = getToken(str, " ");
            for(int i = 0; i < tok.size(); i++)
                data.dcset.push_back(stoi(tok[i]));
        }
    }
}

vector<string> getToken(string str, string split){
    vector<string> tokens;
    char *token = strtok((char *)str.c_str(), (char *)split.c_str());
    while (token != NULL)
    {
        tokens.push_back(string(token));
        token = strtok(NULL, (char *)split.c_str());
    }
    return tokens;
}