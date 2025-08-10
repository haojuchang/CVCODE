#include "structure.h"

void Tree::create_init_tree_one_pat(string str, char out)
{
	TreeNode *ptr = this->tree;
	this->treeSize++;
	for (unsigned int i = 0; i < str.size(); i++)
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
	for (unsigned int i = 0; i < rel.size(); i++)
	{
		string pat = "";
		char out = rel[i][inputN + outbit];
		for (unsigned int j = 0; j < diff.size(); j++)
		{
			pat.push_back(rel[i][diff[j].second]);
		}
		TreeNode *ptr = this->tree;
		for (unsigned int j = 0; j < pat.size()+1; j++) 
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
						errpat.push_back(pat.substr(0, j));
						ptr->err = true;
					}
				}
				break;
			}
			else if (pat[j] == '0')
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
	for (unsigned int i = 0; i < rel.size(); i++)
	{
		string pat = "";
		char out = rel[i][inputN + outbit];
		for (unsigned int j = 0; j < diff.size(); j++)
		{
			pat.push_back(rel[i][diff[j].second]);
		}
		TreeNode *ptr = this->tree;
		for (unsigned int j = 0; j < pat.size(); j++) 
		{
			if (pat[j] == '0') 
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

