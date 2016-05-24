#ifndef TASK
#define TASK

#include <thread>
#include <vector>
#include <string>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::vector<int> getBooleanSetFromNumber(int n, int n_vars) {
	std::vector<int> number;
	while (n) {
		number.push_back(n & 1);
		n >>= 1;
	}
	while (number.size() < n_vars) {
		number.insert(number.begin(), 0);
	}
	return number;
}

class Task {
private:
	int thid;
	int NUMELEM;
	int THREADCNT;
	std::vector<int>* result_boolean_vec;
	std::vector<std::string> function;
	bool is_empty = false;
	int n_vars;
public:
	Task () {}

	Task(int t, int ne, int tc, std::vector<int>*& f_res, std::vector<std::string> f_init, int nv) {	
		thid = t;
		NUMELEM = ne;
		THREADCNT = tc;
		result_boolean_vec = f_res;
		function = f_init;
		n_vars = nv;
	}

	Task(bool is_e) {
		is_empty = is_e;
	}

	bool get_empty() {
		return is_empty;
	}

	int getBooleanFunctionValue(const std::vector<int>& nab) {
		int kol=0;
		
		for (int i=0; i<function.size(); i++) {
			if (function[i]!="1") {
				std::vector<std::string> tokens = split(function[i], 'x');
				std::vector<int> tmp;
				for (int j=1; j<tokens.size(); j++) {
					tmp.push_back(atoi(tokens[j].c_str()));
				}
				bool f = true;
				for (int k=0; k<tmp.size(); k++){
					if (nab[tmp[k]-1] == 0) {
						f = false;
						break;
					}
				}
				if (f)  kol++;
			} 
			else {
				kol++;
			}
		}
		if (kol%2 == 0) 
			return 0;
		else 
			return 1;
	}

	void process() {
	    std::vector<int> &vr = *result_boolean_vec;
		const unsigned numTasks = NUMELEM/THREADCNT, numTougherThreads = NUMELEM%THREADCNT;
		
	    for( unsigned int index0 = (thid < numTougherThreads ? thid * (numTasks+1) : NUMELEM - (THREADCNT - thid) * numTasks), index = index0; index < index0 + numTasks + (thid < numTougherThreads) ; ++index) {
	    	std::vector<int> boolean_set = getBooleanSetFromNumber(index, n_vars);
	    	vr[index] = getBooleanFunctionValue(boolean_set);
	    }
	}
};
#endif
