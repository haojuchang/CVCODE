#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <chrono>
#include "structure.h"
#include "../src/quine_mccluskey.h"
#include "../src/Petrick.h"
#include "../src/CircuitModeling.h"
using namespace std;

string IO_INFO;
string IOGEN;
string CIRCUIT;
string TEMPCIRCUIT;
string IN_PAT;
string IO_REL;
string FOLDER_POS;
int superInput = 10;

clock_t _START, _END;
double _CPUTIME;

void read_io_info(IO &io, string io_info);

string creatOnePat(int bit);
void gen_io_rel(IO &io, string in_pat);
void initModeling(IO &io, vector<Tree> &tree_s);
bool modifyModeling(IO &io, vector<Tree> &tree_s);
bool cp_pair_db_int_fr(pair<double, int> i, pair<double, int> j);
bool cp_pair_db_int_sc(pair<double, int> i, pair<double, int> j);
void creat_init_pat(int outbit, int inputN, vector<string> &iopat, vector<vector<pair<double,int>>> &iodiff);
void deterGetSet(vector<char> &iogetSet, vector<string> &rel, int outbit, vector<int> &dummyCount, int inputN);
vector<string> getToken(string str, string split);
void outlog(string str);
ONESTABLE genQMinput(Tree &tree, int inputN);
void minimize_boolean_function( vector<string>& input_min_terms, vector<string>& output_prime_implicants );
void set_input_minterms( vector<string>& input_minterms );
void print_implicants( const vector<string>& implicants );
void print_minterms( const vector<string>& minterms );

static double input_setup_duration = 0;
static double quine_mccluskey_duration = 0;
static double petricks_duration = 0;

int main(int argc, char **argv){

	// vector<string> input_minterms, output_prime_implicants;
	// set_input_minterms( input_minterms );
	// minimize_boolean_function( input_minterms, output_prime_implicants );
	// return 0;

	srand( 123 );
	auto start_time = std::chrono::high_resolution_clock::now();
	_START = clock();

	int max_thread = atoi( argv[4] );
	if ( max_thread < 0 ) { max_thread = 4; }
	if ( max_thread > omp_get_max_threads() ) { max_thread = omp_get_max_threads(); }
	int input_size_as_2s_exponent = atoi( argv[5] );

	auto input_setup_start_time = std::chrono::high_resolution_clock::now();
	omp_set_num_threads( max_thread );
	superInput = input_size_as_2s_exponent;

	IO_INFO = string(argv[1]);
	IOGEN = string(argv[2]);
	CIRCUIT = string(argv[3]);
	int found = CIRCUIT.find_last_of("/\\");
	if (found != -1)
		FOLDER_POS = CIRCUIT.substr(0, found + 1);
	else
		FOLDER_POS = "./";

	TEMPCIRCUIT = FOLDER_POS + "TEMP.v";
	IN_PAT = FOLDER_POS + "in_pat";
	IO_REL = FOLDER_POS + "io_rel";
	IO io;
	read_io_info(io, IO_INFO);

	if (io.inputN < superInput)
	{
		superInput = io.inputN;
	}

	cout << "\ninputN: " << io.inputN << " outputN: " << io.outputN << endl;
	cout << "superInput: " << superInput << endl;

	MODEL model;
	model._START = _START;
	model.IO_REL = IO_REL;
	model.IN_PAT = IN_PAT;
	model.IOGEN = IOGEN;
	model.FOLDER_POS = FOLDER_POS;
	model.testDiff(io.inputN, io.outputN, io.IOvariable, io.diff, io.dummyCount);
	outlog("\ntestDiff done\n");

	vector<Tree> tree_s;
	initModeling(io, tree_s);
	outlog("\ninitModeling done \n");
	// return 0;
	// while(!modifyModeling(io, tree_s)){};
	// cout << "end modeling\n\n" << endl;
	input_setup_duration = std::chrono::duration_cast<std::chrono::duration<double>>( std::chrono::high_resolution_clock::now() - input_setup_start_time ).count();

	for(int i = 0; i < io.outputN; i++){
		vector<string> input_min_terms, output_prime_implicants;
		tree_s[i].get_tree_getset_data("", tree_s[i].tree, input_min_terms);

		model.prepareQM(io.inputN, input_min_terms, io.diff[i]);
		// for(unsigned int j = 0; j < input_min_terms.size(); j++)
		// 	cout << input_min_terms[j] << endl;

		// cout << "output: " << i << " getSet: " << io.getSet[i] <<  " terms: " << input_min_terms.size();
		if(input_min_terms.size() != 0){
			minimize_boolean_function( input_min_terms, output_prime_implicants );
			// cout << " opt: " << output_prime_implicants.size();
		}

		bool same = model.check(input_min_terms, output_prime_implicants);
		cout << "equal: " << same;

		cout << endl;
		
	}
	auto end_time = std::chrono::high_resolution_clock::now();
	auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>( end_time - start_time );
	std::cout << "     Input setup time = " << input_setup_duration << std::endl;
	std::cout << " Quine McCluskey time = " << quine_mccluskey_duration << std::endl;
	std::cout << "Petrick's method time = " << petricks_duration << std::endl;
	std::cout << "           Total time = " << time_span.count() << " seconds." << std::endl;
	return 0;
}

