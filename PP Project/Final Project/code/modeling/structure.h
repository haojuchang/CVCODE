#include <iostream>
#include <vector>
#include <string>
using namespace std;

/*#define  innode 3  
#define  hiddennode 10
#define  outnode 1 
#define  sample 7*/

typedef vector<string> ONES;
typedef vector<ONES> ONESTABLE;

struct Quine {
	string pat;
	bool cover = false;
	Quine *next = NULL;
};
struct TreeNode {
	char out = '-';
	char data = '-';
	bool err = false;
	TreeNode *one = NULL;
	TreeNode *zero = NULL;
};

class Tree
{
public:
	Tree()
	{
		tree = new TreeNode;
	}
	~Tree();
	unsigned treeSize = 0;
	double acc = 0.0;
	TreeNode *tree;
	void create_init_tree_one_pat(string str, char out);
	void print_tree(string str, TreeNode *root);
	vector<string> test_tree_acc(vector<string> rel, vector<pair<double, int>> diff, int outbit, int inputN, double &acc);
	void modify_tree(vector<string> rel, vector<pair<double, int>> diff, int outbit, int inputN);
	void get_tree_getset_data(string str, TreeNode *root, vector<string> &data);
};
/*struct Dummy{
	int bit;
	double acc;
};*/

struct IO
{
	int inputN;
	int outputN;
	vector<string> IOvariable; // A0 A1 .. B0 B1 .. O1 O2 ..
	vector<string> pat;
	vector<string> rel;
	//vector<Dummy> dummy;
	vector<vector<pair<double,int>>> diff;
	vector<int> dummyCount;
	vector<char> getSet;
};

//class bpnet
//{
//public:
//	int innode, hiddennode, outnode, sample;
//	double **w1;	//[hiddennode][innode];
//	double **w2;	//[outnode][hiddennode];
//	double *b1;	//[hiddennode];
//	double *b2;	//[outnode];
//	double **X;
//	double **Y;
//	double e;
//	double error;
//	double lr;
//	bpnet(int inputn, int outputn, int h, int s);
//	~bpnet();
//	void init();
//	double randval(double low, double high);
//	void initw(double **w, int row, int col);
//	void initb(double *w, int n);
//	void train(double **p, double **t);
//	double sigmod(double y);
//	double dsigmod(double y);
//	string predict(double p[]);
//};