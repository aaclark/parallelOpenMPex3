#!/bin/zsh

# Output file to store results
output_file="benchmark_results.txt"

# Matrix sizes to test; TODO: + 2500 5000
matrix_sizes=$(seq 1 16)  #(100 250 500 1000)

# Number of threads to test
thread_counts=(1 2 4 8 16 32)

# Clear the output file before starting the benchmarks
#echo "" > $output_file

typeset -i N START END
START=1020
END=2000
# Run the benchmarks
for threads in "${thread_counts[@]}"; do
  for ((N=START;N<=END;N+=20)); do
    # Set the number of threads for OpenMP
    export OMP_NUM_THREADS=$threads

    # Run the matrix_multiply program and capture the output
    result=$(./matrix_multiply $N)

    # Append the result to the output file
    echo "$result" >> $output_file
  done
done

echo "Benchmarking complete! Results written to $output_file"
