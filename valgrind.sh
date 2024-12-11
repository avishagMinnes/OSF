#!/bin/bash

# Define the output directories for Valgrind reports
MEMCHECK_OUTPUT="memcheck_report.txt"
HELGRIND_OUTPUT="helgrind_report.txt"

# Compile the project with debug information
echo "Compiling the project with -g flag for Valgrind analysis..."
g++ -g -o mst_server main.cpp MSTFactory.cpp BoruvkaSolver.cpp PrimSolver.cpp Graph.cpp Edge.cpp Vertex.cpp MSTTree.cpp IMSTSolver.cpp

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting..."
    exit 1
fi

echo "Compilation successful. Running Valgrind analysis..."

# Run Valgrind Memcheck for memory leak detection
echo "Running Valgrind Memcheck..."
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=$MEMCHECK_OUTPUT ./mst_server

if [ $? -ne 0 ]; then
    echo "Valgrind Memcheck failed. Check $MEMCHECK_OUTPUT for details."
else
    echo "Valgrind Memcheck completed. Check $MEMCHECK_OUTPUT for details."
fi

# Run Valgrind Helgrind for detecting thread race conditions
echo "Running Valgrind Helgrind..."
valgrind --tool=helgrind --log-file=$HELGRIND_OUTPUT ./mst_server

if [ $? -ne 0 ]; then
    echo "Valgrind Helgrind failed. Check $HELGRIND_OUTPUT for details."
else
    echo "Valgrind Helgrind completed. Check $HELGRIND_OUTPUT for details."
fi

echo "Valgrind analysis completed."
