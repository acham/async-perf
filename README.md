# Async-perf
Uses built-in parallelization mechanisms in various programming languages to measure speedup.

Requires: g++ with c++14 support, go 1.12, make.

Build: `make`.

Usage: `async-perf-[go/cpp] [num-jobs] [seed]` where seed determines the size of each job.
One job with seed 1 runs in about 18s on a modern commodity CPU.
`num-jobs` and `seed` must be integers greater than 0. `num-jobs` must be `< 10000`, `seed < 1000`.
