#include <iostream>
#include <vector>
#include <string>
using namespace std;

typedef vector<string> ONES;
typedef vector<ONES> ONESTABLE;

// struct Quine {
// 	string pat;
// 	bool cover = false;
// 	Quine *next = NULL;
// };
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
	void initModeling(int inputN, int outputN, vector<int> &dummyCount, vector<char> &iogetSet, vector<string> &IOvariable, vector<vector<pair<double,int>>> &iodiff, vector<Tree> &tree_s);

};

struct IO
{
	int inputN;
	int outputN;
	vector<string> IOvariable; // A0 A1 .. B0 B1 .. O1 O2 ..
	vector<string> pat;
	vector<string> rel;
	vector<vector<pair<double,int>>> diff;
	vector<int> dummyCount;
	vector<char> getSet;
};