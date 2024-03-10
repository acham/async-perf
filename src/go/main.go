package main

import (
	"math"
	"os"
	"strconv"
	"time"

	log "github.com/sirupsen/logrus"
)

//#include <time.h>
import "C"

func main() {
	log.Info("Starting aync-perf in Go")

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

	log.Info("Running ", num_jobs, " jobs with a seed of ", seed)

	sync_results := make(chan float64, num_jobs)
	async_results := make(chan float64, num_jobs)

	/** Sync jobs **/
	sync_cpu_start := C.clock()
	sync_wall_start := time.Now()

	for i := 0; i < num_jobs; i++ {
		work(int32(seed), sync_results)
	}
	sync_wall_duration := time.Since(sync_wall_start)
	sync_cpu_duration := float64(C.clock()-sync_cpu_start) / float64(C.CLOCKS_PER_SEC)

	log.Info("sync CPU duration: ", sync_cpu_duration, " s")
	log.Info("sync wall-clock duration: ", sync_wall_duration.Seconds(), " s")

	/** Async jobs **/
	async_cpu_start := C.clock()
	async_wall_start := time.Now()
	// Start async jobs
	for i := 0; i < num_jobs; i++ {
		go work(int32(seed), async_results)
	}

	log.Printf("async result: %f", <-async_results)

	// Here: all results have been collected
	async_wall_duration := time.Since(async_wall_start)
	async_cpu_duration := float64(C.clock()-async_cpu_start) / float64(C.CLOCKS_PER_SEC)

	log.Info("async CPU duration: ", async_cpu_duration, " s")
	log.Info("async wall-clock duration: ", async_wall_duration.Seconds(), " s")
	log.Info("speedup: ", sync_wall_duration.Seconds()/async_wall_duration.Seconds())
}

func usage() {
	log.Error("Usage: async-perf [num-jobs] [seed]" +
		"\n\twhere seed determines the size of each job." +
		"\n\tOne job with seed 1 runs in about 2s on a modern commodity CPU." +
		"\n\tnum-jobs and seed must be integers greater than 0. num-jobs must be < 10000, seed < 1000.")
	os.Exit(1)
}

func handle(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func poly(x float64) float64 {
	y := float64(x)
	res := math.Pow(y, 3) - 4*math.Pow(y, 2) + y
	return float64(res)
}

func work(seed int32, results chan float64) {
	var s float64 = 0
	var start float64 = 0
	var end float64 = 100
	var steps int32 = seed * 1e8
	var dx float64 = (end - start) / float64(steps)

	for i := 0; i < int(steps); i++ {
		s += poly(float64(i) * dx)
	}

	result := s * dx
	results <- result
}