void minimize_boolean_function(vector<string>& input_implicants, vector<string>& output_prime_implicants )
{
	// print_minterms( input_implicants );

    // Quine McCluskey
	auto quine_mccluskey_start_time = std::chrono::high_resolution_clock::now();
    QuineMcCluskey qm;
    qm.run( input_implicants );
    vector<vector<int>> petricks_initial_function;
    vector<int> costs;
    qm.get_petricks_initial_function( petricks_initial_function, costs );
	double duration = std::chrono::duration_cast<std::chrono::duration<double>>( std::chrono::high_resolution_clock::now() - quine_mccluskey_start_time ).count();
	std::cout << "Quine McCluskey done, duration = " << duration << std::endl;
	quine_mccluskey_duration += duration;

    // Petrick's method
	auto petricks_start_time = std::chrono::high_resolution_clock::now();
    vector<int> selected_prime_implicant_ids = Petrick( petricks_initial_function, costs );
	duration = std::chrono::duration_cast<std::chrono::duration<double>>( std::chrono::high_resolution_clock::now() - petricks_start_time ).count();
	std::cout << "Petrick's done, duration = " << duration  << std::endl;
	petricks_duration += duration;
	qm.get_prime_implicant_strings( selected_prime_implicant_ids, output_prime_implicants );

	// print_implicants( output_prime_implicants );
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

	for(unsigned int i = 0; i < tok.size(); i++){
		io.IOvariable.push_back(tok[i]);
	}

	inputFile.close();
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
	MODEL model;
	vector<string> rel;
	//remove(in_pat.c_str());
	string rm = "rm " + in_pat;

	unsigned two19 = pow(2, 19);
	
	rel.reserve(io.pat.size());

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
			for (unsigned int j = 1; j < io.pat[i].size(); j++)
			{
				fout << " " << io.pat[i][j];
			}
			fout << "\n";
		}
		fout.close();

		model.put_iogen(IOGEN, IN_PAT, IO_REL);
		model.read_io_rel(IO_REL, rel);
		io.rel = rel;
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
	int inputN = io.inputN;
	int outputN = io.outputN;
	vector<char> getSet;
	vector<int> dummyCount;
	vector<string> IOvariable = io.IOvariable;
	vector<vector<pair<double,int>>> iodiff = io.diff;
	unsigned beforMerge = 0;
	
	omp_lock_t lck; 
	omp_init_lock(&lck);
	//vector<string> temptemp = io.pat;
	#pragma omp parallel for
	for (int i = 0; i < outputN; i++)
	{
		cout << "initTree: " << i << endl;
		vector<pair<double, int>> diff = iodiff[i];
		sort(diff.begin(), diff.end(), cp_pair_db_int_fr);
		vector<pair<double, int>> diffSave, diffNoNeed, diffSaveTemp;

		int MaxBit = (inputN >= superInput) ? superInput : inputN;
		for (int d = 0; d < inputN; d++) {
			if (diff[d].first < 1 && (int) diffSave.size() < MaxBit) {
				diffSave.push_back(diff[d]);
			}
			else
			{
				diffNoNeed.push_back(diff[d]);
			}
		}
		diffSaveTemp = diffSave; 
		sort(diffSave.begin(), diffSave.end(), cp_pair_db_int_sc); 

		vector<string> pat;
		creat_init_pat(i, inputN, pat, iodiff);
		beforMerge += pat.size();

		//bool isRan = false;

		vector<string> rel;
		string relDir = IO_REL + to_string(i) + ".txt";
		string patDir = IN_PAT + to_string(i) + ".txt";
		string iogen = IOGEN;
		MODEL model;
		model.gen_io_rel(iogen, relDir, patDir, pat, rel, inputN, io.IOvariable);

		// io.rel = rel;
		// gen_io_rel(io, IN_PAT);
		deterGetSet(getSet, rel, i, io.dummyCount, inputN);
		// deterGetSet(io, i);
		/*0903*/
		Tree tree;
		tree.tree->data = getSet[i]; 
		for (unsigned int j = 0; j < rel.size(); j++)
		{
			string str = "";
			for (unsigned int b = 0; b < diffSaveTemp.size(); b++)
				str.push_back(rel[j][diffSaveTemp[b].second]);
			char out = rel[j][inputN + i];
			tree.create_init_tree_one_pat(str, out);
		}
		
		int patCounts = pow(2, 17);
		pat.clear();
		rel.clear();
		for (int j = 0; j < patCounts; j++)
		{
			string ts = creatOnePat(inputN);
			pat.push_back(ts);
		}
		model.gen_io_rel(iogen, relDir, patDir, pat, rel, inputN, IOvariable);
		// gen_io_rel(io, IN_PAT);
		vector<string>errpat;
		double acc = 0.0;
		errpat = tree.test_tree_acc(rel, diff, i, inputN, acc);

		omp_set_lock(&lck);
		tree_s.push_back(tree);
		omp_unset_lock(&lck);
	}
	omp_destroy_lock(&lck);
	io.getSet = getSet;

	cout << "\nInit acc of each primary output" << endl;
	for(unsigned int i = 0; i < tree_s.size(); i++){
		cout << "PO: " << i << " Acc: " << tree_s[i].acc << endl;
	}
}

