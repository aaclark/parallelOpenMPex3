#include <iostream>     // std::cerr
#include <iomanip>      // setfill, setw
#include <chrono>       // time it


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
    int tracked_variable_2 = std::atoi(argv[2]);

    if(size_N <= 0 || tracked_variable_2 <= 0) {
        std::cerr << "Arguments must be positive integers" << std::endl;
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

    // Stop the clock!
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;

    // Print results and net runtime of running <STUFF>
    std::cout << std::left
        << std::setfill(' ') << std::setw(10) << std::setprecision(10) << ms_double.count() << std::endl;
    return EXIT_SUCCESS;
}