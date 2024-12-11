#!/bin/bash

# Step 1: Compile with coverage flags
g++ -g -fprofile-arcs -ftest-coverage -o test_suite TestSuite.cpp BoruvkaSolver.cpp PrimSolver.cpp MSTFactory.cpp Graph.cpp MSTTree.cpp

# Step 2: Run the test suite
./test_suite

# Step 3: Generate the coverage report
gcov TestSuite.cpp BoruvkaSolver.cpp PrimSolver.cpp MSTFactory.cpp Graph.cpp MSTTree.cpp

# Display a message that the coverage report is generated
echo "Code coverage report generated!"
