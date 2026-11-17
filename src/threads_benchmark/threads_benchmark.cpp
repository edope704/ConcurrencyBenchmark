#include <sys/types.h>
#include <wait.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <print>
#include <span>
#include <thread>
#include <vector>

#include "common.hpp"

namespace bm = common::benchmark;

/* Arguments for each child process */
struct args {
    uint16_t start_row;
    uint16_t n_rows;
    int64_t partial_sum;
};
std::array<args, MAX_THREADS> thread_args;

std::vector<uint16_t> big_matrix;

static void thread_routine( args& curr_args ) {
  auto start_index = curr_args.start_row * COLS;
  auto count = curr_args.n_rows * COLS;
  auto sum = std::accumulate( big_matrix.begin() + start_index,
                              big_matrix.begin() + start_index + count, int64_t{ 0 } );
  curr_args.partial_sum = sum;
}

static bm::benchmark_result benchmark_routine( uint16_t n_threads ) {
  int64_t total_sum{ 0 };
  uint16_t rows_per_thread, last_thread_rows{ 0 };
  std::array<std::thread, MAX_THREADS> threads;

  rows_per_thread = ROWS / n_threads;
  last_thread_rows = rows_per_thread + ROWS % n_threads;

  /* Setup thread_args */
  for ( auto i{ 0uz }; i < n_threads; i++ ) {
    thread_args[ i ].start_row = i * rows_per_thread;
    if ( i == n_threads - 1 )
      thread_args[ i ].n_rows = last_thread_rows;
    else
      thread_args[ i ].n_rows = rows_per_thread;
  }

  /* Start measuring wall time and CPU time */
  timespec wall_time_start, wall_time_end;
  timespec cpu_time_start, cpu_time_end;
  clock_gettime( CLOCK_REALTIME, &wall_time_start );
  clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &cpu_time_start );

  /**** Benchmark Starts Here *****/
  for ( auto i{ 0uz }; i < n_threads; i++ ) {
    threads.at( i ) = std::thread( thread_routine, std::ref( thread_args.at( i ) ) );
  }
  for ( auto i{ 0uz }; i < n_threads; i++ ) {
    threads.at( i ).join();
    total_sum += thread_args.at( i ).partial_sum;
  }
  /**** Benchmark Ends Here *****/

  /* Stop measuring wall time and cpu time */
  clock_gettime( CLOCK_REALTIME, &wall_time_end );
  clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &cpu_time_end );

  auto wall_time_us = bm::timespec_to_us( wall_time_end ) - bm::timespec_to_us( wall_time_start );
  auto cpu_time_us = bm::timespec_to_us( cpu_time_end ) - bm::timespec_to_us( cpu_time_start );

  return { wall_time_us, cpu_time_us };
}

int main() {
  big_matrix.resize( ROWS * COLS );
  common::fill_matrix( big_matrix );

  std::ofstream bm( "threads_benchmark.csv" );
  if ( !bm.is_open() ) {
    std::printf( "ERROR: Unable to open 'threads_benchmark.csv'\n" );
    std::exit( 1 );
  }
  std::println( bm, "nThreads,clock_realtime,clock_cputime" );
  std::printf( "nThreads,clock_realtime,clock_cputime\n" );

  /* Run benchmark */
  for ( auto num_threads{ 1uz }; num_threads <= MAX_THREADS; num_threads++ ) {
    auto benchmark_res = benchmark_routine( num_threads );
    std::printf( "%zu,%f,%f\n", num_threads, benchmark_res.wall_time_us,
                 benchmark_res.cpu_time_us );
    std::println( bm, "{},{},{}", num_threads, benchmark_res.wall_time_us,
                  benchmark_res.cpu_time_us );
  }

  bm.close();
  return 0;
}