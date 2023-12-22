#include "fine_set.h"
#include "tester.h"

#include <iostream>
#include <fstream>
#include <iterator>

struct CfgParams {
	int nThreadsWriters = -1;
	int sizeWriters = -1;
	int nThreadsReaders = -1;
	int sizeReaders = -1;
	int numOfTests = -1;
};

bool ReadConfig(std::string path, CfgParams& params) {
	std::ifstream fin(path);
	
	fin >> params.nThreadsWriters >> params.sizeWriters;
	fin >> params.nThreadsReaders >> params.sizeReaders;
	fin >> params.numOfTests;

	fin.close();
	if (std::any_of(&params.nThreadsReaders, &params.numOfTests + 1, [](int field){ return field < 0; }))
		return false;
	std::cout << "Count of writers: " << params.nThreadsWriters << ", count of writers size " << params.sizeWriters << std::endl;
	std::cout << "Count of readers: "  << params.nThreadsReaders << ", count of readers size " << params.sizeReaders << std::endl;
	std::cout << "Count of tests: " << params.numOfTests << std::endl;
	return true;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "Enter path to config file as argument\n";
		return 1;
	}

	CfgParams params;
	if (!ReadConfig(argv[1], params)) {
		std::cout << "Incorrect arguments in config file. The correct structure:" << std::endl;
		std::cout << "<num of writers> <count of writers size>" << std::endl;
		std::cout << "<num of readers> <count of readers size>" << std::endl;
		std::cout << "<num of tests>" << std::endl;
		return 1;
	}

	std::cout << "\nFine-Grained Set:\n";
	Tester::WritersFuncTest(params.nThreadsWriters, params.sizeWriters);
	Tester::ReadersFuncTest(params.nThreadsReaders, params.sizeReaders);
	Tester::GeneralFuncTest(params.sizeReaders, params.sizeWriters);
	// Tester::WritersPerfTest(params.nThreadsWriters, params.sizeWriters, params.numOfTests);
	// Tester::ReadersPerfTest(params.nThreadsWriters, params.sizeReaders, params.numOfTests);
	// Tester::GeneralPerfTest(params.sizeReaders, params.sizeWriters, params.numOfTests);
	
	return 0;
}