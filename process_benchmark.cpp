#include <sys/resource.h>
#include <sys/types.h>
#include <wait.h>

#include <array>
#include <boost/interprocess/anonymous_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <print>
#include <span>
#include <vector>

#define MAX_PROCESSES 16
#define ROWS 10000ul
#define COLS 10000ul
#define MAX_VAL 256

using matrix_t = std::vector<int16_t>;

struct benchmark_result {
    double wall_time_us{ 0.0 };
    double cpu_time_us{ 0.0 };
};

/* Arguments for each child process */
struct args {
    uint16_t start_row;
    uint16_t n_rows;
    int64_t partial_sum;
};
std::span<args> process_args;

std::vector<int16_t> big_matrix;

int16_t curr_process_index;

static void fill_matrix() {
  std::srand( std::time( nullptr ) );
  for ( auto& elem : big_matrix ) elem = std::rand() % MAX_VAL;
}

static void process_routine() {
  auto start_index = process_args[ curr_process_index ].start_row * COLS;
  auto count = process_args[ curr_process_index ].n_rows * COLS;
  auto sum = std::accumulate( big_matrix.begin() + start_index,
                              big_matrix.begin() + start_index + count, int64_t{ 0 } );
  process_args[ curr_process_index ].partial_sum = sum;
}

static benchmark_result benchmark_routine( int16_t num_processes ) {
  int16_t n_procs{ num_processes };
  int64_t total_sum{ 0 };
  int32_t rows_per_proc, last_proc_rows{ 0 };
  std::array<pid_t, MAX_PROCESSES> pids;

  /* setup shared memory region*/
  auto shm_region = [ & ]() {
    namespace bip = boost::interprocess;
    bip::mapped_region region( bip::anonymous_shared_memory( sizeof( args ) * MAX_PROCESSES ) );
    process_args = std::span<args>( static_cast<args*>( region.get_address() ), MAX_PROCESSES );
    return region;
  }();

  /* setup process_args */
  rows_per_proc = ROWS / n_procs;
  last_proc_rows = rows_per_proc + ROWS % n_procs;

  for ( auto i{ 0uz }; i < n_procs; i++ ) {
    process_args[ i ].start_row = i * rows_per_proc;
    if ( i == n_procs - 1 )
      process_args[ i ].n_rows = last_proc_rows;
    else
      process_args[ i ].n_rows = rows_per_proc;
  }

  /* Start measuring wall time and CPU time */
  rusage ru_children_start, ru_children_end;
  rusage ru_self_start, ru_self_end;
  getrusage( RUSAGE_CHILDREN, &ru_children_start );
  getrusage( RUSAGE_SELF, &ru_self_start );
  auto wall_start = std::chrono::steady_clock::now();

  /* Calculate partial sums */
  for ( curr_process_index = 0; curr_process_index < n_procs; curr_process_index++ ) {
    pids[ curr_process_index ] = fork();
    if ( pids[ curr_process_index ] == 0 ) {
      process_routine();
      std::_Exit( 0 );
    }
  }

  /* Wait for process_routine() end for each thread and calculate total_sum */
  for ( curr_process_index = 0; curr_process_index < n_procs; curr_process_index++ ) {
    waitpid( pids[ curr_process_index ], NULL, 0 );
    total_sum += process_args[ curr_process_index ].partial_sum;
  }

  /* Stop measuring wall time and CPU time */
  getrusage( RUSAGE_CHILDREN, &ru_children_end );
  getrusage( RUSAGE_SELF, &ru_self_end );
  auto wall_end = std::chrono::steady_clock::now();

  auto timeval_to_us = []( const timeval& tv ) {
    return static_cast<double>( tv.tv_sec ) * 1e6 + static_cast<double>( tv.tv_usec );
  };

  double wall_time_us = std::chrono::duration<double, std::micro>( wall_end - wall_start ).count();

  double children_cpu_us =
      ( timeval_to_us( ru_children_end.ru_utime ) - timeval_to_us( ru_children_start.ru_utime ) ) +
      ( timeval_to_us( ru_children_end.ru_stime ) - timeval_to_us( ru_children_start.ru_stime ) );

  double parent_cpu_us =
      ( timeval_to_us( ru_self_end.ru_utime ) - timeval_to_us( ru_self_start.ru_utime ) ) +
      ( timeval_to_us( ru_self_end.ru_stime ) - timeval_to_us( ru_self_start.ru_stime ) );

  double total_cpu_us = children_cpu_us + parent_cpu_us;

  return { wall_time_us, total_cpu_us };
}

int main() {
  /* setup matrix */
  big_matrix.resize( ROWS * COLS );
  fill_matrix();
  std::ofstream bm( "process_benchmark.csv" );
  if ( !bm.is_open() ) {
    std::printf( "ERROR: Unable to open 'process_benchmark.csv'\n" );
    std::exit;
  }
  std::println( bm, "nProcesses,clock_realtime,clock_cputime" );

  /* Run benchmark */
  for ( auto num_procs{ 1uz }; num_procs <= MAX_PROCESSES; num_procs++ ) {
    auto benchmark_res = benchmark_routine( num_procs );
    std::printf( "%zu,%f,%f\n", num_procs, benchmark_res.wall_time_us, benchmark_res.cpu_time_us );
    std::println( bm, "{},{},{}", num_procs, benchmark_res.wall_time_us,
                  benchmark_res.cpu_time_us );
  }

  bm.close();
  return 0;
}
