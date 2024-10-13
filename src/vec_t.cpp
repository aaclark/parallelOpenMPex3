#include <vector>
//#include <thread>
//#include <pthread.h>
//#include <omp.h>

#include "vec_t.hpp"

/**
 * C++ Default implementation A.resize(n,val)
 * Resizes vec A to n*n with default entries val
 * @tparam T
 * @param m
 * @param val
 * @return * vec<T>
 */
template <typename T>
vec<T>&  vec<T>::resize(const int m, const T& val) {
    M = m;
    values.resize(M, val);
    return *this;
};

/**
 * C++ Default implementation A.fill(val)
 * Fills vec A with val
 * @tparam T
 * @param val
 * @return * vec<T>
 */
template <typename T>
vec<T>& vec<T>::fill(const T& val) {
    std::fill(values.begin(), values.end(),val);
    return *this;
};
