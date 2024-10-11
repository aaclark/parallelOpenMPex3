#include "matrix_vec_shared_t.hpp"

template <typename T>
bool solve_c(matrix<T>& A, vec<T>& x, vec<T>& b) {
    int A_M = A.size(); // A_M = width(M)
    int b_N = b.size(); // b_N = |b|
    if (b_N != A_M)
        return false;
    int row, col;
    x.resize(A_M); // such that |x| = |b| and |x| = width(M)
//#pragma omp parallel default(none) private(b_N,A_M,row,col) shared(A,x,b) num_threads(8)
    {

        /**
         * Loop Analysis:
         * In general it is always possible to normalize a loop specification from:
         *      for (i = begin, (i<) end, (i+=) stride) { a(i) }
         * To:
         *      for (i’ = 0, (end – begin + stride)/stride – 1, 1) {a(i’*stride + begin)}
         * Such that for each loop depth we have:
         *      for (L1 : 0 .. M)
         *          for (L2 : 0 .. N)
         *              ...
         *
         * auto _norm_N = [](int begin, int stride, int end)->int{return (end - begin + stride) / stride - 1;};
         * auto _norm_fn_idx = [](int begin, int stride, int end){return [begin,stride](int idx){return idx * stride + begin;};};
        */
        auto _norm_N = [](int begin, int stride, int end)->int{return (end - begin + stride) / stride - 1;};
        auto _norm_fn_idx = [](int begin, int stride, int end){return [begin,stride](int idx){return idx * stride + begin;};};
        //auto INDEX_NORM = [](int l, int s, int i){return i*s+l;};
        
//#pragma omp for schedule(static)
        // Non-loop-carried
        for (row = 0; row < A_M; row++) {
            x(row) = b(row);
        }

        // fn. such that j(L1) = j
        auto j = _norm_fn_idx(A_M-1, -1, -1);
        int L1;
        int L1N = _norm_N(A_M-1, -1, -1);
        // for (L1 : 0 .. L1N)

        for (L1 = 0; L1 < L1N; L1++) {
            T val_x_L1 = x(j(L1)); // READ
            T val_A_L1_L1 = (A(j(L1), j(L1))); // READ
            x(j(L1)) = val_x_L1 / val_A_L1_L1; // WRITE
//            x(j(L1)) /= A(j(L1), j(L1));

            // fn. such that i(L2) = i
            auto i = _norm_fn_idx(0, 1, j(L1));
            int L2;
            int L2N = _norm_N(0, 1, j(L1)); // L2N = {0 .. L1 -1}
            for (L2 = 0; L2 < L2N; L2++) {
                T val_x_L2 = x(i(L2)); // READ
                T val_A_L2_L1 = (A(i(L2), j(L1))); // READ
                T val_x_L1_ = x(j(L1)); // READ SHADOWS OUTER
                T mul_A_L2_L1_val_x_L1 = val_A_L2_L1 * val_x_L1; // MUL
                x(i(L2)) = val_x_L2 - mul_A_L2_L1_val_x_L1; // WRITE
//                x(i(L2)) -= A(i(L2), j(L1)) * x(j(L1));
            }
        }

//        for (j = A_M - 1; j >= 0; j--) {
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
