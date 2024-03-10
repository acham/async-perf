""" Conftest file for python tests """

def pytest_addoption(parser):
    parser.addoption("--num_jobs", action="store")
    parser.addoption("--seed", action="store")
