#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

#include "BlockingQueue.h"
#include "Task.h"

#define MAX_QUEUE_SIZE 10

using namespace std;

static int num_client_threads;
static int num_worker_threads;

BlockingQueue<Task> blocking_queue(MAX_QUEUE_SIZE, Task(true));

int getVarsNumbers(vector<string> vec) {	
	int max=0;
	
	for (int i=0; i<vec.size(); i++) {
		if (vec[i]!="1") {std::vector<std::string> tokens = split(vec[i], 'x');
		vector<int> tmp;
		for (int j=1; j<tokens.size(); j++) {
			tmp.push_back(atoi(tokens[j].c_str()));
			}
			for (int k=0; k<tmp.size(); k++){
				if (tmp[k]>max) max=tmp[k];
			}
			
		} 			
	}
	return max;
}

void call_from_client_thread(int tid, vector<vector<int>*>* results, string INPUT_DIR, string FORMAT) {
	printf("Client thread: %d. Starting...\n", tid);
	string filename = INPUT_DIR + to_string(tid+1) + FORMAT;
	
	std::ifstream input(filename);
	for( std::string line; std::getline( input, line ); ) {
		std::vector<std::string> tokens = split(line, '+');
		int n_vars = getVarsNumbers(tokens);
		int n_sets = pow(2, n_vars);
		
		vector<int>* v(new vector<int>(n_sets));
		results->push_back(v);
		for(int j=0; j<num_worker_threads; j++) {
			blocking_queue.push(Task(j, n_sets, num_worker_threads, v, tokens, n_vars));
		}
	}
	printf("Client thread: %d. Ending...\n", tid);
}

void call_from_worker_thread(int tid) {
	printf("Worker thread: %d. Starting...\n", tid);
	while (1) {
		Task task = blocking_queue.pop();
		printf("Worker thread: %d. Task received\n", tid);
		if (task.get_empty()) {
			break;
		}
		task.process();
		printf("Worker thread: %d. Task finished\n", tid);
	}
	printf("Worker thread: %d. Ending...\n", tid);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Incorrect number of arguments! Expected two integers\n";
		return -1;
	}
	
	num_client_threads = atoi(argv[1]);
	num_worker_threads = atoi(argv[2]);
	
	string INPUT_DIR = "functions/poly_";
	string OUTPUT_DIR = "results/results_";
	string FORMAT = ".txt";

	std::thread client_threads[num_client_threads];
	std::thread worker_threads[num_worker_threads];

	vector<vector<vector<int>*>*> all_results;
	
	for (int i = 0; i < num_client_threads; ++i) {
		vector<vector<int>*>* results = new vector<vector<int>*>();
	 	client_threads[i] = std::thread(call_from_client_thread, i, results, INPUT_DIR, FORMAT);
	 	all_results.push_back(results);
	}

	for (int i = 0; i < num_worker_threads; ++i) {
	 	worker_threads[i] = std::thread(call_from_worker_thread, i);
	}

	for (int i = 0; i < num_client_threads; ++i) {
		client_threads[i].join();
	}

	blocking_queue.close();

	for (int i = 0; i < num_worker_threads; ++i) {
		worker_threads[i].join();
	}
	
	printf("All tasks finished!\nWriting results to files...\n");
	for (int k=0; k<all_results.size(); k++) {
		vector<vector<int>*> results = *all_results[k];
	 	string filename = OUTPUT_DIR + to_string(k+1) + FORMAT;
	 	ofstream outfile(filename);
	 	for(int i=0; i<results.size(); i++) {
	 		vector<int> &vr = *results[i];
	 		for (int j=0; j<vr.size(); j++) {
	 			outfile << vr[j];
	 		} 
	 		outfile << endl;
		}
	}

	return 0;
}
