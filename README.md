# Async-perf
Uses built-in parallelization mechanisms (`async` in C++, goroutines in Go, Python multiprocessing pools) in various programming languages to measure speedup.

Requires: g++ with c++14 support, go 1.12, make, Python environment with Python version 3.10+.

Build: `make`.

Usage for c++ and go: `async-perf-[go/cpp] [num-jobs] [seed]` where `seed` determines the size of each job.
One job with `seed` 1 runs in about 18s on a modern commodity CPU.
`num-jobs` and `seed` must be integers greater than 0. `num-jobs` must be `< 10000`, `seed < 1000`.

Usage for Python: install `requirements.txt` in a Python environment, and run: `pytest --num_jobs [num-jobs] --seed [seed] src/python` from the top-level directory.
