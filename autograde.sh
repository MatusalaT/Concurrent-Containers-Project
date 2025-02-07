#!/bin/bash

# Exit the script on any error
set -e

# Compilation step
echo "Compiling the program..."
g++ -std=c++20 -pthread -o test_containers main.cpp

# Define test configurations
declare -a thread_counts=(1 2 4 8)
declare -a operations_counts=(10 50 100 500)

# Run tests for all configurations
echo "Running tests with various configurations..."
for threads in "${thread_counts[@]}"; do
    for ops in "${operations_counts[@]}"; do
        echo "Testing with $threads threads and $ops operations per thread..."
        ./test_containers "$threads" "$ops"
        echo "Test passed with $threads threads and $ops operations per thread."
    done
done

echo "All tests passed successfully!"
