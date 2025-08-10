#include "Petrick.h"
#include <omp.h>

using namespace std;

static int POS_len, PI_num;

#define ARRAY_SIZE 268435456

bool* Simplify(int* B){
	//Products after simplify, ex. 10101 (p0p2p4)
	bool* A = new bool[PI_num];
	//Initialize A
	for(int i=0; i<PI_num; i++){
		A[i] = 0;
	}
	//If number is in B, then A[number] = 1, else A[number] = 0
	for(int i=0; i<POS_len; i++){
		A[B[i]] = 1;
	}
	delete [] B;
	return A;
}

/// start of new version ///

vector<int> Petrick(vector< vector<int> > &POS, vector<int> &cost){
	
	// cout << "Start Petrick" << endl;
	
	POS_len = POS.size();
	PI_num = cost.size();
	vector<int> output;
	
	
	if(POS_len == 0)	return output; 
	
	unsigned long long int* P_size = new unsigned long long int[POS_len];
	unsigned long long int total_P_b;
	
	//count P_size (total number of P_b before each vector)
	#pragma omp parallel for
	for(int i=0; i<POS_len; i++){
		P_size[i] = 1;
	}
	
	for(int i=0; i<POS_len-1; i++){
		int tmp = POS[i].size();
		#pragma omp parallel for
		for(int j=i+1; j<POS_len; j++){
			P_size[j] = P_size[j] * (unsigned long long int)tmp;
		}
		if(tmp == 0){
			cout << "Empty vector in POS!" << endl;
			return output;
		}
	}
	total_P_b = P_size[POS_len-1] * (unsigned long long int)POS[POS_len-1].size();
	// cout << "end of count P_size" << endl;
	
	/*
	//print P_size
	cout << "P_size: ";
	for(int i=0; i<POS_len; i++){
		cout << P_size[i] << " ";
	}
	cout << endl;
	*/
	cout << "total_P_b: " << total_P_b << endl;
	
	unsigned long long int cur_start = 0;
	unsigned long long int cur_end = 0;
	int min_cost = 2147483647;
	bool* best_P_a = new bool[PI_num];
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	while(cur_start < total_P_b){
		
		cur_end = cur_start + (unsigned long long int)ARRAY_SIZE;
		
		if(cur_end > total_P_b){
			cur_end = total_P_b;
		}
		
		// cout << "cur_start = " << cur_start << endl;
		// cout << "cur_end = " << cur_end << endl;
		
		unsigned long long int cur_arr_size = cur_end - cur_start;
		
		//new a 2D array
		int** P_b = new int*[cur_arr_size];
		#pragma omp parallel for
		for(unsigned long long int i=0; i<cur_arr_size; i++){
			P_b[i] = new int[POS_len];
		}
		// cout << "end of new 2D array" << endl;
		
		//find all P_b and store to 2D array
		
		// #pragma omp parallel for
		// for(int i=0; i<POS_len; i++){
			// int group_size = P_size[i] * POS[i].size();
			// for(unsigned long long int j=0; j<total_P_b; j+=group_size){	//start point of a group
				// for(int m=0; m<POS[i].size(); m++){	//first number of a string of repeat number
					// for(int n=0; n<P_size[i]; n++){	//repeat POS[i][m] n times
						// P_b[j+m*P_size[i]+n][i] = POS[i][m];
					// }
				// }
			// }
		// }
		
		//method with a lot of division
		// #pragma omp parallel for
		// for(int i=0; i<cur_arr_size; i++){
			// unsigned long long int tmp = i + cur_start;
			// for(int j=0; j<POS_len; j++){
				// int k = (tmp / (unsigned long long int)P_size[j]) % (unsigned long long int)POS[j].size();
				// P_b[i][j] = POS[j][k];
			// }
		// }
		
		//method with large memory
		// #pragma omp parallel for
		// for(int i=0; i<POS_len; i++){
			// unsigned long long int group_size = P_size[i] * (unsigned long long int)POS[i].size();
			// for(unsigned long long int j=0; j<total_P_b; j+=group_size){	//start point of a group
				// for(int m=0; m<POS[i].size(); m++){	//first number of a string of repeat number
					// for(unsigned long long int n=0; n<P_size[i]; n++){	//repeat POS[i][m] n times
						// P_b[j+m*P_size[i]+n][i] = POS[i][m];
					// }
				// }
			// }
		// }
		
		//method with less division
		#pragma omp parallel for
		for(int i=0; i<POS_len; i++){
			unsigned long long int group_size = P_size[i] * (unsigned long long int)POS[i].size();
			unsigned long long int group_init = cur_start - cur_start % group_size;
			for(int j=0; j<POS[i].size(); j++){
				unsigned long long int init, end, pos;
				init = group_init + P_size[i] * j;
				end = init + P_size[i];
				if(end <= cur_start){
					init += group_size;
					end += group_size;
				}
				else{
					if(init < cur_start)	init = cur_start;
				}
				init -= cur_start; 
				end -= cur_start;
				while(init < cur_arr_size){
					if(end > cur_arr_size){
						end = cur_arr_size;
					}
					for(pos = init; pos < end; pos++){
						P_b[pos][i] = POS[i][j];
					}
					init += group_size;
					end += group_size;
				}
			}
		}
		
		
		// cout << "end of find all P_b to 2D array" << endl;
		
		
		//print vector of vector
		// cout << "vector of vector: " << endl << endl;
		// for(int i=0; i<POS_len; i++){
			// cout << i << ":	";
			// for(int j=0; j<POS[i].size(); j++){
				// cout << setw(3) << POS[i][j];
			// }
			// cout << endl;
		// }
		// cout << endl;
		
		//print 2D array
		// cout << "2D array: " << endl << endl;
		// for(int i=0; i<50 && i<total_P_b; i++){
			// for(int j=0; j<POS_len; j++){
				// cout << left << setw(3) << P_b[i][j] << " ";
			// }
			// cout << endl;
		// }
		// cout << endl;
		// cout << "..." << endl << endl;
		// for(int i=total_P_b - 10; i<total_P_b; i++){
			// for(int j=0; j<POS_len; j++){
				// cout << left << setw(3) << P_b[i][j] << " ";
			// }
			// cout << endl;
		// }
		// cout << endl;
		
		
		//simplify
		int* total_cost = new int[cur_arr_size];
		bool** P_a = new bool*[cur_arr_size];
		#pragma omp parallel for
		for(unsigned long long int i=0; i<cur_arr_size; i++){
			P_a[i] = Simplify(P_b[i]);
			total_cost[i] = 0;
			for(int j=0; j<PI_num; j++){
				if(P_a[i][j]){
					total_cost[i] += cost[j];
				}
			}
		}
		// cout << "end of simplify" << endl;
		
		//find best cost
		for(unsigned long long int i=0; i<cur_arr_size; i++){
			if(total_cost[i] < min_cost){
				#pragma omp parallel for
				for(int j=0; j<PI_num; j++){
					best_P_a[j] = P_a[i][j];
				}
				min_cost = total_cost[i];
			}
		}
		// cout << "end of find best cost" << endl;
		
		//update cur_start
		cur_start += ARRAY_SIZE;
		// cout << "end of update cur_start" << endl;
		
		//delete memory
		for(int i=0; i<cur_arr_size; i++){
			delete [] P_a[i];
		}
		delete [] P_b;
		delete [] P_a;
		delete [] total_cost;
		
		cout << cur_end << " / " << total_P_b << endl;
		
	}
	
	//get output
	for(int i=0; i<PI_num; i++){
		if(best_P_a[i]){
			output.push_back(i);
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	//new a 2D array
	int** P_b = new int*[total_P_b];
	#pragma omp parallel for
	for(int i=0; i<total_P_b; i++){
		P_b[i] = new int[POS_len];
	}
	// cout << "end of new 2D array" << endl;
	
	//find all P_b and store to 2D array
	
	#pragma omp parallel for
	for(int i=0; i<POS_len; i++){
		int group_size = P_size[i] * POS[i].size();
		for(unsigned long long int j=0; j<total_P_b; j+=group_size){	//start point of a group
			for(int m=0; m<POS[i].size(); m++){	//first number of a string of repeat number
				for(int n=0; n<P_size[i]; n++){	//repeat POS[i][m] n times
					P_b[j+m*P_size[i]+n][i] = POS[i][m];
				}
			}
		}
	}
	
	// cout << "end of find all P_b to 2D array" << endl;
	
	
	//print 2D array
	// cout << "2D array: " << endl << endl;
	// for(int i=0; i<50 || i<total_P_b; i++){
		// for(int j=0; j<POS_len; j++){
			// cout << left << setw(3) << P_b[i][j] << " ";
		// }
		// cout << endl;
	// }
	// cout << endl;
	// cout << "..." << endl << endl;
	// for(int i=total_P_b - 10; i<total_P_b; i++){
		// for(int j=0; j<POS_len; j++){
			// cout << left << setw(3) << P_b[i][j] << " ";
		// }
		// cout << endl;
	// }
	// cout << endl;
	
	
	//simplify
	int* total_cost = new int[total_P_b];
	bool** P_a = new bool*[total_P_b];
	#pragma omp parallel for
	for(int i=0; i<total_P_b; i++){
		P_a[i] = Simplify(P_b[i]);
		total_cost[i] = 0;
		for(int j=0; j<PI_num; j++){
			if(P_a[i][j]){
				total_cost[i] += cost[j];
			}
		}
	}
	// cout << "end of simplify" << endl;
	
	//find best cost
	int best_i = 0;
	for(int i=0; i<total_P_b; i++){
		if(total_cost[i] < min_cost){
			best_i = i;
			min_cost = total_cost[i];
		}
	}
	// cout << "end of find best cost" << endl;
	
	//get output
	for(int i=0; i<PI_num; i++){
		if(P_a[best_i][i]){
			output.push_back(i);
		}
	}
	
	*/
	//////////////////////////////////////////////////////////////////////////////////////
	
	return output;
}

/// end of new version ///

///start of old version ///

/*

struct stru{
	bool* P_a;
	int cost;
};

vector<int> Petrick(vector< vector<int> > &POS, vector<int> &cost){
	
	// cout << "Start Petrick" << endl;
	
	POS_len = POS.size();
	PI_num = cost.size();
	vector<int> output;
	
	
	if(POS_len == 0)	return output; 
	
	int* cur_index = new int[POS_len];	//To store current index of vector in POS
	int* P_b;	//Products before simplify, ex. [2, 0 ,2, 4]
	int min_cost = 2147483647;
	list<stru> task_result;
	stru best_P;
	int k;
	bool is_finish = false;
	
	
	//Initialize cur_index
	#pragma omp parallel for
	for(int i=0; i<POS_len; i++){
		cur_index[i] = 0;
	}
	
	
	//Turn POS to SOP
	
	#pragma omp parallel
	{
		#pragma omp single
		{
			// omp_set_num_threads(NUM_THREADS);
			int NUM_THREADS = omp_get_num_threads();
			// cout << "NUM_THREADS = " << NUM_THREADS << endl << endl;
			// int main_id = omp_get_thread_num();
			while(!is_finish){
				for(int kk=0; kk<NUM_THREADS; kk++){
					if(is_finish)	continue;
					// cout << "current id: " << main_id << "\tfor loop: " << kk << " in " << NUM_THREADS << endl;
					// Add new P_b
					P_b = new int[POS_len];
					for(int i=0; i<POS_len; i++){
						P_b[i] = POS[i][cur_index[i]];
					}
					#pragma omp task firstprivate(P_b)
					{
						// int id = omp_get_thread_num();
						//
						// cout << "*** task" << id << " ***" << endl;
						//	
						stru tmp_result;
						tmp_result.P_a = Simplify(P_b);
						//Calculate cost
						tmp_result.cost = 0;
						for(int i=0; i<PI_num; i++){
							if(tmp_result.P_a[i]){
								tmp_result.cost += cost[i];
							}
						}
						task_result.push_back(tmp_result);
					}
					//Update index
					// cout << "Start update index" << endl;
					++cur_index[0];
					k = 0;
					while(cur_index[k] == POS[k].size()){	//carry out
						cur_index[k] = 0;
						++k;
						if(k == POS_len)	break;
						++cur_index[k];
					}
					for(int i=0; i<POS_len; i++){
						if(cur_index[i] != 0)	break;
						if(i == POS_len-1)	is_finish = true;
					}
					// cout << "Finish update index" << endl;
				}
				#pragma omp taskwait
				//Update best_P, min_cost
				for(list<stru>::iterator it=task_result.begin(); it!=task_result.end(); it++){
					stru tmp_result = *it;
					if(tmp_result.cost < min_cost){
						// cout << "update min_cost!" << endl;
						min_cost = tmp_result.cost;
						best_P = tmp_result;
					}
					else{
						delete tmp_result.P_a;
					}
				}
				task_result.clear();
			}
		}
	}
	
	
	//get output
	#pragma omp parallel for
	for(int i=0; i<PI_num; i++){
		if(best_P.P_a[i]){
			#pragma omp critical
			output.push_back(i);
		}
	}
	
	return output;
}

*/

/// end of old version ///