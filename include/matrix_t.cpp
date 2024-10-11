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
    values.resize(N * N, val);
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
