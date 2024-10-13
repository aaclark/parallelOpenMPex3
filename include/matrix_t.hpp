#ifndef matrix_t_hpp
#define matrix_t_hpp

#include <vector>
#include <cstdio>
#include <iostream>
#include <iomanip>

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

//    T& operator()(int x, int y);
//    matrix<T> operator*(matrix<T> other);
//    matrix<T> operator^(int exponent);

    T& operator()(int x, int y){
        return values.at(addr(x, y));
    };

    bool operator&&(matrix<T> other) {
        return this->N == other.N;
    }

    /**
     * Matrix multiplication
     */
    matrix<T> operator*(matrix<T> other) {
        /* static scheduling of matrix multiplication loops */
        //int dim = 3; // N = 3
        if(! (*this && other)) { exit(EXIT_FAILURE); }
        matrix<T> c; // NxN matrix = {v ... v}
        c.resize(N);

//        int i, j, k;
        // TODO FIX: ‘this’ allowed in OpenMP only in ‘declare simd’ clauses
//#pragma omp parallel default(none) private(i,j,k) shared(this->values, other, c)
        {
#pragma omp parallel for default(none) shared(c, other) schedule(static) collapse(1)
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    c(i, j) = 0;
                    for (int k = 0; k < N; k++) {
                        c(i, j) += (T)(*this)(i, k) * other(k, j);
                    }
                }
            }
        }

        return c;
    }
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