#pragma once

#include "fine_set.h"

#include <vector>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <algorithm>

class Tester {
private:
  static pthread_cond_t cv;
  static bool start;
  static const int timeout_sec = 7;

  struct write_args_t {
    FineSet<int>* set;
    int begin;
    int end;
    const std::vector<int>* vals;
	std::vector<int>* checkArray;

    write_args_t(FineSet<int>* set, int begin, int end, const std::vector<int>* vals, std::vector<int>* checkArray) :
      set(set), begin(begin), end(end), vals(vals), checkArray(checkArray) {}
  };

  struct read_args_t {
    FineSet<int>* set;
    int threadNum;
    int step;
    std::vector<int>* checkArray;

    read_args_t(FineSet<int>* set, int threadNum, int step, std::vector<int>* checkArray) :
      set(set), threadNum(threadNum), step(step), checkArray(checkArray) {}
  };

  static int Random(int minNum, int maxNum) {
    return (int)(rand() % (maxNum - minNum + 1) + minNum);
  }

  static std::vector<int> GenVals(int size, int seed) {
    constexpr int MIN = 0;
    const int MAX = 3 * size;

    std::vector<int> res;
    res.reserve(size);
    std::srand(seed);
    for (int i = 0; i < size; ++i)
      res.push_back(Random(MIN, MAX));

    return res;
  }

  static void Synchronize() {
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&mutex);
    if (!start)
      pthread_cond_wait(&cv, &mutex);
    else
      pthread_cond_broadcast(&cv);
    pthread_mutex_unlock(&mutex);
  }

  static void Start() {
    start = true;
    pthread_cond_broadcast(&cv);
  }

  static void* _write(void* inputArgs) {
    write_args_t* args = static_cast<write_args_t*>(inputArgs);
    Synchronize();
    for (int i = args->begin; i < args->end; ++i) {
		//debug std::cout << "i = " << i << ", v[i] = " << (*(args->vals))[i] << std::endl;
    	args->set->Add((*(args->vals))[i]);
		if (args->set->Contains((*(args->vals))[i])) {
			(*args->checkArray)[i] = 1;
		}
		// if (args->set->Add((*(args->vals))[i])) {
		// 	std::cout << "added " << (*(args->vals))[i] << std::endl;
		// 	(*args->checkArray)[i] = 1;
		// }
	}

    return (void*)nullptr;
  }

  static void* _read(void* inputArgs) {
    read_args_t* args = static_cast<read_args_t*>(inputArgs);

    Synchronize();
    int size = int(args->checkArray->size());
    for (int i = 0; i * args->step + args->threadNum < size; ++i) {
    	if (args->set->Contains(i * args->step + args->threadNum)) {
    		(*args->checkArray)[i * args->step + args->threadNum] = 1;
    		args->set->Remove(i * args->step + args->threadNum);
      }
    }

    return (void*)nullptr;
  }

  static void PrepareWritersThreads(FineSet<int>& set, const std::vector<int>& vals, 
    std::vector<pthread_t>& threads, std::vector<write_args_t>& args, std::vector<int>& checkArr) {
    int n_writers = (int)threads.size();
    args.reserve(n_writers);

    int begin = 0;
    int size = vals.size();
    int step = size / n_writers;
    int end = step;
    // std::cout << "\nbegin: " << begin << " step: " << step << " end: " << end << std::endl;
    start = false;
    for (int i = 0; i < n_writers; ++i) {
      if (i == n_writers - 1)
        end = size;
      //debug std::cout << "\nbegin: " << begin << " step: " << step << " end: " << end << std::endl;
      args.emplace_back(&set, begin, end, &vals, &checkArr);
      pthread_create(&threads[i], nullptr, _write, (void*)&args.back());
      begin += step;
      end += step;
    }
  }

  static long long WritersTest(int n_writers, FineSet<int>& set, const std::vector<int>& vals, std::vector<int>& checkArr) {
    std::vector<pthread_t> threads(n_writers);
    std::vector<write_args_t> args;
    PrepareWritersThreads(set, vals, threads, args, checkArr);

    auto t1 = std::chrono::high_resolution_clock::now();
    Start();
    for (int i = 0; i < n_writers; ++i) 
      pthread_join(threads[i], nullptr);
    
    auto t2 = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
  }

  static void PrepareReadersThreads(FineSet<int>& set, std::vector<int>& checkArr,
    std::vector<pthread_t>& threads, std::vector<read_args_t>& args) {
    int n_readers = (int)threads.size();
    args.reserve(n_readers);

    start = false;
    for (int i = 0; i < n_readers; ++i) {
      args.emplace_back(&set, i, n_readers, &checkArr);
      pthread_create(&threads[i], nullptr, _read, (void*)&args.back());
    }
  }

  static long long ReadersTest(int n_readers, FineSet<int>& set, std::vector<int>& checkArr) {
    std::vector<pthread_t> threads(n_readers);
    std::vector<read_args_t> args;

    PrepareReadersThreads(set, checkArr, threads, args);

    auto t1 = std::chrono::high_resolution_clock::now();
    Start();
    for (int i = 0; i < n_readers; ++i)
      pthread_join(threads[i], nullptr);
    auto t2 = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
  }

  static long long GeneralTest(int n_readers, int n_writers, FineSet<int>& set, 
    const std::vector<int>& vals, std::vector<int>& readCheckArr, std::vector<int>& writeCheckArr) {
    std::vector<pthread_t> readers(n_readers);
    std::vector<read_args_t> readersArgs;
    std::vector<pthread_t> writers(n_writers);
    std::vector<write_args_t> writersArgs;

    PrepareReadersThreads(set, readCheckArr, readers, readersArgs);
    PrepareWritersThreads(set, vals, writers, writersArgs, writeCheckArr);

    auto t1 = std::chrono::high_resolution_clock::now();
    // struct timespec ts;
    // int milliseconds = 1000;
    // ts.tv_sec = milliseconds / 1000;
    // ts.tv_nsec = (milliseconds % 1000) * 1000000;
    Start();
    // nanosleep(&ts, nullptr);
    for (int i = 0; i < n_writers; ++i)
      pthread_join(writers[i], nullptr);

    // nanosleep(&ts, nullptr);
    for (int i = 0; i < n_readers; ++i)
      pthread_join(readers[i], nullptr);
    auto t2 = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
  }

