#ifndef vec_t_hpp
#define vec_t_hpp
//#ifdef matrix_t_hpp
//#include "matrix_vec_shared_t.hpp"
//#endif

#include <vector>
#include <cstdio>
#include <iostream>
#include <iomanip>

#define DEBUG true

template <typename T>
class vec {
    int M {0};
    std::vector<T> values;
    T& operator()(int i) const{
        return (values.at(i));
    };
public:
    int size() {return M;}
    void show() {
//        std::cout << "[ ";
        for (int i = 0; i < M; i++) {
            std::cout << std::left << std::setfill(' ') << std::setw(12) << std::setprecision(4) << (*this)(i);
        }
//        std::cout << " ]";
        std::cout << std::endl;
    }
    vec<T>& resize(int n, const T& val = 0);
    vec<T>& fill(const T& val);

    /**
     * Sequential implementation V(i)
     * Accessor for i'th entry of V
     * @param other
     * @return & T
     */
    T& operator()(int i){
        return values.at(i);
    };

    /**
     * C++ Default implementation V && W
     * Checks that V, W have equal length
     * @param other
     * @return bool
     */
    //friend bool operator&&(const vec<T>&, const vec<T>&);


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
    vec() = default;
//    vec(const vec<T>& other) = default;
//    vec(vec<T>&& other) = default;
//    vec<T>& operator=(const vec<T>& other) = default;
//    vec<T>& operator=(vec<T>&& other) = default;
    ~vec() = default;

};

#endif // vec_t_hpp