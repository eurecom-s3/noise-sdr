/*
 * Copyright 2020, Giovanni Camurati
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#ifdef LEAKY_OP_ION
#include "ion/ion.h"
#elif LEAKY_OP_STREAM
#include <emmintrin.h>
#elif LEAKY_OP_CNT
#define NSEC_PER_SEC 1000000000ull
typedef struct timespec mach_timespec_t;
typedef unsigned int mach_port_t;
#endif

#define BILLION 1000000000L
#define BUFSIZE 32000000
#define leak hammer_civac

// Hello message
// https://onlineasciitools.com/convert-text-to-ascii-art
void hello(void) {
    printf("#     #                               #####  ######  ###### \n"); 
    printf("##    #  ####  #  ####  ######       #     # #     # #     #\n");
    printf("# #   # #    # # #      #            #       #     # #     #\n");
    printf("#  #  # #    # #  ####  #####  #####  #####  #     # ###### \n");
    printf("#   # # #    # #      # #                  # #     # #   #  \n");
    printf("#    ## #    # # #    # #            #     # #     # #    # \n");
    printf("#     #  ####  #  ####  ######        #####  ######  #     #\n");
}

// Help message
void help(char *program_name) {
  fprintf(stderr, "Usage: %s -l filename : Transmit\n", program_name);
  fprintf(stderr, "          -m          : Measure basic op\n");
  fprintf(stderr, "          -u          : Synchronize FT4 to UTC time\n");
  fprintf(stderr, "          -f          : Run forever\n");
  // fprintf(stderr, "          -q cpu      : Pin to core\n");
}

// Timer function
static inline uint64_t get_ns(void) {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return BILLION * (uint64_t)t.tv_sec + (uint64_t)t.tv_nsec;
}

#ifdef LEAKY_OP_CIVAC
// Access the DRAM by LDR + CIVAC
__attribute__((naked)) void hammer_civac(uint64_t *addr) {
  __asm volatile("LDR X9, [X0] \r\n");
  __asm volatile("DC CIVAC, X0 \r\n");
  __asm volatile("DSB 0xB \r\n");
  __asm volatile("RET");
}
#elif LEAKY_OP_LOOP
void evict(uint32_t *addr, int len) {
    int u;
    for(int i=0; i<len; i+=4){
        u = *(volatile uint32_t*)(addr + i);
    }
}
#elif LEAKY_OP_ION
int ion_fd;
int len = 65536; //1024;
int chipset = -1;

// Access DRAM using ION
static inline void ion_leak(void) {
  ion_user_handle_t ion_handle;
  ion_alloc(ion_fd, len, 0, (0x1 << chipset), 0, &ion_handle);
  ion_free(ion_fd, ion_handle);
}
#elif LEAKY_OP_STREAM
// Access DRAM using mm stream
__m128i reg;
__m128i reg_zero;
__m128i reg_one;

void stream(void) {
  _mm_stream_si128(&reg, reg_one);
  _mm_stream_si128(&reg, reg_zero);
}
#elif LEAKY_OP_CNT
// system-bus-radio
static inline void clock_sleep_trap(mach_port_t clock_port, int sleep_type, time_t sec, long nsec, mach_timespec_t *remain) {
    mach_timespec_t req = { sec, nsec };
    int res = clock_nanosleep(sleep_type, CLOCK_MONOTONIC, &req, remain);
    if (res < 0) {
        perror("clock_nanosleep");
        exit(1);
    }
}
#endif

// Main program
int main(int argc, char **argv) {

  // Handle options
  // Partially taken from DRAMMER https://github.com/vusec/drammer
  int c;
  char *inputfile = NULL;
  bool m = false;
  bool sync = false;
  bool forever = false;
  // int cpu_pinning = -1;
  opterr = 0;
  while ((c = getopt(argc, argv, ":l:mfuq:")) != -1) {
    switch (c) {
    case 'l':
      inputfile = optarg;
      break;
    case 'm':
      m = true;
      break;
    case 'f':
      forever = true;
      break;
    case 'u':
      sync = true;
      break;
    // case 'q':
    // cpu_pinning = strtol(optarg, NULL, 10);
    // break;
    case '?':
      if (optopt == 'l' || optopt == 'u') // || optopt == 'q')
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      help(argv[0]);
      return 1;
    default:
      abort();
    }
  }

  if (inputfile == NULL) {
    help(argv[0]);
    return 1;
  }

  // Nice name
  hello();

  // Cpu pinning (taken from DRAMMER)
  // https://github.com/vusec/drammer
  // if (cpu_pinning != -1) {
  //     printf("Pin to CPU %d\n", cpu_pinning);
  //     cpu_set_t cpuset;
  //     CPU_ZERO(&cpuset);
  //     CPU_SET(cpu_pinning, &cpuset);
  //     if (sched_setaffinity(0, sizeof(cpuset), &cpuset)) {
  //         perror("Could not pin CPU\n");
  //     }
  // }
//
//  // Address to hammer for the basic operation
//  uint64_t variable = ~0;
//  uint64_t *address = &variable;
//
//  // Prepare by removing address from the cache
//  prepare(address);
//

  // read edges from file and place them in a static array
  // hopefully it will go in the stack in cache
  // anyhow, accessing the array should be faster than looping over the file
  // stream
  printf("Loading file\n");
  std::ifstream infile(inputfile);
  int th_ns, t_ns;
  static uint64_t edges[BUFSIZE] = {0};
  int len = 0;
  while (infile >> th_ns >> t_ns and len < BUFSIZE) {
    edges[len++] = th_ns;
    edges[len++] = t_ns;
  }
  printf("Buffer size = %d\n", BUFSIZE);
  printf("File length = %d\n", len);
  infile.close();

  #ifdef LEAKY_OP_CIVAC
    // Address to hammer for the basic operation
    uint64_t variable = ~0;
    uint64_t *address = &variable;
  #elif LEAKY_OP_LOOP
    uint32_t address[512];
  #elif LEAKY_OP_ION
    ion_fd = ion_open();
    chipset = ion_magic_number(ion_fd, len);
    if (chipset == -1) {
      fprintf(stderr, "Chipset not found\n");
      abort();
    }
  #elif LEAKY_OP_CNT
    volatile uint32_t cnt = 0;
    mach_port_t clock_port;
    mach_timespec_t remain;
  #endif

  // Measure basic op
  if (m){
      printf("Start measuring basic op\n");
      int times = 100;
      uint64_t start = get_ns();
      for (int i = 0; i < times; i++) {
      // basic operation
#ifdef LEAKY_OP_CIVAC
        hammer_civac(address);
#elif LEAKY_OP_LOOP
        evict(address, 512);
#elif LEAKY_OP_ION
        ion_leak();
#elif LEAKY_OP_STREAM
        stream();
#elif LEAKY_OP_CNT
        cnt++;
#endif
      }
      uint64_t stop = get_ns();
      printf("Done\n");
      printf("Basic operation duration  = %lu ns\n", (stop - start) / times);
  }

  printf("Start transmission with RF-PWM\n");

  do {
    // synchronize FT4 with UTC
    if (sync) {
      time_t rawtime;
      struct tm *ptm;
      do {
        time(&rawtime);
        ptm = gmtime(&rawtime);
      } while ((ptm->tm_sec * 10) % 75 != 0);
    }

    // generate a squarewave following the edges in the file
    int i = 0;
    uint64_t mid, reset;
    uint64_t start = get_ns();
    while (i < len) {
      mid = start + edges[i++];
      reset = start + edges[i++];

      // High period
      while (get_ns() < mid) {
// basic operation
#ifdef LEAKY_OP_CIVAC
        hammer_civac(address);
#elif LEAKY_OP_LOOP
        evict(address, 512);
#elif LEAKY_OP_ION
        ion_leak();
#elif LEAKY_OP_STREAM
        stream();
#elif LEAKY_OP_CNT
        cnt++;
#endif
      }
      // Low period
#ifdef LEAKY_OP_CNT
      clock_sleep_trap(clock_port, CLOCK_MONOTONIC, reset / NSEC_PER_SEC, reset % NSEC_PER_SEC, &remain);
#else
      while (get_ns() < reset) {
      };
#endif
      // Reset the staring point, using reset gives the better result
      // start = high_resolution_clock::now();
      start = reset;
    }
  } while (forever);

#ifdef LEAKY_OP_ION
  ion_close(ion_fd);
#endif
}