public:
  static void WritersFuncTest(int n_writers, int size, int seed = 42) {
    FineSet<int> set;
    auto vals = GenVals(size, seed);
	std::vector<int> checkArr(size, 0);
    WritersTest(n_writers, set, vals, checkArr);

    std::string result = "success";
    for (int v : checkArr) {
      if (!v) {
        result = "fail (miss value)";
        break;
      }
    }

    std::cout << "Writing functional test\tn_writers: " << n_writers << "\tsize: " << size <<
      "\tresult: " << result << std::endl;
  }

  static void ReadersFuncTest(int n_readers, int size) {
    FineSet<int> set;
    for (int i = 0; i < size; ++i)
      set.Add(i);

    std::vector<int> checkArr(size, 0);

    ReadersTest(n_readers, set, checkArr);

    std::string result = "success";
    if (!set.Empty())
      result = "fail (set is not empty)";
    else {
      for (int v : checkArr) {
        if (!v) {
          result = "fail (miss value)";
          break;
        }
      }
    }

    std::cout << "Reading functional test\tn_readers: " << n_readers << "\tsize: " << size <<
      "\tresult: " << result << std::endl;
  }

  static void GeneralFuncTest(int sizeForReading, int sizeForWriting, int seed = 42) {
    auto vals = GenVals(sizeForWriting, seed);
    // std::sort(vals.begin(), vals.end());
    //debug std::cout << "generated values: ";
    //debug for (auto v : vals)
        //debug std::cout << v << " ";
    //debug std::cout << std::endl;
    int maxThreads = sysconf(_SC_NPROCESSORS_ONLN);

    int n_readers = 1;
    int n_writers = maxThreads - 1;

    std::cout << "General functional test\tsize for reading: " << sizeForReading << 
      "\tsize for writing: " << sizeForWriting << std::endl;

    while (n_readers < maxThreads) {
      FineSet<int> set;
      for (int i = 0; i < sizeForReading; ++i)
        set.Add(i);
      std::vector<int> readCheckArr(sizeForReading, 0);
	  std::vector<int> writeCheckArr(sizeForWriting, 0);
      GeneralTest(n_readers, n_writers, set, vals, readCheckArr, writeCheckArr);
    //   set.Print();
      std::string resultWriting = "success";
      for (int v : writeCheckArr) {
        if (!v) {
          resultWriting = "fail (miss value)";
          break;
        }
      }

      std::string resultReading = "success";
      for (int v : readCheckArr)
        if (!v) {
          resultReading = "fail (miss value)";
          break;
        }

      std::cout << "n_readers: " << n_readers << "\tn_writers: " << n_writers <<
        "\treading result: " << resultReading << "\twriting result: " << resultWriting << std::endl << std::endl;
      
      ++n_readers;
      --n_writers;
    }
  }
  
  static void WritersPerfTest(int n_writers, int size, int numOfTests, int seed = 42) {
    auto vals = GenVals(size, seed);

    long long dt = 0;
    int num_of_performed_tests = 0;
	std::vector<int> checkArr(size, 0);
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numOfTests; ++i, ++num_of_performed_tests) {
      FineSet<int> set;
      dt += WritersTest(n_writers, set, vals, checkArr);
      auto end_time = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
      if (duration > timeout_sec) {
        std::cout << "Writing performance test timeout" << std::endl;
        break;
      }
    }

    std::cout << "Writing performance test\tn_writers: " << n_writers << "\tsize: " << size <<
      "\ttime: " << dt / (num_of_performed_tests+1) << " ms" << std::endl;
  }

  static void ReadersPerfTest(int n_readers, int size, int numOfTests) {
    std::vector<FineSet<int>> sets(numOfTests);
    for (auto& set : sets)
      for (int i = 0; i < size; ++i)
        set.Add(i);

    std::vector<int> checkArr(size, 0);

    long long dt = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    int num_of_performed_tests = 0;
    for (auto& set : sets) {
      dt += ReadersTest(n_readers, set, checkArr);
      auto end_time = std::chrono::high_resolution_clock::now();
      ++num_of_performed_tests;
      auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
      if (duration > timeout_sec) {
        std::cout << "Reading performance test timeout" << std::endl;
        break;
      }
    }

    std::cout << "Reading performance test\tn_readers: " << n_readers << "\tsize: " << size <<
      "\ttime: " << dt / (num_of_performed_tests+1) << " ms" << std::endl;
  }

  static void GeneralPerfTest(int sizeForReading, int sizeForWriting, int numOfTests, int seed = 42) {
    auto vals = GenVals(sizeForWriting, seed);

    int maxThreads = sysconf(_SC_NPROCESSORS_ONLN);

    int n_readers = 1;
    int n_writers = maxThreads - 1;

    std::cout << "General functional test\tsize for reading: " << sizeForReading <<
      "\tsize for writing: " << sizeForWriting << std::endl;

    while (n_readers < maxThreads) {
      long long dt = 0;
      int num_of_performed_tests = 0;
      auto start_time = std::chrono::high_resolution_clock::now();
      for (int j = 0; j < numOfTests; ++j, ++num_of_performed_tests) {
        FineSet<int> set;
        for (int i = 0; i < sizeForReading; ++i)
          set.Add(i);
        std::vector<int> readCheckArr(sizeForReading, 0);
		std::vector<int> writeCheckArr(sizeForWriting, 0);
        dt += GeneralTest(n_readers, n_writers, set, vals, readCheckArr, writeCheckArr);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
        if (duration > timeout_sec) {
            std::cout << "General performance test timeout" << std::endl;
            break;
        }
      }

      std::cout << "n_readers: " << n_readers << "\tn_writers: " << n_writers <<
       "\ttime: " << dt / (num_of_performed_tests+1) << " ms" << std::endl;

      ++n_readers;
      --n_writers;
    }
  }
};

pthread_cond_t Tester::cv = PTHREAD_COND_INITIALIZER;
bool Tester::start = false;