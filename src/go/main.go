package main

import (
	"math"
	"os"
	"strconv"
	"time"
	"unsafe"

	log "github.com/sirupsen/logrus"
)

//#include <time.h>
import "C"

func main() {
	argsWithoutProg := os.Args
	if len(argsWithoutProg) != 3 {
		usage()
	}

	num_jobs, err := strconv.Atoi(os.Args[1])
	handle(err)
	seed, err := strconv.Atoi(os.Args[2])
	handle(err)

	if num_jobs >= 10000 || seed >= 1000 {
		log.Fatal("Arguments out of range. num-jobs must be < 10000, seed < 1000.")
	}

	log.Info("Size of int: ", unsafe.Sizeof(seed)*8)
	log.Info("MaxInt32: ", math.MaxInt32)
	log.Info("MaxUint32: ", math.MaxUint32)
	log.Info("Running ", num_jobs, " jobs with a seed of ", seed)

	sync_results := make(chan int64, num_jobs)
	async_results := make(chan int64, num_jobs)

	/** Sync jobs **/
	sync_cpu_start := C.clock()
	sync_wall_start := time.Now()

	for i := 0; i < num_jobs; i++ {
		work(seed, sync_results)
	}
	sync_wall_duration := time.Since(sync_wall_start)
	sync_cpu_duration := float64(C.clock()-sync_cpu_start) / float64(C.CLOCKS_PER_SEC)

	log.Info("sync values (sanity check): ")
	var sync_vals string
	for i := 0; i < num_jobs; i++ {
		sync_vals += strconv.FormatInt(<-sync_results, 10) + " "
	}
	log.Info(sync_vals)
	log.Info("sync CPU duration: ", sync_cpu_duration, " s")
	log.Info("sync wall-clock duration: ", sync_wall_duration.Seconds(), " s")

	/** Async jobs **/
	async_cpu_start := C.clock()
	async_wall_start := time.Now()
	// Start async jobs
	for i := 0; i < num_jobs; i++ {
		go work(seed, async_results)
	}
	var async_vals string
	for i := 0; i < num_jobs; i++ {
		async_vals += strconv.FormatInt(<-async_results, 10) + " "
	}
	// Here: all results have been collected
	async_wall_duration := time.Since(async_wall_start)
	async_cpu_duration := float64(C.clock()-async_cpu_start) / float64(C.CLOCKS_PER_SEC)

	log.Info("async values (sanity check): ")
	log.Info(async_vals)
	log.Info("async CPU duration: ", async_cpu_duration, " s")
	log.Info("async wall-clock duration: ", async_wall_duration.Seconds(), " s")
	log.Info("speedup: ", sync_wall_duration.Seconds()/async_wall_duration.Seconds())
}

func usage() {
	log.Error("Usage: async-perf [num-jobs] [seed]" +
		"\n\twhere seed determines the size of each job." +
		"\n\tOne job with seed 1 runs in about 18s on a modern commodity CPU." +
		"\n\tnum-jobs and seed must be integers greater than 0. num-jobs must be < 10000, seed < 1000.")
	os.Exit(1)
}

func handle(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func work(seed int, results chan int64) {
	var uint_limit_reached_count int64 = 0

	for j := 0; j < seed; j++ {
		// likely both vars below are 64-bit already but just to be sure
		var i int64
		for i = 0; i < int64(math.MaxInt32); i++ {
			product := i * math.MaxInt32
			quotient := product / int64(seed)
			if quotient > math.MaxUint32 {
				uint_limit_reached_count++
			}
		}
	}

	results <- uint_limit_reached_count
}
