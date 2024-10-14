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
        auto _norm_fn_idx = [](int begin, int stride, int end){return [begin,stride](int idx){return idx * stride + begin;};};
        auto _norm_N = [](int begin, int stride, int end)->int{return (end - begin + stride) / stride - 1;};
        //auto INDEX_NORM = [](int l, int s, int i){return i*s+l;};

//#pragma omp for schedule(static)
        // Non-loop-carried
        for (row = 0; row < A_M; row++) {
            x(row) = b(row);
        }

        // fn. such that j(L1) = j = col
        auto j = _norm_fn_idx(A_M-1, -1, -1);
        int L1;
        int L1N = _norm_N(A_M-1, -1, -1);
        // for (L1 : 0 .. L1N)

        for (L1 = 0; L1 < L1N; L1++) {
            col = j(L1);
            T val_x_L1 = x(col); // READ
            T val_A_L1_L1 = (A(col, col)); // READ
            x(col) = val_x_L1 / val_A_L1_L1; // WRITE
            // x(col) /= A(col, col);

            // fn. such that i(L2) = i = row
            auto i = _norm_fn_idx(0, 1, j(L1));
            int L2;
            int L2N = _norm_N(0, 1, j(L1)); // L2N = {0 .. L1-1}
            for (L2 = 0; L2 < L2N; L2++) {
                row = i(L2);
                T val_x_L2 = x(row); // READ
                T val_A_L2_L1 = (A(row, col)); // READ
                T val_x_L1_shadow = x(col); // READ SHADOWS OUTER
                T mul_A_L2_L1_val_x_L1 = val_A_L2_L1 * val_x_L1_shadow; // MUL
                x(row) = val_x_L2 - mul_A_L2_L1_val_x_L1; // WRITE
                // x(row) -= A(row, col) * x(col);
            }
        }

        /**
        for (row = 0; row < n; row++)
            x[row] = b[row];

        for (col = n-1; col >= 0; col--) {
            x[col] /= A[col][col];
            for (row = 0; row < col; row++)
                x[row] -= A[row][col] * x[col];
        }
        */

        /**
        for (row = 0; row < n; row++)
            x[row] = b[row];
        for (col = n-1; col >= 0; col--) {
            x[col] /= A[col][col];
            for (row = 0; row < col; row++)
                x[row] -= A[row][col] * x[col];
        }
        */

    }
    return true;
}


template <typename T>
bool solve_r(matrix<T>& A,vec<T>& x, vec<T>& b) {
    int A_M = A.size(); // A_M = width(M)
    int b_N = b.size(); // b_N = |b|
    if (b_N != A_M)
        return false;
    int row, col;
    x.resize(A_M); // such that |x| = |b| and |x| = width(M)
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
        auto _norm_fn_idx = [](int begin, int stride, int end){return [begin,stride](int idx){return idx * stride + begin;};};
        auto _norm_N = [](int begin, int stride, int end)->int{return (end - begin + stride) / stride - 1;};

        // fn. such that j(L1) = j = row;   L1 = j'(row)
        auto j = _norm_fn_idx(A_M-1, -1, -1);
        int L1;
        int L1N = _norm_N(A_M-1, -1, -1);
        // for (L1 : 0 .. L1N)

        for (L1 = 0; L1 < L1N; L1++) {
            int view_j_L1 = j(L1);
            T val_b_L1 = b(view_j_L1); // READ
            x(j(L1)) = val_b_L1; // WRITE

            // fn. such that i(L2) = i = col; L2 = i'(col)
            auto i = _norm_fn_idx(j(L1)+1, 1, A_M);
            int L2;
            int L2N = _norm_N(j(L1)+1, 1, A_M);

            for (L2 = 0; L2 < L2N; L2++) {
                int view_i_L2 = i(L2);
                x(j(L1)) -= A(j(L1), i(L2)) * x(view_i_L2);
            }
            x(j(L1)) /= A(j(L1), j(L1));
        }

        /**
        for (L1 = 0; L1 < L1N; L1++) {
            x(j(L1)) = b(j(L1));
            for (L2 = 0; L2 < L2N; L2++)
                x(j(L1)) -= A(j(L1),i(L2)) * x(i(L2));
            x(j(L1)) /= A(j(L1),j(L1));
        }
         */

        /**
        for (L1 = 0; L1 < L1N; L1++) {
            x(j(L1)) = b(j(L1));
            for (L2 = 0; L2 < L2N; L2++)
                x(j(L1)) -= A(j(L1),i(L2)) * x(i(L2));
            x(j(L1)) /= A(j(L1),j(L1));
        }
         */

        /**
        for (L1 = j'(n-1); L1 < j'(-1); L1++) {
            x(j(L1)) = b(j(L1));
            for (L2 = i'(j(L1)+1); L2 < i'(n); L2++)
                x(j(L1)) -= A(j(L1),i(L2)) * x(i(L2));
            x(j(L1)) /= A(j(L1),j(L1));
        }
         */

        /**
        for (j(L1) = n-1; j(L1) > -1; j(L1)--) {
            x(j(L1)) = b(j(L1));
            for (i(L2) = j(L1)+1; i(L2) < n; i(L2)++)
                x(j(L1)) -= A(j(L1),i(L2)) * x(i(L2));
            x(j(L1)) /= A(j(L1),j(L1));
        }
         */

        /**
        for (j = n-1; j >= 0; j--) {
            x[j] = b[j];
            for (i = j+1; i < n; i++)
                x[j] -= A[j][i] * x[i];
            x[j] /= A[j][j];
        }
         */

        /**
        for (j = n-1; j >= 0; j--) {
            x[j] = b[j];
            for (i = j+1; i < n; i++)
                x[j] -= A[j][i] * x[i];
            x[j] /= A[j][j];
        }
         */

        /**
        for (row = n-1; row >= 0; row--) {
            x[row] = b[row];
            for (col = row+1; col < n; col++)
                x[row] -= A[row][col] * x[col];
            x[row] /= A[row][row];
        }
         */

    }
    return true;
}
