#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include "structure.h"
using namespace std;

string IO_INFO;
string IOGEN;
string CIRCUIT;
string TEMPCIRCUIT;
string IN_PAT;
string IO_REL;
string FOLDER_POS;
int superInput = 17;

time_t start;

void read_io_info(IO &io, string io_info);
void testDiff(IO &io);
void creat_randomPat(IO &io, int bitCount);
string creatOnePat(int bit);
void gen_io_rel(IO &io, string in_pat);
void read_io_rel(IO &io, string io_rel);
void put_iogen(string iogen, string in_pat, string io_rel);
void initModeling(IO &io, vector<Tree> &tree_s);
bool modifyModeling(IO &io, vector<Tree> &tree_s);
bool cp_pair_db_int_fr(pair<double, int> i, pair<double, int> j);
bool cp_pair_db_int_sc(pair<double, int> i, pair<double, int> j);
void creat_init_pat(IO &io, int outbit);
void deterGetSet(IO &io, int outbit);
vector<string> getToken(string str, string split);
void outlog(string str);
ONESTABLE genQMinput(Tree &tree, int inputN);

int main(int argc, char **argv){
	IO io;
	IO_INFO = string(argv[1]);
	IOGEN = string(argv[2]);
	CIRCUIT = string(argv[3]);
	int found = CIRCUIT.find_last_of("/\\");
	if (found != -1)
		FOLDER_POS = CIRCUIT.substr(0, found + 1);
	else
		FOLDER_POS = "./";

	TEMPCIRCUIT = FOLDER_POS + "TEMP.v";
	IN_PAT = FOLDER_POS + "in_pat.txt";
	IO_REL = FOLDER_POS + "io_rel.txt";
	read_io_info(io, IO_INFO);

	if (io.inputN < superInput)
	{
		superInput = io.inputN;
	}

	cout << "\ninputN: " << io.inputN << " outputN: " << io.outputN << endl;
	cout << "superInput: " << superInput << endl;

	testDiff(io);

	vector<Tree> tree_s;
	initModeling(io, tree_s);
	while(!modifyModeling(io, tree_s)){};
	cout << "end modeling\n\n" << endl;

	vector<string> data;
	tree_s[0].get_tree_getset_data("", tree_s[0].tree, data);
	for(int i = 0; i < data.size(); i++){
		cout << data[i] << endl;
	}

	// ONESTABLE onestable = genQMinput(tree_s[0], io.inputN);

	// for(int i = 0; i < onestable.size(); i++){
	// 	cout << "ones count: " << i << "\n";
	// 	for(int j = 0; j < onestable[i].size(); j++){
	// 		cout << onestable[i][j] << endl;
	// 	}
	// }
	return 0;
}

void read_io_info(IO &io, string io_info)
{
	ifstream inputFile;
	inputFile.open(io_info, ios::in);

	if (!inputFile)
	{
		cerr << "File could not be open at read_io_info()" << endl;
		exit(1);
	}

	string str;
	vector<string> tok;

	getline(inputFile, str);
	tok = getToken(str, " ");
	io.inputN = stoi(tok[0]);
	io.outputN = stoi(tok[1]);


	getline(inputFile, str);
	tok = getToken(str, " ");

	for(int i = 0; i < tok.size(); i++){
		io.IOvariable.push_back(tok[i]);
	}

	inputFile.close();
}

