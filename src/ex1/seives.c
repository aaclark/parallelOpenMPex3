/* File:     seives.c
 * Purpose:  Find all the primes for the natural number in the range 1..Max
 *
 * Compile:  gcc seives.c -o seives -fopenmp  -lm
 * Run:      ./seives <the highest ><max> <number of threads> <n>
 *           n is the number of terms of the series to use.
 *           n should be evenly divisible by the number of threads
 * Output:   Stores the result in three files
 *           report.txt will report how many for primes was found and the execution times for this run
 *           primes.txt will contain all the primes from this run
 *           statistics text will append information for each run about execution times for the selected parameters
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

/* First a sequential solution for finding primes
 Mark all multiples of k between k*k and Max in the marked_natural_numbers boolean array.*/
void mark_all_multiples_of_k_sequential(bool marked_natural_numbers[], int k, int max) {
     int number_to_mark = k*k;
     while(number_to_mark <= max) {
          if (!marked_natural_numbers[number_to_mark - 1]) {
            marked_natural_numbers[number_to_mark-1]=true;
          }
          number_to_mark +=k;
     }
}

int find_next_smallest_unmarked_number_sequential(bool marked_natural_numbers[], int k, int max) {
    for (int i = k; i < max; i++) {
        if (!marked_natural_numbers[i - 1]) {
            return i;
        }
    }
    
    // It is assumed that when mark_all_numbers_not_prime_sequential calls this function a next unmarked number is always found.
    // So it shouldn't be possible to end up here but better to report and exit if we unexpectedly haven't exited the function yet.
    fprintf(stderr, "Error: Number not found.\n");
    exit(EXIT_FAILURE);
}
/*
This function marks the natural numbers from k to max in the boolean array marked_natural_number that are not primes.
*/
void mark_all_numbers_not_prime_sequential(bool marked_natural_numbers[], int k, int max) {
    do {
        mark_all_multiples_of_k_sequential(marked_natural_numbers, k, max);
        k = find_next_smallest_unmarked_number_sequential(marked_natural_numbers, k+1, max);
    }  while (k*k <=max);
}

// Count how many primes there are (natural numbers that are not marked) in the range 1 .. max 
int count_primes(bool marked_natural_numbers[], int natural_number_max) {
    int primes_found = 0;
    for (int i = 0;i < natural_number_max; i++) {
        if (!marked_natural_numbers[i]) {
            primes_found++;
        }
    }
    return primes_found;
}

int main (int argc, char ** argv) {
    int nthreads = 8;
    int max = 110;
    if (argc > 1){
        max = atoi(argv[1]);
    }
    if (argc > 2)
        nthreads = atoi(argv[2]);
    
    struct timeval start, starting_threads, end;
    gettimeofday(&start, NULL);

    // A boolean array where you can check if a natural number n (<=max) is 'marked' by inspecting marked_natural_numbers[n-1]
    // We want to mark all numbers that is not a prime
    bool* marked_natural_numbers = (bool*)malloc(max * sizeof(bool));
    for (int i = 0; i < max; i++) {
        marked_natural_numbers[i] = false;
    }

    marked_natural_numbers[0] = true; // manually mark the first natural number 1
    int k = 2;
    int sequential_max = ceil(sqrt(max)); // Round upward to ensure sequential_max*sequential_max <= max
    
    mark_all_numbers_not_prime_sequential(marked_natural_numbers, k, sequential_max);

    int nr_of_prime_numbers_sequential = count_primes(marked_natural_numbers, sequential_max);
    
    printf("Number of primes found from the sequential part is %d \n", nr_of_prime_numbers_sequential);
    int* prime_numbers = (int*)malloc(nr_of_prime_numbers_sequential * sizeof(int));
    
    int prime_index = 0;
    for (int i=0; i<sequential_max; i++) {
        if (!marked_natural_numbers[i]) {
            int prime = i + 1;
            prime_numbers[prime_index] = prime;
            prime_index++;
        }
    }

   gettimeofday(&starting_threads, NULL);
   omp_set_num_threads(nthreads);
#pragma omp parallel for default(none) shared(sequential_max, max, marked_natural_numbers, prime_numbers, nr_of_prime_numbers_sequential)
    // Loop over all natural numbers in array not processed by sequential part
    for (int natural_number=sequential_max+1; natural_number<=max; natural_number++){
      for (int i = 0; i < nr_of_prime_numbers_sequential; i++) {
            int prime = prime_numbers[i];
            if (natural_number % prime == 0) {
                marked_natural_numbers[natural_number - 1] = true;
                break;
            }
        }
    }

    gettimeofday(&end, NULL);
    FILE* file_report = fopen("report.txt", "w");
    if (file_report == NULL) {
        printf("Error opening file for report!\n");
        return 1;
    }
    FILE* file_primes = fopen("primes.txt", "w");
    if (file_primes == NULL) {
        printf("Error opening file for writing the primes!\n");
        return 1;
    }

    FILE* file_statistics = fopen("statistics.txt", "a");
    if (file_statistics == NULL) {
        printf("Error opening file for statistics!\n");
        return 1;
    }

    double sequential_time_spent = ((double)(starting_threads.tv_usec - start.tv_usec) / 1000000.0) + starting_threads.tv_sec - start.tv_sec;
    double total_time_spent = ((double)(end.tv_usec - start.tv_usec) / 1000000.0) + end.tv_sec - start.tv_sec;
    fprintf(file_report, "The program is using %d threads to find all the primes for the natural numbers in the range 1 to %d \n", nthreads, max);
    fprintf(file_report, "Calculating all the primes took %f seconds:\n", total_time_spent);
    fprintf(file_report, "The sequential part calculated primes in the range 1 to %d and took %f seconds:\n", sequential_max, sequential_time_spent);
    fprintf(file_primes, "The first 1000 primes:\n");
    int number_of_primes = 0;
    for (int i = 0; i < max; i++) {
        // To avoid creating a huge file lets just write the first 1000 primes
        if (number_of_primes < 1000) {
            if (!marked_natural_numbers[i]){
                fprintf(file_primes, "%d ", (i + 1));
                number_of_primes++;
            }
        }
    }
    number_of_primes = count_primes(marked_natural_numbers, max);

    fclose(file_primes);
    fprintf(file_report, "Number of primes found %d\n", number_of_primes);
    fclose(file_report);
    fprintf(file_statistics, "\nUsing %d threads to find all the primes for the natural numbers in the range 1 to %d took %f seconds:\n", nthreads, max, total_time_spent);
    fprintf(file_statistics, "The sequential part took %f seconds:\n", sequential_time_spent);
    fprintf(file_statistics, "The threaded part took %f seconds:\n", total_time_spent - sequential_time_spent);
    fclose(file_statistics);

    free(marked_natural_numbers);
    free(prime_numbers);
}
