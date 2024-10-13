#ifndef INC_3_ACCESS_PATTERN_HPP
#define INC_3_ACCESS_PATTERN_HPP

#include <vector>
#include <cstring>
#include <iostream>     // std::cerr etc.

// Why two names for a typedef? If you
// want to make a forward declaration,
// you have to give it a name in the
// tag namespace.
typedef enum access_t {
    NONE, R, W, RW
} access_t;
typedef struct access_record {
    access_t type;
    access_record * prior;
    int thread_id;
    access_record():type(NONE),prior(nullptr),thread_id(0){};
    access_record(access_t op):type(op), prior(nullptr), thread_id(0){};
    access_record(access_t op, access_record* p):type(op), prior(p), thread_id(0){};
    access_record(access_t op, access_record* p, int t):type(op), prior(p), thread_id(t){};
} access_record;
typedef struct access_pattern {
//    std::vector<access_record *> first_indices;
    std::vector<access_record *> last_indices;

    void update(access_t action, int access_at_index, int depends_on_index = -1) { // x(index) depends
        access_record * target = nullptr;
        if (depends_on_index >= 0) {
            target = last_indices.at(depends_on_index);
        }
        access_record * record = new access_record(action, target);
        if (access_at_index >= (int)last_indices.size())
            last_indices.resize(access_at_index + 1);
        last_indices.at(access_at_index) = (access_record *) record;
    };

    access_pattern() = default;
    access_pattern(const access_pattern& other) = default;
    access_pattern(access_pattern&& other) = default;
    access_pattern& operator=(const access_pattern& other) = default;
    access_pattern& operator=(access_pattern&& other) = default;
    ~access_pattern() = default; // Default destructor?
} access_pattern;

void print_access_records(const std::string& prefix, const access_record* record)
{
    if(record != nullptr )
    {
        // print the value of the record
        std::cout << prefix << "(" << record->type << ")";
        if (record->prior != nullptr) {
            print_access_records("──>", record->prior);
        } else {
            std::cout << std::endl;
//            prefix + (last ? "│  " : "   ")
        }
    }
}

void print_access_pattern(access_pattern* pattern)
{
    std::cout << "idx [READ=1, WRITE=2]" << std::endl;
    for (int i = (int)pattern->last_indices.size(); --i>=0; ) {
        std::cout << i;
        std::cout << (i ? "├──" : "└──" );
        print_access_records("", pattern->last_indices.at(i));
    }
    std::cout << std::endl;
//    print_access_pattern("", pattern, false);
}


#endif //INC_3_ACCESS_PATTERN_HPP
