package main

import (
	"math"
	"os"
	"strconv"
	"unsafe"

	log "github.com/sirupsen/logrus"
)

func main() {
	argsWithoutProg := os.Args
	if len(argsWithoutProg) != 3 {
		usage()
	}

	num_jobs, err := strconv.Atoi(os.Args[1])
	handle(err)
	seed, err := strconv.Atoi(os.Args[2])
	handle(err)

	// Info
	log.Info(num_jobs, seed)
	log.Printf("Size of int: %d\n", unsafe.Sizeof(seed))

	if num_jobs >= 10000 || seed >= 1000 {
		log.Fatal("Arguments out of range. num-jobs must be < 10000, seed < 1000.")
	}

	log.Info("Running ", num_jobs, " jobs with a seed of ", seed)
	log.Info("Size of int: ", unsafe.Sizeof(seed))
	log.Info("MaxInt32: ", math.MaxInt32)
	log.Info("MaxUint32: ", math.MaxUint32)

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
