#include "matrix_vec_shared_t.hpp"

/**
 * Loop Analysis:
 * for i = lower, upper, stride; { a(i) }
 * --> for i’ = 0, (upper – lower + stride)/stride – 1, 1; {a(i’*stride + lower)}
 * let i' = _L1 ; j' = _L2 ; etc.
 * auto _UPPER_NORM = [](int l, int u, int s)->int{return (u - l + s)/s - 1;};
 * auto _INDEX_NORM = [](int idx, int l, int s)->int{return idx*s+l;};
 */
auto _norm_N = [](int l, int s, int u)->int{return (u - l + s)/s - 1;};
auto _norm_fn_idx = [](int l, int s, int u){return [l,s](int idx){return idx*s+l;};};
//auto INDEX_NORM = [](int l, int s, int i){return i*s+l;};

template <typename T>
bool solve_c(matrix<T>& A, vec<T>& x, vec<T>& b) {
    int N = A.size();
    int M = b.size();
    if (M != N)
        return false;
    int row, col;
    x.resize(N);
//#pragma omp parallel default(none) private(M,N,row,col) shared(A,x,b) num_threads(8)
    {
//#pragma omp for schedule(static)
        // Non-loop-carried
        for (row = 0; row < N; row++) {
            x(row) = b(row);
        }
        /**
         * Loop Analysis:
         * for i = lower, upper, stride; { a(i) }
         * --> for i’ = 0, (upper – lower + stride)/stride – 1, 1; {a(i’*stride + lower)}
         * let i' = _L1 ; j' = _L2 ; etc.
         * auto _UPPER_NORM = lambda [](int l, int u, int s)->int{return (u - l + s)/s - 1;};
         * auto _INDEX_NORM = lambda [](int l, int u, int s)->int{return lambda [](int idx){return idx*s+l;};};
         */
        int _L1 = 0;
        int _L1_N = ((N - 1)-(0)+(-1))/(-1)-1;
        auto _L1_i = [_L1]()->int{return ((-1)*_L1)+(0);};
        int _L2 = 0;
        int _L2_N = ((N - 1)-(0)+(-1))/(-1)-1;
        auto _L2_i = [_L1]()->int{return ((-1)*_L1)+(0);};
        for (col = N - 1; col >= 0; col--) {
            x(col) /= A(col, col);
            for (row = 0; row < col; row++) {
                x(row) -= A(row, col) * x(col);
            }
        }

//        for (j = N - 1; j >= 0; j--) {
//            x(j) /= A(j, j);
//            for (i = 0; i < j; i++) {
//                x(i) -= A(i, j) * x(j);
//            }
//        }
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