void testDiff(IO &io)
{
	io.diff = vector<vector<pair<double, int>>>(io.outputN);
	for (int bit = 0; bit < io.inputN; bit++)
	{
		creat_randomPat(io, 10);
		// ��J �� 0,1 pattern
		string all0(io.inputN, '0');
		string all1(io.inputN, '1');
		io.pat.push_back(all0);
		io.pat.push_back(all1);

		for (int i = 0; i < io.inputN; i++) {
			string one1(io.inputN, '0');
			one1[i] = '1';
			string one0(io.inputN, '1');
			one0[i] = '0';
			io.pat.push_back(one1);
			io.pat.push_back(one0);
		}
		int size = io.pat.size();

		// �ƻs�t�@������� pattern
		for (int i = 0; i < size; i++)
		{
			io.pat[i][bit] = '0';
			string t = io.pat[i];
			t[bit] = '1';
			io.pat.push_back(t);
		}
		gen_io_rel(io, IN_PAT);
		// cout << io.rel.size() << endl;
		for (int outbit = io.inputN; outbit < io.inputN + io.outputN; outbit++)
		{
			int same = 0;
			//int out = 0;
			for (int i = 0; i < size; i++)
			{
				if (io.rel[i][outbit] == io.rel[i + size][outbit])
					same++;
			}
			pair<double, int> d;
			d.first = (double)same / (double)size;
			d.second = bit;
			io.diff[outbit - io.inputN].push_back(d);
		}
	}
	for (int i = 0; i < io.diff.size(); i++)
	{
		int dummyCount = 0;
		for (int j = 0; j < io.diff[i].size(); j++)
		{
			if (io.diff[i][j].first >= 1)
			{
				dummyCount++;
			}
		}
		io.dummyCount.push_back(dummyCount);
	}
	ofstream dummySave(FOLDER_POS + "diffSave.txt");
	ofstream fout(FOLDER_POS + "diff.csv");
	for (int i = 0; i < io.diff.size(); i++)
	{
		fout << "outbit:" << i << "," << endl;
		int count = 0;
		for (int j = 0; j < io.diff[i].size(); j++)
		{
			if (io.diff[i][j].first >= 1)
			{
				count++;
			}
			dummySave << (int)io.diff[i][j].first << " ";
			fout << io.diff[i][j].first << ",";
		}
		fout << endl;
		dummySave << "\noutput:" << i << " diff:" << count << " realInput:" << io.inputN - count << " name:" << io.IOvariable[io.inputN + i] << endl;
		dummySave << endl;
	}
	dummySave.close();
	fout.close();
}

void creat_randomPat(IO &io, int bitCount)
{
	// �p�G����bit�Ƥp��bitCount
	if (io.inputN < bitCount)
		bitCount = io.inputN;
	// �⦸����ন�Q�i��
	int patCounts = pow(2, bitCount);
	int t;
	string ts = "";
	io.pat.clear();
	for (int i = 0; i < patCounts; i++)
	{
		ts = creatOnePat(io.inputN);
		io.pat.push_back(ts);
		ts = "";
	}

	string all0(io.inputN, '0');
	string all1(io.inputN, '1');
	io.pat.push_back(all0);
	io.pat.push_back(all1);

	for (int i = 0; i < io.inputN; i++) {
		string one1(io.inputN, '0');
		one1[i] = '1';
		string one0(io.inputN, '1');
		one0[i] = '0';
		io.pat.push_back(one1);
		io.pat.push_back(one0);
	}
	for (int i = 0; i < io.inputN - 1; i++)
	{
		for (int j = i + 1; j < io.inputN; j++)
		{
			string two1(io.inputN, '0');
			two1[i] = '1';
			two1[j] = '1';
			string two0(io.inputN, '1');
			two0[i] = '0';
			two0[j] = '0';
			io.pat.push_back(two1);
			io.pat.push_back(two0);
		}
	}
}

string creatOnePat(int bit)
{

	int t;
	string ts = "";
	for (int j = 0; j < bit; j++)
	{
		t = rand() % 2;
		ts.push_back(char(48 + t));
	}
	return ts;
}

void gen_io_rel(IO &io, string in_pat)
{
	//remove(in_pat.c_str());
	string rm = "rm " + in_pat;

	unsigned two19 = pow(2, 19);
	io.rel.clear();
	io.rel.shrink_to_fit();
	io.rel.reserve(io.pat.size());

	for (unsigned s = 0, end = two19; s < io.pat.size(); s += two19, end += two19)
	{
		system(rm.c_str());
		ofstream fout(in_pat);
		if (!fout)
		{
			cerr << "File could not be write at gen_io_rel()" << endl;
			exit(1);
		}

		// first line
		fout << io.inputN << " " << ((end > io.pat.size()) ? io.pat.size() - s : two19) << "\n";

		// second line
		fout << io.IOvariable[0];
		for (int i = 1; i < io.inputN; i++)
		{
			fout << " " << io.IOvariable[i];
		}
		fout << "\n"; // the last one

		// input pattern
		for (unsigned i = s; i < end && i < io.pat.size(); i++)
		{
			fout << io.pat[i][0];
			for (int j = 1; j < io.pat[i].size(); j++)
			{
				fout << " " << io.pat[i][j];
			}
			fout << "\n";
		}
		fout.close();

		put_iogen(IOGEN, IN_PAT, IO_REL);
		read_io_rel(io, IO_REL);
	}
}

