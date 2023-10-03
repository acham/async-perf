""" Conftest file for python tests """

def pytest_addoption(parser):
    parser.addoption("--num_jobs", action="store", default=2)
    parser.addoption("--seed", action="store", default=1)
