#ifndef INC_3_MATRIX_VEC_SOLVE_HPP
#define INC_3_MATRIX_VEC_SOLVE_HPP

#ifndef matrix_t_hpp
#include "../matrix_t.hpp"
#endif

#ifndef vec_t_hpp
#include "../vec_t.hpp"
#endif

#if defined(matrix_t_hpp) && defined(vec_t_hpp)

// Matrix-vector operations
//vec<T>& solve_c(vec<T> other);
//vec<T>& solve_r(vec<T> b);

template <typename T>
bool solve_c(matrix<T>& A, vec<T>& x, vec<T>& b);

template <typename T>
bool solve_r(matrix<T>& A,vec<T>& x, vec<T>& b);

#endif //defined(matrix_t_hpp) && defined(vec_t_hpp)

#endif //INC_3_MATRIX_VEC_SOLVE_HPP