void read_io_rel(IO &io, string io_rel)
{
	ifstream inputFile;
	inputFile.open(io_rel, ios::in);

	if (!inputFile)
	{
		cerr << "File could not be open at read_io_rel()" << endl;
		exit(1);
	}

	// read txt into file
	vector<string> file;
	string str;
	while (!inputFile.eof() && getline(inputFile, str, '\n'))
	{
		file.push_back(str);
	}
	// io.rel.clear();
	for (int i = 2; i < file.size(); i++)
	{
		string rel = "";
		for (int j = 0; j < file[i].size(); j += 2)
		{
			rel.push_back(file[i][j]);
		}
		io.rel.push_back(rel);
	}
	inputFile.close();
}

void put_iogen(string iogen, string in_pat, string io_rel)
{
	string rm = "rm " + io_rel;
	system(rm.c_str());

	string command = iogen + " " + in_pat + " " + io_rel;
	int i = -1;
	i = system(command.c_str());
	if (i != 0)
	{
		cout << command << endl;
		cout << "Fail at put_iogen()" << i << endl;
		exit(1);
	}
}

bool cp_pair_db_int_fr(pair<double, int> i, pair<double, int> j)
{
	return (i.first < j.first);
}

bool cp_pair_db_int_sc(pair<double, int> i, pair<double, int> j)
{
	return (i.second < j.second);
}

void initModeling(IO &io, vector<Tree> &tree_s)
{
	unsigned beforMerge = 0;
	
	//vector<string> temptemp = io.pat;
	for (int i = 0; i < io.outputN; i++)
	{
		vector<pair<double, int>> diff = io.diff[i];
		sort(diff.begin(), diff.end(), cp_pair_db_int_fr);
		vector<pair<double, int>> diffSave, diffNoNeed, diffSaveTemp;

		int MaxBit = (io.inputN >= superInput) ? superInput : io.inputN;
		for (int d = 0; d < io.inputN; d++) {
			if (diff[d].first < 1 && diffSave.size() < MaxBit) {
				diffSave.push_back(diff[d]);
			}
			else
			{
				diffNoNeed.push_back(diff[d]);
			}
		}
		diffSaveTemp = diffSave; // ���o�έ��n�ʱƧǪ� diffSave
		sort(diffSave.begin(), diffSave.end(), cp_pair_db_int_sc); // �έ쥻��m�Ƨ� diffSave

		creat_init_pat(io, i);
		beforMerge += io.pat.size();

		//bool isRan = false;
		gen_io_rel(io, IN_PAT);
		deterGetSet(io, i);
		/*0903*/
		Tree tree;
		tree.tree->data = io.getSet[i]; // �w�� realpi = 0
		for (int j = 0; j < io.rel.size(); j++)
		{
			string str = "";
			for (int b = 0; b < diffSaveTemp.size(); b++)
				str.push_back(io.rel[j][diffSaveTemp[b].second]);
			char out = io.rel[j][io.inputN + i];
			tree.create_init_tree_one_pat(str, out);
		}
		
		int patCounts = pow(2, 17);
		io.pat.clear();
		for (int j = 0; j < patCounts; j++)
		{
			string ts = creatOnePat(io.inputN);
			io.pat.push_back(ts);
		}
		gen_io_rel(io, IN_PAT);
		vector<string>errpat;
		double acc = 0.0;
		errpat = tree.test_tree_acc(io.rel, diff, i, io.inputN, acc);
		tree_s.push_back(tree);
	}

	cout << "\nInit acc of each primary output" << endl;
	for(int i = 0; i < tree_s.size(); i++){
		cout << "PO: " << i << " Acc: " << tree_s[i].acc << endl;
	}
}

