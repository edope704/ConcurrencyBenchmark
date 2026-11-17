#ifndef COMMON_H
#define COMMON_H

#include <sys/time.h>

#include <cstdint>
#include <ctime>
#include <random>
#include <vector>

#define MAX_PROCESSES 16
#define MAX_THREADS 16

#define ROWS 10000ul
#define COLS 10000ul
#define MAX_VAL 256

namespace common {

void fill_matrix( std::vector<uint16_t>& matrix ) {
  std::srand( std::time( nullptr ) );
  for ( auto& elem : matrix ) elem = std::rand() % MAX_VAL;
}

namespace benchmark {

struct benchmark_result {
    double wall_time_us{ 0.0 };
    double cpu_time_us{ 0.0 };
};

inline double timeval_to_us( const timeval& tv ) {
  return static_cast<double>( tv.tv_sec ) * 1e6 + static_cast<double>( tv.tv_usec );
}

inline double timespec_to_us( const timespec& ts ) {
  return static_cast<double>( ts.tv_sec ) * 1e6 + static_cast<double>( ts.tv_nsec ) / 1e3;
}

}  // namespace benchmark

}  // namespace common

#endif