#include <vector>
//#include <thread>
//#include <pthread.h>
#include <omp.h>

#include "matrix_t.hpp"

/**
 * C++ Default implementation A.resize(n,val)
 * Resizes matrix A to n*n with default entries val
 * @tparam T
 * @param n
 * @param val
 * @return
 */
template <typename T>
matrix<T>&  matrix<T>::resize(const int n, const T& val) {
    N = n;
    long expected_size = ((long)N*(long)N);
    long current_size = values.size();
    long current_cap = values.capacity();
    long vec_max = values.max_size();
    if(expected_size > vec_max) {
        std::cerr << "Cannot resize to expected:" << expected_size << ", current: " << current_size << ", cap: " << current_cap << " ]" << std::endl;
        exit(EXIT_FAILURE);
    }
    values.resize(expected_size, val);
    current_size = values.size();
    if(expected_size < current_size) {
        std::cerr << "Failed to resize! expected:" << expected_size << ", current: " << current_size << ", cap: " << current_cap << " ]" << std::endl;
        exit(EXIT_FAILURE);
    }
    return *this;
};

/**
 * C++ Default implementation A.fill(val)
 * Fills matrix A with val
 * @tparam T
 * @param val
 * @return * matrix<T>
 */
template <typename T>
matrix<T>& matrix<T>::fill(const T& val) {
    std::fill(values.begin(), values.end(),val);
    return *this;
};

/**
 * Sequential implementation A.diagonal(val)
 * Sets the diagonal entries of A to val
 * @tparam T
 * @param val
 * @return * matrix<T>
 */
template <typename T>
matrix<T>& matrix<T>::diagonal(const T& val) {
    // 0; N+1; 2N+2; 3N+3; ...
    for(int i = 0; i < N; i++) values.at(addr(i,i)) = val;
    return *this;
};

/**
 * Parallel implementation A.upper(val)
 * Fills the upper-triangular portion of A with val
 * @tparam T
 * @param val
 * @return * matrix<T>
 */
template <typename T>
matrix<T>& matrix<T>::upper(const T& val) {
    // 0; N+1; 2N+2; 3N+3; ...
    long expected_size = ((long)N * (long)N);
    long current_size = values.size();
    long current_cap = values.capacity();
    long vec_max = values.max_size();
    if(expected_size > vec_max || expected_size > current_size) {
        std::cerr << "Insufficient dimensions! expected:" << expected_size << ", current: " << current_size << ", cap: " << current_cap << " ]" << std::endl;
        exit(EXIT_FAILURE);
    }
    /**
     * TODO fix:
     * When N = 100,000
     * (N*N) gets stuck evaluating to 1,410,065,408
     * because 100,000 = 10,000,000,000 > 4,294,967,295
     * so we get integer overflow
     * then i == 14100
     * goes out of bounds and EXC_BAD_ACCESS
     * since (14100 + 1)*(N) = 1,410,100,000 > 1,410,065,408
     * and values.begin() + 1,410,100,000 is out of bounds.
     */
#pragma omp for
    for(int i = 0; i < N; i++) {    // eg. N = 5
        //auto iter_a = std::next(values.begin(),((i)*(N))); // 0, 5, 10
        auto iter_b = std::next(values.begin(),((i)*(N+1))); // 0, 6, 12
        auto iter_c = std::next(values.begin(),((i+1)*(N))); // 5, 10, 15 // past end?
        //std::fill(iter_a, iter_b, 0); // none, [5,6), [10,12), ...
        std::fill(iter_b, iter_c, val); // [0,5), [6,10), [12,15), ...
    }
    return *this;
};
