#include <vector>
//#include <thread>
//#include <pthread.h>
//#include <omp.h>

#include "matrix_t.hpp"

template <typename T>
matrix<T>&  matrix<T>::resize(const int n, const T& val) {
    N = n;
    values.resize(N * N, val);
    return *this;
};
template <typename T>
matrix<T>& matrix<T>::fill(const T& val) {
    std::fill(values.begin(), values.end(),val);
    return *this;
};
template <typename T>
matrix<T>& matrix<T>::diagonal(const T& val) {
    // 0; N+1; 2N+2; 3N+3; ...
    for(int i = 0; i < N; i++) values.at(addr(i,i)) = val;
    return *this;
};
