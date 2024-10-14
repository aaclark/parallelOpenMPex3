#include "matrix_vec_solve.hpp"

template <typename T>
bool solve_c(matrix<T>& A, vec<T>& x, vec<T>& b) {
    int N = A.size();
    int M = b.size();
    if (M != N)
        return false;
    x.resize(N);

#pragma omp parallel for default(none)  shared(x, b, N)
    for (int row = 0; row < N; row++) {
        x(row) = b(row);
    }
    //x = b; // b is never written; use implicit vec<T>(vec<T>) constructor instead

    // O(n^2)
    for (int col = N - 1; col >= 0; col--) {
        x(col) /= A(col, col);
        T x_row_sum = 0;
#pragma omp parallel for default(none) firstprivate(col) shared(A, x, N)
        for (int row = 0; row < col; row++) {
            x(row) -= (A(row, col) * x(col));
        }
    }
    return true;
}


template <typename T>
bool solve_r(matrix<T>& A,vec<T>& x, vec<T>& b) {
    int N = A.size();
    int M = b.size();
    if (M != N)
        return false;
    x.resize(N);

#pragma omp parallel for default(none)  shared(x, b, N)
    for (int row = 0; row < N; row++) {
        x(row) = b(row);
    }
    //x = b; // b is never written; use implicit vec<T>(vec<T>) constructor instead

    // O(n^2)
    for (int row = N-1; row >= 0; row--) {
//        x(row) = b(row);
        T x_col_sum = 0;
#pragma omp parallel for default(none) firstprivate(row) shared(A, x, N) reduction(- : x_col_sum)
        for (int col = row+1; col < N; col++) {
            x_col_sum -= (A(row, col) * x(col));
//            x(row) -= (A(row, col) * x(col));
        }
        x(row) += x_col_sum;
        x(row) /= A(row, row); //(x(row) - x_row_sum)
    }
    return true;
}
