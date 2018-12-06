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

/*
 INT_MAX: 2.14 E 9
 UINT_MAX: 4.29 E 9
 */

long work(int seed) {
   long uint_limit_reached_count = 0;
   
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

int main(int argc, char **argv) {
   std::cout << "INT_MAX: " << INT_MAX << std::endl;
   std::cout << "UINT_MAX: " << UINT_MAX << std::endl;
   // (should be about 8B, or about 4*INT_MAX)
   
   int num_jobs = 1;
   
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
      sync_results[i] = work(7);
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
      futures[i] = std::async(work, 7);
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
