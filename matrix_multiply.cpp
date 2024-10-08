#include <iostream>     // std::cerr
#include <iomanip>      // setfill, setw
#include <chrono>       // time it



#include "matrix_t.cpp"

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
    int exp_K = std::atoi(argv[2]);

    if(size_N < 0 || exp_K < 0) {
        std::cerr << "Arguments must be non-negative integers" << std::endl;
        usage(argv);
    }

    //Set up precision timer
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    // Start the clock!
    auto t1 = high_resolution_clock::now();

    /**
     * DO STUFF HERE
     */
    matrix<double_t> a,b,c,d,e;
    a.resize(size_N,1.0).diagonal(4);
    b.resize(size_N,-2).diagonal(9.0);
    a(0,0)=0.0;
    b(0,0)=0.0;
    c = a*b;

    // Stop the clock!
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;

    // Print results and net runtime of running <STUFF>
    std::cout << std::left
              << std::setfill(' ') << std::setw(8) << std::setprecision(2) << size_N
              << std::setfill(' ') << std::setw(8) << std::setprecision(2) << exp_K
              << std::setfill(' ') << std::setw(10) << std::setprecision(10) << ms_double.count() << std::endl;
    return EXIT_SUCCESS;
}