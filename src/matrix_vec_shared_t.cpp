#include "matrix_vec_shared_t.hpp"

template <typename T>
bool solve_c(matrix<T>& A, vec<T>& x, vec<T>& b) {
    int N = A.size();
    int M = b.size();
    if (M != N)
        return false;
    int row, col;
    x.resize(N);
    for (row = 0; row < N; row++)
        x(row) = b(row);
    for (col = N-1; col >= 0; col--) {
        x(col) /= A(col,col);
        for (row = 0; row < col; row++)
            x(row) -= A(row,col) * x(col);
    }
    return true;
}


template <typename T>
bool solve_r(matrix<T>& A,vec<T>& x, vec<T>& b) {
    int N = A.size();
    int M = b.size();
    if (M != N)
        return false;
    int row, col;
    x.resize(N);
    for (row = N-1; row >= 0; row--) {
        x(row) = b(row);
        for (col = row+1; col < N; col++)
            x(row) -= A(row, col) * x(col);
        x(row) /= A(row, row);
    }
    return true;
}
