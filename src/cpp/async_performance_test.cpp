//
//  async_performance_test.cpp
//  BuiltInParallelizationTesting
//
//  Created by Alexis Champsaur on 12/5/18.
//  Copyright © 2018 Alexis Champsaur. All rights reserved.
//

#include <iostream>
#include <climits>
#include <future>
#include <sys/time.h>
#include <stdlib.h>

/*
 INT_MAX: 2.14 E 9
 UINT_MAX: 4.29 E 9
 */

long work(int seed) {
   long uint_limit_reached_count = 0;
   
   for (int j = 0; j < seed; j++)
      for (long i = 0; i < static_cast<long>(INT_MAX); ++i)
      {
         auto product = i * INT_MAX;
         auto quotient = product / seed;
         if (quotient > UINT_MAX)
            ++uint_limit_reached_count;
      }
   
   return uint_limit_reached_count;
}

double get_wall_time() {
   struct timeval time;
   if (gettimeofday(&time,NULL)) {
      //  Handle error
      return 0;
   }
   return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void usage() {
   std::cerr << "Usage: async-perf [num-jobs] [seed]" <<
      "\n\twhere seed determines the size of each job. \n\tOne job with seed 1 runs in about 18s on a modern commodity CPU." <<
      "\n\tnum-jobs and seed must be integers greater than 0. num-jobs must be < 10000, seed < 1000." <<
      std::endl;
   exit(1);
}

int main(int argc, char *argv[]) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
   std::cerr << "Windows not supported yet." << std::endl;
   exit(1);
#endif
   
   // Argument parsing
   if (argc != 3)
      usage();
   
   long lnum_jobs = strtol(argv[1], NULL, 0);
   long lseed = strtol(argv[2], NULL, 0);
   
   if (lnum_jobs >= 10000 || lseed >= 1000) {
      std::cerr << "Arguments out of range. num-jobs must be < 10000, seed < 1000." << std::endl;
      exit(1);
   }
   
   int num_jobs = static_cast<int>(lnum_jobs);
   int seed = static_cast<int>(lseed);
   
   std::cout << "Running " << num_jobs << " jobs with a seed of " << seed << std::endl;
   std::cout << "Size of long: " << sizeof(lnum_jobs) * 8 << std::endl;
   std::cout << "Size of int: " << sizeof(num_jobs) * 8 << std::endl;
   std::cout << "INT_MAX: " << INT_MAX << std::endl;
   std::cout << "UINT_MAX: " << UINT_MAX << std::endl;
   
   clock_t sync_cpu_start, async_cpu_start;
   double sync_cpu_duration, async_cpu_duration,
   sync_wall_clock_start, async_wall_clock_start,
   sync_wall_clock_duration, async_wall_clock_duration;
   
   long sync_results[num_jobs];
   long async_results[num_jobs];
   
   /** Sync jobs **/
   sync_cpu_start = clock();
   sync_wall_clock_start = get_wall_time();
   for (int i = 0; i < num_jobs; ++i)
   {
      sync_results[i] = work(seed);
   }
   sync_wall_clock_duration = get_wall_time() - sync_wall_clock_start;
   sync_cpu_duration = (clock() - sync_cpu_start) / (double)CLOCKS_PER_SEC;
   
   std::cout << "sync values (sanity check): " << std::endl;
   for (int i = 0; i < num_jobs; ++i)
   {
      std::cout << sync_results[i] << " ";
   }
   std::cout << std::endl;
   std::cout << "sync CPU duration: " << sync_cpu_duration << " s" << std::endl;
   std::cout << "sync wall-clock duration: " << sync_wall_clock_duration << " s" << std::endl;
   
   /** Async jobs **/
   std::future<long> futures[num_jobs];
   
   async_cpu_start = clock();
   async_wall_clock_start = get_wall_time();
   // Start the async jobs
   for (int i = 0; i < num_jobs; ++i)
   {
      futures[i] = std::async(work, seed);
   }
   
   // Wait for and collect the results
   for (int i = 0; i < num_jobs; ++i)
   {
      async_results[i] = futures[i].get();
   }
   async_wall_clock_duration = get_wall_time() - async_wall_clock_start;
   async_cpu_duration = (clock() - async_cpu_start) / (double)CLOCKS_PER_SEC;
   
   std::cout << "async values (sanity check): " << std::endl;
   for (int i = 0; i < num_jobs; ++i)
   {
      std::cout << async_results[i] << " ";
   }
   std::cout << std::endl;
   
   std::cout << "async CPU duration: " << async_cpu_duration << " s" << std::endl;
   std::cout << "async wall-clock duration: " << async_wall_clock_duration << " s" << std::endl;
   std::cout << "speedup: " << sync_wall_clock_duration / async_wall_clock_duration << std::endl;
   
   return 0;
}
