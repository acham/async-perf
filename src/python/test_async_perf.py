import ctypes
import os
import logging
import math
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

def limits(c_int_type):
    signed = c_int_type(-1).value < c_int_type(0).value
    bit_size = ctypes.sizeof(c_int_type) * 8
    signed_limit = 2 ** (bit_size - 1)
    return (-signed_limit, signed_limit - 1) if signed else (0, 2 * signed_limit - 1)

def perform_work(seed: int, int_max: int):
    uint_limit_reached_count = 0

    for j in range(seed):
        for i in range(math.floor(int_max/8)):
            product = i * int_max
            quotient = product / seed
            if quotient > int_max:
                uint_limit_reached_count += 1
    
    return uint_limit_reached_count

def perform_work_cpu_time(seed:int, int_max: int):
    cpu_time_start = time.process_time()
    perform_work_result = perform_work(seed, int_max)
    cpu_duration = time.process_time() - cpu_time_start

    return (perform_work_result, cpu_duration)

def test_speedup(num_jobs, seed):
    """ The main test function. """
    if os.name == "nt":
        _logger.info("Windows not supported yet.")

    inum_jobs = int(num_jobs)
    iseed = int(seed)

    if inum_jobs >= 10000 or iseed >= 1000:
        raise ValueError("Arguments out of range. num_jobs must be < 10000, seed < 1000.")

    INT_MAX = limits(ctypes.c_int32)[1]
    UINT_MAX = limits(ctypes.c_uint32)[1]
    _logger.info("INT_MAX: %i" % INT_MAX)
    _logger.info("UINT_MAX: %i" % UINT_MAX)
    _logger.info("Running %i jobs with a seed of %i" % (inum_jobs, iseed))

    sync_results = []

    # Sync jobs
    sync_cpu_time_start = time.process_time()
    sync_wall_clock_start = time.time()

    for _ in range(inum_jobs):
        sync_results.append(perform_work(iseed, INT_MAX))

    sync_cpu_duration = time.process_time() - sync_cpu_time_start
    sync_wall_clock_duration = time.time() - sync_wall_clock_start

    # Async jobs
    async_wall_clock_start = time.time()

    with Pool() as pool:
        async_results = pool.starmap(perform_work_cpu_time, [(iseed, INT_MAX) for i in range(inum_jobs)])

    async_wall_clock_duration = time.time() - async_wall_clock_start
    async_cpu_duration = sum([async_results_tuple[1] for async_results_tuple in async_results])
    speedup = sync_wall_clock_duration / async_wall_clock_duration

    _logger.info("sync wall-clock duration: %f" % sync_wall_clock_duration)
    _logger.info("sync cpu duration: %f" % sync_cpu_duration)
    _logger.info("async wall-clock duration: %f" % async_wall_clock_duration)
    _logger.info("async cpu duration: %f" % async_cpu_duration)
    _logger.info("speedup: %f" % speedup)
