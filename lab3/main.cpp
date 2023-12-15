#include "fine_set.h"
#include "tester.h"

#include <iostream>
#include <fstream>

struct CfgParams {
	int nThreadsWriters;
	int sizeWriters;
	int nThreadsReaders;
	int sizeReaders;
	int numOfTests;
};

void ReadConfig(std::string path, CfgParams& params) {
	std::ifstream fin(path);
	
	fin >> params.nThreadsWriters >> params.sizeWriters;
	fin >> params.nThreadsReaders >> params.sizeReaders;
	fin >> params.numOfTests;

	fin.close();
	std::cout << "Count of writers: " << params.nThreadsWriters << ", count of writers size " << params.sizeWriters << std::endl;
	std::cout << "Count of readers: "  << params.nThreadsReaders << ", count of readers size " << params.sizeReaders << std::endl;
	std::cout << "Count of tests: " << params.numOfTests << std::endl;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "Enter path to config file as argument\n";
		return 1;
	}

	CfgParams params;
	ReadConfig(argv[1], params);

	std::cout << "\nFine-Grained Set:\n";
	Tester::WritersFuncTest(params.nThreadsWriters, params.sizeWriters);
	Tester::ReadersFuncTest(params.nThreadsReaders, params.sizeReaders);
	Tester::GeneralFuncTest(params.sizeReaders, params.sizeWriters);
	Tester::WritersPerfTest(params.nThreadsWriters, params.sizeWriters, params.numOfTests);
	Tester::ReadersPerfTest(params.nThreadsWriters, params.sizeReaders, params.numOfTests);
	Tester::GeneralPerfTest(params.sizeReaders, params.sizeWriters, params.numOfTests);
	
	return 0;
}