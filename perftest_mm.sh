#!/bin/zsh

# Output file to store results
output_file="benchmark_results_mm.txt"

# optional Matrix sizes to test;
#matrix_sizes=$(seq 1 16)

# Number of threads to test
thread_counts=(2 4 8 16)

# Clear the output file before starting the benchmarks
echo "" > $output_file

typeset -i N START END
START=200
END=2000
# Run the benchmarks
for threads in "${thread_counts[@]}"; do
  for ((N=START;N<=END;N+=200)); do
    # Set the number of threads for OpenMP
    export OMP_NUM_THREADS=$threads
    echo "T=${OMP_NUM_THREADS}, N=${N}"

    # Run the matrix_multiply program and capture the output
    result=$(./matrix_multiply $N)

    # Append the result to the output file
    echo "$result" >> $output_file
  done
done

echo "Benchmarking complete! Results written to $output_file"
