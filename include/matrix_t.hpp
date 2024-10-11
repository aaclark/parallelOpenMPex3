#ifndef matrix_t_hpp
#define matrix_t_hpp

#include <vector>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <omp.h>
#define DEBUG true

template <typename T>
class matrix {
    int N {};
    std::vector<T> values;
    int addr(int x, int y) const{return (x+N*y);}
    T& operator()(int x, int y) const{
        return (values.at(addr(x, y)));
    };
public:
    void show() {
        for (int y=0; y<N; y++) {
            for (int x = 0; x < N; x++) {
                std::cout << std::left << std::setfill(' ') << std::setw(10) << std::setprecision(4) << (*this)(x,y);
            }
            std::cout << std::endl;
        }
    }
    matrix<T>& resize(int n, const T& val = 0);
    matrix<T>& fill(const T& val);
    matrix<T>& diagonal(const T& val);
    matrix<T>& upper(const T& val);

//    T& operator()(int x, int y);
//    matrix<T> operator*(matrix<T> other);
//    matrix<T> operator^(int exponent);

    /**
     * Sequential implementation A(x,y)
     * Accessor for entry at (x,y)
     * @param other
     * @return & T
     */
    T& operator()(int x, int y){
        return values.at(addr(x, y));
    };

    /**
     * Parallel implementation A*B
     * Multiplies matrix A by matrix B
     * @param other
     * @return * matrix<T>
     */
    bool operator&&(matrix<T> other) {
        return this->N == other.N;
    }

    /**
     * Parallel implementation A*B
     * Multiplies matrix A by matrix B
     * @param other
     * @return * matrix<T>
     */
    matrix<T> operator*(matrix<T> other) {
        /* static scheduling of matrix multiplication loops */
        //int dim = 3; // N = 3
        if(! (*this && other)) { exit(EXIT_FAILURE); }
        matrix<T> c; // NxN matrix = {v ... v}
        c.resize(N);

        int i, j, k;
        // TODO FIX: ‘this’ allowed in OpenMP only in ‘declare simd’ clauses
#pragma omp parallel default(none) private(i,j,k) shared(other, c)
        {
#pragma omp for schedule(static)
            for (i = 0; i < N; i++) {
                for (j = 0; j < N; j++) {
                    c(i, j) = 0;
                    for (k = 0; k < N; k++) {
                        c(i, j) += (T)(*this)(i, k) * other(k, j);
                    }
                }
            }
        }

        return c;
    }

    /**
     * Sequential implementation A^k
     * Multiplies matrix A by itself k times
     * @param other
     * @return * matrix<T>
     */
    matrix<T>& operator^(const int exponent){
        matrix<T> c; // NxN matrix = {v ... v}
        c.resize(N);
        c.diagonal(1.0);
        if(exponent == 1)
            return *this;
        return (*this)*((*this)^(exponent-1));
    }


    /* default implementations:
     * default constructor
     * copy constructor (note: shallow copy)
     * move constructor
     * copy assignment operator (note: shallow copy)
     * move assignment operator
     *
     * The first is required due to the others,
     * which are explicitly listed due to the rule of five.
     */
    matrix() = default;
//    matrix(const matrix<T>& other) = default;
//    matrix(matrix<T>&& other) = default;
//    matrix<T>& operator=(const matrix<T>& other) = default;
//    matrix<T>& operator=(matrix<T>&& other) = default;
    ~matrix() = default;

};

#endif // matrix_t_hpp
