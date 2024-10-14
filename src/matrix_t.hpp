#ifndef matrix_t_hpp
#define matrix_t_hpp

#include <vector>
#include <cstdio>
#include <iostream>
#include <iomanip>

#define DEBUG true

#define COLLAPSE_LEVEL 1

template <typename T>
class matrix {
    int N {0};
    std::vector<T> values;
    int addr(int row, int i) const{return (N * row) + i;}
    T& operator()(int row, int i) const{
        return (values.at(addr(row, i)));
    };
public:
    char collapse_level {1};
    int size() {return N;}
    void show() {
        for (int row=0; row < N; row++) { // for each ROW:
            for (int i = 0; i < N; i++) { // for each index {x_0, x_1 ...} :
                std::cout << std::left << std::setfill(' ') << std::setw(8) << std::setprecision(4) << (*this)(row, i);
            }
            std::cout << std::endl;
        }
    }

    // Matrix operations

    matrix<T>& resize(int n, const T& val = 0);
    matrix<T>& fill(const T& val);
    matrix<T>& diagonal(const T& val);
    matrix<T>& upper(const T& val);


    /**
     * Sequential implementation A(row, i)
     * Accessor for entry at (row, i)
     * @param other
     * @return & T
     */
    T& operator()(int row, int i){
        return values.at(addr(row, i));
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

        /**
         * According to OpenMP:
         * If execution of any associated loop changes any of the values
         * used to compute any of the iteration counts, then the behavior
         * is unspecified.
         */

//        int row, i, j; //private(row,i,j)
        {
#pragma omp parallel for default(none) shared(other, c) schedule(static) collapse(COLLAPSE_LEVEL)
                for (int row = 0; row < N; row++) { // row
                    for (int i = 0; i < N; i++) {
                        c(row, i) = 0;
                        for (int j = 0; j < N; j++) {
                            c(row, i) += (T)(*this)(row, j) * other(j, i);
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