#include <iostream>     // std::cerr
#include <iomanip>      // setfill, setw
#include <cstring>      // for strcmp on linux clang
#include <chrono>       // time it

#include <random>

#include "matrix_t.cpp"

#include "omp.h"

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

    matrix<double_t> a,b,c; // Declare
    a.resize(size_N, uniform_double(engine))        // Initialize
    .diagonal(uniform_double(engine));                  // Modify: set the diagonals to V
    b.resize(size_N,1)                                  // Initialize
    .upper(uniform_double(engine));                     // Modify: set upper triangle to V
    a(uniform_selection(engine),uniform_selection(engine))
    =uniform_double(engine);                                // Modify: set a random entry to V
    //b(uniform_selection(engine),uniform_selection(engine))=uniform_double(engine);

    //b.show();

    // Start the clock!
    auto t1 = high_resolution_clock::now();

    /**
     * DO TIMED STUFF HERE
     */
    c = (a*b);
    c(size_N-1,size_N-1) = (double)1.0;

    // Stop the clock!
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;

    //c.show(); std::cout << std::endl << std::endl;

    // Print results and net runtime of running <STUFF>
    std::cout << std::left<< std::setfill(' ')
    << "N="         << std::setw(8) << std::setprecision(4) << size_N
    //<< "Threads="   << std::setw(8) << std::setprecision(4) << c.thr
    << "dt="        << std::setw(8) << std::setprecision(4) << ms_double.count() << "ms"
    << std::endl;

    return EXIT_SUCCESS;
}