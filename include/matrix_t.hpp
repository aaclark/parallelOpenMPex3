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

    matrix<T> operator*(matrix<T> other) {
        /* static scheduling of matrix multiplication loops */
        //int dim = 3; // N = 3
        if(! (*this && other)) { exit(EXIT_FAILURE); }
        matrix<T> c; // NxN matrix = {v ... v}
        c.resize(N);

        int i, j, k;
        //#pragma omp parallel default(private) shared(a, b, c, dim) num_threads(4)
        //#pragma omp for schedule(static)
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                c(i,j) = 0;
                for (k = 0; k < N; k++) {
                    c(i,j) += (double_t)(*this)(i,k) * other(k,j);
                }
            }
        }

        (*this).show();
        std::cout << "    *    " << std::endl;
        (other).show();
        std::cout << "--------" << std::endl;
        c.show();
        std::cout << std::endl;

        return c;
    }
    matrix<T> operator^(const int exponent) const{
        matrix<T> c; // NxN matrix = {v ... v}
        c.resize(N);
        c.diagonal(1.0);
        if(exponent == 0)
            return *this;
        return this*(this^(exponent-1));
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