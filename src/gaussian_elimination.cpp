#include <iostream>     // std::cerr
#include <iomanip>      // setfill, setw
#include <cstring>      // for strcmp on linux clang
#include <chrono>       // time it

#include <random>

#include "matrix_vec_shared_t.cpp"
#include "matrix_t.cpp"
#include "vec_t.cpp"


// Do we have our types?
#ifndef double_t /*implementation defined*/
#define double_t double
#endif


void usage(char* argv[]) {
    std::cerr << "Usage: \n"
              << argv[0] << " <size N> <exp K>\n"
              << argv[0] << " -h\n";
    std::exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {

    if((argc < 2)||!strcmp(argv[1],"-h")) {
        usage(argv);
    }

    int size_N = std::atoi(argv[1]);

    if(size_N < 0) {
        std::cerr << "Arguments must be non-negative integers" << std::endl;
        usage(argv);
    }

    //Set up precision timer
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    /* set up random number generator */
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double_t> uniform_double(-10, 10);
    std::uniform_int_distribution<int> uniform_selection(0, size_N - 1);



    /**
     * SETUP HERE
     */

    matrix<double_t> A; // Declare
    vec<double_t> b, x, y;    // Declare
    //uniform_double(engine)
    A.resize(3);
    //.upper(2.0);     // Modify: set upper triangle to V
    b.resize(A.size());

    A(0,0) = 2.0;   A(0,1) =-3.0;
                            A(1,1) = 1.0;   A(1,2) = 1.0;
                                                    A(2,2) = -5.0;

    b(0) = 3.0;
    b(1) = 1.0;
    b(2) = 0.0;

    std::cout<<"A="<<std::endl;A.show(); std::cout << std::endl << std::endl;
    std::cout<<"b="<<std::endl;b.show(); std::cout << std::endl << std::endl;

    // Start the clock!
    auto t1 = high_resolution_clock::now();

    /**
     * DO TIMED STUFF HERE
     */

    // Ax = b
    solve_c(A, x, b);
    //x.show(); std::cout << std::endl << std::endl;

    // Stop the clock!
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double_c = t2 - t1;

    /**
     * DO TIMED STUFF HERE
     */

    // Ax = b
    solve_r(A, y, b);

    // Stop the clock!
    auto t3 = high_resolution_clock::now();
    duration<double, std::milli> ms_double_r = t3 - t2;

    std::cout<<"x="<<std::endl; x.show(); std::cout << std::endl << std::endl;

    std::cout<<"y="<<std::endl; y.show(); std::cout << std::endl << std::endl;

    // Print results and net runtime of running <STUFF>
    std::cout << std::left<< std::setfill(' ')
    << "N="         << std::setw(8) << std::setprecision(4) << size_N
    //<< "Threads="   << std::setw(8) << std::setprecision(4) << c.thr
    << "solve_c="        << std::setw(8) << std::setprecision(4) << ms_double_c.count() << "ms "
    << "solve_r="        << std::setw(8) << std::setprecision(4) << ms_double_r.count() << "ms "
    << std::endl;

    return EXIT_SUCCESS;
}