bool modifyModeling(IO &io, vector<Tree> &tree_s)
{
	int count = 0;
	string blifDir = FOLDER_POS + "circuit.blif";
	
	bool satisfy = true;
	for (int i = 0; i < io.outputN; i++){
		if (tree_s[i].acc < 99.99)
			satisfy = false;
	}
	if(satisfy)
		return true;

	int minOut = 0;
	for (int i = 0; i < io.outputN; i++)
	{
		if (tree_s[i].acc < tree_s[minOut].acc)
		{
			minOut = i;
		}
	}
	vector<pair<double, int>> diff = io.diff[minOut];
	sort(diff.begin(), diff.end(), cp_pair_db_int_fr);

	int patCounts = pow(2, 17);
	io.pat.clear();
	for (int j = 0; j < patCounts; j++)
	{
		string ts = creatOnePat(io.inputN);
		io.pat.push_back(ts);
	}
	gen_io_rel(io, IN_PAT);
	vector<string>errpat;
	double acc = 0.0;
	errpat = tree_s[minOut].test_tree_acc(io.rel, diff, minOut, io.inputN, acc);

	cout << "modify PO: " << minOut << " acc: " << acc << endl;
	if (errpat.size() != 0)
	{
		vector<string> copy = errpat;
		for (int j = 0; j < errpat.size(); j++) {
			errpat[j].push_back('0');
			copy[j].push_back('1');
		}
		errpat.insert(errpat.end(), copy.begin(), copy.end());

		for (int j = 0; j < errpat.size(); j++)
		{
			for (int b = 0; b < diff.size() - errpat[j].size(); b++)
			{
				int t = rand() % 2;
				errpat[j].push_back(char(48 + t));
			}
		}

		io.pat.clear();
		for (int j = 0; j < errpat.size(); j++)
		{
			string p(diff.size(), '0');
			for (int b = 0; b < errpat[j].size(); b++)
			{
				p[diff[b].second] = errpat[j][b];
			}
			io.pat.push_back(p);
		}
		gen_io_rel(io, IN_PAT);
		tree_s[minOut].modify_tree(io.rel, diff, minOut, io.inputN);
	}	
	
	satisfy = true;
	for (int i = 0; i < io.outputN; i++){
		if (tree_s[i].acc < 99.99)
			satisfy = false;
	}
	return satisfy;
}

ONESTABLE genQMinput(Tree &tree, int inputN){
	ONESTABLE onestable;
	for(int i = 0; i <= inputN; i++){
		ONES ones;
		onestable.push_back(ones);
	}

	vector<string> data;
	tree.get_tree_getset_data("", tree.tree, data);


	for(int i = 0; i < data.size(); i++){
		int _1 = 0;
		for(int j = 0; j < data[i].size(); j++){
			if(data[i][j] == '1') _1++;
		}
		onestable[_1].push_back(data[i]);
	}

	return onestable;
}

// gray code 
void creat_init_pat(IO &io, int outbit)
{
	vector<pair<double, int>> diff = io.diff[outbit];
	sort(diff.begin(), diff.end(), cp_pair_db_int_fr);
	vector<pair<double, int>> diffSave, diffNoNeed;
	int MaxBit = (io.inputN >= superInput) ? superInput : io.inputN;

	for (int d = 0; d < io.inputN; d++) {
		if (diff[d].first < 1 && diffSave.size() < MaxBit) {
			diffSave.push_back(diff[d]);
		}
		else
		{
			diffNoNeed.push_back(diff[d]);
		}
	}
	int bitCount = diffSave.size();
	if (bitCount == 0) {
		io.pat.clear();
		string p(io.inputN, '0');
		//for (int vote = 0; vote < voteInitPat; vote++)
		io.pat.push_back(p);
	}
	else {
		vector<string> pat;
		pat.push_back("0");
		pat.push_back("1");

		for (int i = 0; i < bitCount - 1; i++) {
			vector<string> copy = pat;
			for (int j = 0; j < pat.size(); j++) {
				pat[j] = "0" + pat[j];
				copy[j] = "1" + copy[j];
			}
			pat.insert(pat.end(), copy.begin(), copy.end());
		}

		sort(diffNoNeed.begin(), diffNoNeed.end(), cp_pair_db_int_sc);
		io.pat.clear();
		for (int i = 0; i < pat.size(); i++)
		{
			string p = pat[i];
			for (int j = 0; j < diffNoNeed.size(); j++)
			{
				int t = rand() % 2;
				p.insert(p.begin() + diffNoNeed[j].second, char(48 + t));
			}
			io.pat.push_back(p);
		}

		/*io.pat.clear();
		io.pat = pat;*/
		string all0(io.inputN, '0');
		string all1(io.inputN, '1');
		
		io.pat.push_back(all0);
		io.pat.push_back(all1);

		for (int i = 0; i < io.inputN; i++) {
			string one1(io.inputN, '0');
			one1[i] = '1';
			string one0(io.inputN, '1');
			one0[i] = '0';
			io.pat.push_back(one1);
			io.pat.push_back(one0);
		}

		for (int i = 0; i < io.inputN - 1; i++)
		{
			for (int j = i + 1; j < io.inputN; j++)
			{
				string two1(io.inputN, '0');
				two1[i] = '1';
				two1[j] = '1';
				string two0(io.inputN, '1');
				two0[i] = '0';
				two0[j] = '0';
				io.pat.push_back(two1);
				io.pat.push_back(two0);
			}
		}
	}
}