bool modifyModeling(IO &io, vector<Tree> &tree_s)
{
	int inputN = io.inputN;
	int outputN = io.outputN;
	// int count = 0;
	string blifDir = FOLDER_POS + "circuit.blif";
	
	bool satisfy = true;
	for (int i = 0; i < outputN; i++){
		if (tree_s[i].acc < 99.99)
			satisfy = false;
	}
	if(satisfy)
		return true;

	int minOut = 0;
	for (int i = 0; i < outputN; i++)
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
		string ts = creatOnePat(inputN);
		io.pat.push_back(ts);
	}
	gen_io_rel(io, IN_PAT);
	vector<string>errpat;
	double acc = 0.0;
	errpat = tree_s[minOut].test_tree_acc(io.rel, diff, minOut, inputN, acc);

	cout << "modify PO: " << minOut << " acc: " << acc << endl;
	if (errpat.size() != 0)
	{
		vector<string> copy = errpat;
		for (unsigned int j = 0; j < errpat.size(); j++) {
			errpat[j].push_back('0');
			copy[j].push_back('1');
		}
		errpat.insert(errpat.end(), copy.begin(), copy.end());

		for (unsigned int j = 0; j < errpat.size(); j++)
		{
			for (unsigned int b = 0; b < diff.size() - errpat[j].size(); b++)
			{
				int t = rand() % 2;
				errpat[j].push_back(char(48 + t));
			}
		}

		io.pat.clear();
		for (unsigned int j = 0; j < errpat.size(); j++)
		{
			string p(diff.size(), '0');
			for (unsigned int b = 0; b < errpat[j].size(); b++)
			{
				p[diff[b].second] = errpat[j][b];
			}
			io.pat.push_back(p);
		}
		gen_io_rel(io, IN_PAT);
		tree_s[minOut].modify_tree(io.rel, diff, minOut, inputN);
	}	
	
	satisfy = true;
	for (int i = 0; i < outputN; i++){
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


	for(unsigned int i = 0; i < data.size(); i++){
		int _1 = 0;
		for(unsigned int j = 0; j < data[i].size(); j++){
			if(data[i][j] == '1') _1++;
		}
		onestable[_1].push_back(data[i]);
	}

	return onestable;
}

