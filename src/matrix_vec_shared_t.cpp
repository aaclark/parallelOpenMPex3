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


        // fn. such that j(_1) = j
        auto j = _norm_fn_idx(0,-1,N);
        int _1;
        int _1N = _norm_N(0,-1,N);
        for (_1 = 0; _1 < _1N; _1++) {
            x(j(_1)) /= A(j(_1), j(_1));

            // fn. such that i(_2) = i
            auto i = _norm_fn_idx(0,1,j(_1));
            int _2;
            int _2N = _norm_N(0,1,j(_1));
            for (_2 = 0; _2 < _2N; _2++) {
                x(i(_2)) -= A(i(_2), j(_1)) * x(j(_1));
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
