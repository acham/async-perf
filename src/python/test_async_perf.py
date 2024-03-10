import os
import logging
import time

from multiprocessing import Pool

import pytest

logging.basicConfig()
_logger = logging.getLogger(__name__)

@pytest.fixture(scope="session")
def num_jobs(pytestconfig):
    return pytestconfig.getoption("num_jobs")

@pytest.fixture(scope="session")
def seed(pytestconfig):
    return pytestconfig.getoption("seed")

def usage():
    _logger.info("\n\tUsage: "
                 "\n\t $ python -m pytest test_async_perf.py --num_jobs [num-jobs] --seed [seed]"
                 "\n\twhere seed determines the size of each job."
                 "\n\tOne job with seed 1 runs in about 20s on a modern commodity CPU."
                 "\n\tUse more than 1 job to measure multi-core speedup.")

def poly(x):
    y = x**3 - 4 * x**2 + x
    return y 

def perform_work(seed: int):
    s = 0
    start = 0
    end = 100
    steps = int(seed * 1e08)
    dx = (end - start) / steps

    for i in range(steps):
        s += poly(i * dx)

    return s * dx

def perform_work_cpu_time(seed:int):
    cpu_time_start = time.process_time()
    perform_work_result = perform_work(seed)
    cpu_duration = time.process_time() - cpu_time_start

    return (perform_work_result, cpu_duration)

def test_speedup(num_jobs, seed):
    """ The main test function. """
    if os.name == "nt":
        _logger.info("Windows not supported yet.")

    _logger.info(f"num_jobs {num_jobs}, seed {seed}")

    try:
        inum_jobs = int(num_jobs)
        iseed = int(seed)
    except TypeError:
        usage()
        return

    if inum_jobs >= 10000 or iseed >= 1000:
        raise ValueError("Arguments out of range. num_jobs must be < 10000, seed < 1000.")

    _logger.info("Running %i job(s) with a seed of %i" % (inum_jobs, iseed))

    sync_results = []

    # Sync jobs
    sync_cpu_time_start = time.process_time()
    sync_wall_clock_start = time.time()

    for _ in range(inum_jobs):
        sync_results.append(perform_work(iseed))

    sync_cpu_duration = time.process_time() - sync_cpu_time_start
    sync_wall_clock_duration = time.time() - sync_wall_clock_start

    # Async jobs
    async_wall_clock_start = time.time()

    with Pool() as pool:
        async_results = pool.starmap(perform_work_cpu_time, [(iseed,) for i in range(inum_jobs)])

    async_wall_clock_duration = time.time() - async_wall_clock_start
    async_cpu_duration = sum([async_results_tuple[1] for async_results_tuple in async_results])
    speedup = sync_wall_clock_duration / async_wall_clock_duration

    _logger.info("async result: %f" % async_results[0][0])
    _logger.info("sync wall-clock duration: %f" % sync_wall_clock_duration)
    _logger.info("sync cpu duration: %f" % sync_cpu_duration)
    _logger.info("async wall-clock duration: %f" % async_wall_clock_duration)
    _logger.info("async cpu duration: %f" % async_cpu_duration)
    _logger.info("speedup: %f" % speedup)
