#include "CircuitModeling.h"

bool MODEL::check(vector<string> &inpat, vector<string> &qmpat){
	cout << "\n===== start check =====" << endl;
	vector<bool> cover;
	cover.resize(inpat.size(), false);

	omp_lock_t lck; 
	omp_init_lock(&lck);
	#pragma omp parallel for
	for(unsigned int i = 0; i < qmpat.size(); i++){
		for(unsigned int j = 0; j < inpat.size(); j++){
			if(cover[j]) 
				continue;
			else{
				bool c = iscover(inpat[j], qmpat[i]);
				if(c){
					omp_set_lock(&lck);
					cover[j] = c;
					omp_unset_lock(&lck);
				}
			}
		}
	}	
	omp_destroy_lock(&lck);

	for(unsigned int i = 0; i < cover.size(); i++){
		if(!cover[i])
			return false;
	}
	return true;
}

bool MODEL::iscover(string &in, string &qm){
	int size = 0;
	for(int i = 0; i < size; i++){
		if(qm[i] == '-') 
			continue;
		else if(in[i] != qm[i]) 
			return false;
	}
	return true;
}

void MODEL::prepareQM(int inputN, vector<string> &pat, vector<pair<double,int>> &diff){
	vector<string> prepare;
	prepare.reserve(pat.size());
	for(unsigned int i = 0; i < pat.size(); i++){
		string p(inputN, '-');
		for(unsigned int j = 0; j < pat[i].size(); j++){
			p[diff[j].second] = pat[i][j];
		}
		prepare.push_back(p);
	}
	pat = prepare;
}



void MODEL::creat_randomPat(int bitCount, vector<string> &pat, int inputN)
{
	
	if (inputN < bitCount)
		bitCount = inputN;
		
	int patCounts = pow(2, bitCount);
	// int t;
	string ts = "";
	pat.clear();
	for (int i = 0; i < patCounts; i++)
	{
		ts = creatOnePat(inputN);
		pat.push_back(ts);
		ts = "";
	}

	string all0(inputN, '0');
	string all1(inputN, '1');
	pat.push_back(all0);
	pat.push_back(all1);

	for (int i = 0; i < inputN; i++) {
		string one1(inputN, '0');
		one1[i] = '1';
		string one0(inputN, '1');
		one0[i] = '0';
		pat.push_back(one1);
		pat.push_back(one0);
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
			pat.push_back(two1);
			pat.push_back(two0);
		}
	}
}

string MODEL::creatOnePat(int bit)
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

void MODEL::put_iogen(string iogen, string in_pat, string io_rel)
{

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

void MODEL::read_io_rel(string io_rel, vector<string> &rel)
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
	
	for (unsigned int i = 2; i < file.size(); i++)
	{
		string r = "";
		for (unsigned int j = 0; j < file[i].size(); j += 2)
		{
			r.push_back(file[i][j]);
		}
		rel.push_back(r);
	}
	inputFile.close();
}


void MODEL::gen_io_rel(string iogen, string relDir, string patDir, vector<string> &pat, vector<string> &rel, int inputN, vector<string> IOvariable)
{
	string rm = "rm " + patDir;
	string rm1 = "rm " + relDir;

	unsigned two19 = pow(2, 19);
	
	rel.reserve(pat.size());

	for (unsigned s = 0, end = two19; s < pat.size(); s += two19, end += two19)
	{
		ofstream fout(patDir);
		if (!fout)
		{
			cerr << "File could not be write at gen_io_rel()" << endl;
			exit(1);
		}

		// first line
		fout << inputN << " " << ((end > pat.size()) ? pat.size() - s : two19) << "\n";

		// second line
		fout << IOvariable[0];
		for (int i = 1; i < inputN; i++)
		{
			fout << " " << IOvariable[i];
		}
		fout << "\n"; // the last one

		// input pattern
		for (unsigned i = s; i < end && i < pat.size(); i++)
		{
			fout << pat[i][0];
			for (unsigned int j = 1; j < pat[i].size(); j++)
			{
				fout << " " << pat[i][j];
			}
			fout << "\n";
		}
		fout.close();

		put_iogen(iogen, patDir, relDir);
		read_io_rel(relDir, rel);
		system(rm.c_str());
		system(rm1.c_str());
	}
}

void MODEL::testDiff(int inputN, int outputN, vector<string> IOvariable, vector<vector<pair<double,int>>> &iodiff, vector<int> &iodummyCount)
{
	vector<vector<pair<double,int>>> diff(outputN);

	// omp_set_num_threads(4);
	omp_lock_t lck; 
	omp_init_lock(&lck);

	#pragma omp parallel for
	for (int bit = 0; bit < inputN; bit++)
	{
		vector<string> pat;
		creat_randomPat(10, pat, inputN);
		string all0(inputN, '0');
		string all1(inputN, '1');
		pat.push_back(all0);
		pat.push_back(all1);

		for (int i = 0; i < inputN; i++) {
			string one1(inputN, '0');
			one1[i] = '1';
			string one0(inputN, '1');
			one0[i] = '0';
			pat.push_back(one1);
			pat.push_back(one0);
		}
		int size = pat.size();

		for (int i = 0; i < size; i++)
		{
			pat[i][bit] = '0';
			string t = pat[i];
			t[bit] = '1';
			pat.push_back(t);
		}
		
		vector<string> rel;
		string relDir = IO_REL + to_string(bit) + ".txt";
		string patDir = IN_PAT + to_string(bit) + ".txt";
		string iogen = IOGEN;
		gen_io_rel(iogen, relDir, patDir, pat, rel, inputN, IOvariable);
		
		for (int outbit = inputN; outbit < inputN + outputN; outbit++)
		{
			int same = 0;
			//int out = 0;
			for (int i = 0; i < size; i++)
			{
				if (rel[i][outbit] == rel[i + size][outbit])
					same++;
			}
			pair<double, int> d;
			d.first = (double)same / (double)size;
			d.second = bit;	
			omp_set_lock(&lck);
			diff[outbit - inputN].push_back(d);
			omp_unset_lock(&lck);
		}
		// cout << "testDiff: " + to_string(bit) << endl;
	}
	omp_destroy_lock(&lck);
	iodiff = diff;
	

	for (unsigned int i = 0; i < iodiff.size(); i++)
	{
		int dummyCount = 0;
		for (unsigned int j = 0; j < iodiff[i].size(); j++)
		{
			if (iodiff[i][j].first >= 1)
			{
				dummyCount++;
			}
		}
		iodummyCount.push_back(dummyCount);
	}
	ofstream dummySave(FOLDER_POS + "diffSave.txt");
	ofstream fout(FOLDER_POS + "diff.csv");
	for (unsigned int i = 0; i < iodiff.size(); i++)
	{
		fout << "outbit:" << i << "," << endl;
		int count = 0;
		for (unsigned int j = 0; j < iodiff[i].size(); j++)
		{
			if (iodiff[i][j].first >= 1)
			{
				count++;
			}
			dummySave << (int)iodiff[i][j].first << " ";
			fout << iodiff[i][j].first << ",";
		}
		fout << endl;
		dummySave << "\noutput:" << i << " diff:" << count << " realInput:" << inputN - count << " name:" << IOvariable[inputN + i] << endl;
		dummySave << endl;
	}
	dummySave.close();
	fout.close();
}