void deterGetSet(IO &io, int outbit)
{
	char getSet;
	int is0 = 0;
	int is1 = 0;
	int i = io.inputN + outbit;
	for (int j = 0; j < io.rel.size(); j++)
	{
		io.rel[j][i] == '1' ? is1++ : is0++;
	}
	is1 >= is0 ? getSet = '0' : getSet = '1';
	if (io.dummyCount[i - io.inputN] == io.inputN)
		is1 >= is0 ? getSet = '1' : getSet = '0';
	io.getSet.push_back(getSet);
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

void outlog(string str)
{
	time_t end = time(NULL);
	double diff = difftime(end, start);
	cout << "time:" << diff << endl
		<< str << endl;
}

void Tree::create_init_tree_one_pat(string str, char out)
{
	TreeNode *ptr = this->tree;
	this->treeSize++;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == '0')
		{
			if (ptr->zero == NULL)
			{
				TreeNode *newNode = new TreeNode;
				newNode->data = '0';
				ptr->zero = newNode;
			}
			ptr = ptr->zero;
		}
		else if (str[i] == '1')
		{
			if (ptr->one == NULL)
			{
				TreeNode *newNode = new TreeNode;
				newNode->data = '1';
				ptr->one = newNode;
			}
			ptr = ptr->one;
		}
	}
	ptr->out = out;
}
void Tree::print_tree(string str, TreeNode *root)
{
	str.push_back(root->data);
	if (root->zero == NULL && root->one == NULL)
	{
		cout << str << ":" << root->out << endl;
		return;
	}
	print_tree(str, root->zero);
	print_tree(str, root->one);
}
vector<string> Tree::test_tree_acc(vector<string> rel, vector<pair<double, int>> diff, int outbit, int inputN, double &acc)
{
	int same = 0;
	vector<string> errpat;
	for (int i = 0; i < rel.size(); i++)
	{
		string pat = "";
		char out = rel[i][inputN + outbit];
		for (int j = 0; j < diff.size(); j++)
		{
			pat.push_back(rel[i][diff[j].second]);
		}
		TreeNode *ptr = this->tree;
		for (int j = 0; j < pat.size()+1; j++) // �`�@�� inputN + 1 �� node
		{
			if (ptr->zero == NULL && ptr->one == NULL)
			{
				if (ptr->out == out)
				{
					same++;
				}
				else
				{
					if (ptr->err != true)
					{
						errpat.push_back(pat.substr(0, j)); // ����[1��
						ptr->err = true;
					}
				}
				break;
			}
			else if (pat[j] == '0') // �۫H������N���k��
			{
				ptr = ptr->zero;
			}
			else if (pat[j] == '1')
			{
				ptr = ptr->one;
			}
		}
	}
	acc = (double)same / (double)rel.size() * 100;
	this->acc = acc;
	//cout << "tree_" << outbit << "_acc: " << acc << endl;
	return errpat;
}
void Tree::modify_tree(vector<string> rel, vector<pair<double, int>> diff, int outbit, int inputN)
{
	this->treeSize += rel.size() / 2;
	for (int i = 0; i < rel.size(); i++)
	{
		string pat = "";
		char out = rel[i][inputN + outbit];
		for (int j = 0; j < diff.size(); j++)
		{
			pat.push_back(rel[i][diff[j].second]);
		}
		TreeNode *ptr = this->tree;
		for (int j = 0; j < pat.size(); j++) // �`�@�� inputN + 1 �� node
		{
			if (pat[j] == '0') // �۫H������N���k��
			{
				if (ptr->zero == NULL)
				{
					TreeNode *newNode = new TreeNode;
					newNode->data = '0';
					newNode->out = out;
					ptr->zero = newNode;
					break;
				}
				else
					ptr = ptr->zero;
			}
			else if (pat[j] == '1')
			{
				if (ptr->one == NULL)
				{
					TreeNode *newNode = new TreeNode;
					newNode->data = '1';
					newNode->out = out;
					ptr->one = newNode;
					break;
				}
				else
					ptr = ptr->one;
			}
		}
	}
}
void Tree::get_tree_getset_data(string str, TreeNode *root, vector<string> &data)
{
	char getset = this->tree->data;
	str.push_back(root->data);
	if (root->zero == NULL && root->one == NULL)
	{
		if (root->out == getset)
		{
			data.push_back(str.substr(1));
		}
		return;
	}
	get_tree_getset_data(str, root->zero, data);
	get_tree_getset_data(str, root->one, data);

}
Tree::~Tree()
{
	
}