// gray code 
void creat_init_pat(int outbit, int inputN, vector<string> &iopat, vector<vector<pair<double,int>>> &iodiff)
{
	vector<pair<double, int>> diff = iodiff[outbit];
	sort(diff.begin(), diff.end(), cp_pair_db_int_fr);
	vector<pair<double, int>> diffSave, diffNoNeed;
	int MaxBit = (inputN >= superInput) ? superInput : inputN;

	for (int d = 0; d < inputN; d++) {
		if (diff[d].first < 1 && (int) diffSave.size() < MaxBit) {
			diffSave.push_back(diff[d]);
		}
		else
		{
			diffNoNeed.push_back(diff[d]);
		}
	}
	int bitCount = diffSave.size();
	if (bitCount == 0) {
		iopat.clear();
		string p(inputN, '0');
		//for (int vote = 0; vote < voteInitPat; vote++)
		iopat.push_back(p);
	}
	else {
		vector<string> pat;
		pat.push_back("0");
		pat.push_back("1");

		for (int i = 0; i < bitCount - 1; i++) {
			vector<string> copy = pat;
			for (unsigned int j = 0; j < pat.size(); j++) {
				pat[j] = "0" + pat[j];
				copy[j] = "1" + copy[j];
			}
			pat.insert(pat.end(), copy.begin(), copy.end());
		}

		sort(diffNoNeed.begin(), diffNoNeed.end(), cp_pair_db_int_sc);
		iopat.clear();
		for (unsigned int i = 0; i < pat.size(); i++)
		{
			string p = pat[i];
			for (unsigned int j = 0; j < diffNoNeed.size(); j++)
			{
				int t = rand() % 2;
				p.insert(p.begin() + diffNoNeed[j].second, char(48 + t));
			}
			iopat.push_back(p);
		}

		/*io.pat.clear();
		io.pat = pat;*/
		string all0(inputN, '0');
		string all1(inputN, '1');
		
		iopat.push_back(all0);
		iopat.push_back(all1);

		for (int i = 0; i < inputN; i++) {
			string one1(inputN, '0');
			one1[i] = '1';
			string one0(inputN, '1');
			one0[i] = '0';
			iopat.push_back(one1);
			iopat.push_back(one0);
		}

		for (int i = 0; i < inputN - 1; i++)
		{
			for (int j = i + 1; j < inputN; j++)
			{
				string two1(inputN, '0');
				two1[i] = '1';
				two1[j] = '1';
				string two0(inputN, '1');
				two0[i] = '0';
				two0[j] = '0';
				iopat.push_back(two1);
				iopat.push_back(two0);
			}
		}
	}
}

void deterGetSet(vector<char> &iogetSet, vector<string> &rel, int outbit, vector<int> &dummyCount, int inputN)
{
	char getSet;
	int is0 = 0;
	int is1 = 0;
	int i = inputN + outbit;
	for (unsigned int j = 0; j < rel.size(); j++)
	{
		rel[j][i] == '1' ? is1++ : is0++;
	}
	is1 >= is0 ? getSet = '0' : getSet = '1';
	if (dummyCount[i - inputN] == inputN)
		is1 >= is0 ? getSet = '1' : getSet = '0';
	iogetSet.push_back(getSet);
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
	_END = clock();
    _CPUTIME = ((double) (_END - _START)) / CLOCKS_PER_SEC;
	cout << "time:" << _CPUTIME << endl
		<< str << endl;
}

void set_input_minterms( vector<string>& input_minterms )
{
	input_minterms.resize( 0 );
	input_minterms.push_back( "0000" );
	input_minterms.push_back( "0001" );
	input_minterms.push_back( "0010" );
	input_minterms.push_back( "1000" );
	input_minterms.push_back( "0101" );
	input_minterms.push_back( "0110" );
	input_minterms.push_back( "1001" );
	input_minterms.push_back( "1010" );
	input_minterms.push_back( "0111" );
	input_minterms.push_back( "1110" );
}

void print_minterms( const vector<string>& minterms )
{
	std::cout << "Input minterms : " << std::endl;
	for ( int i = 0; i < static_cast<int>( minterms.size() ); ++i )
	{
		std::cout << "    " << minterms[i] << std::endl;
	}
}

void print_implicants( const vector<string>& implicants )
{
	std::cout << "Minimum sum of products : " << std::endl;
	for ( int i = 0; i < static_cast<int>( implicants.size() ); ++i )
	{
		std::cout << "    " << implicants[i] << std::endl;
	}
}