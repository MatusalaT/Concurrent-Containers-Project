
# WRITEUP.md

  

## Experimental Results

  

I conducted performance testing on several concurrent data structures, including variations of stacks and queues, across a multithreaded environment. Below are the experimental results:

  

### Test Results

| Data Structure | Threads | Operations | Time (ms) |

|----------------------------------|---------|------------|-----------|

| SGLEliminationStack | 3 | 4000 | 2 |

| TreiberEliminationStack | 3 | 4000 | 1 |

| FlatCombiningEliminationStack | 3 | 4000 | 1 |

| SGLQueue | 3 | 4000 | 3 |

| MSQueue | 3 | 4000 | 6 |

| FlatCombiningQueue | 3 | 4000 | 2 |

  

### Performance Counter Results

Using `perf`, I measured the following metrics:

  

```plaintext

Performance counter stats for './concurrent_containers 3 4000':

  

48.70 msec task-clock # 1.260 CPUs utilized

257 context-switches # 0.005 M/sec

0 cpu-migrations # 0.000 K/sec

420 page-faults # 0.009 M/sec

148,059,027 cycles # 3.040 GHz

75,756,833 instructions # 0.51 insn per cycle

12,045,498 branches # 247.327 M/sec

133,771 branch-misses # 1.11% of all branches

  

0.038654615 seconds time elapsed

  

0.022179000 seconds user

0.044358000 seconds sys

```

  

Additional `perf` statistics captured:

  

```plaintext

Performance counter stats for './concurrent_containers 3 4000':

  

710,783 cache-references

269,043 cache-misses # 37.852 % of all cache refs

  

0.037199002 seconds time elapsed

  

0.031039000 seconds user

0.031039000 seconds sys

```

  

### Analysis of Results

1.  **Throughput Analysis:**

- The `TreiberEliminationStack` and `FlatCombiningEliminationStack` had the best performance for stack operations, completing in just 1 ms.

- The `MSQueue` was the slowest queue, taking 6 ms to process 4000 operations, highlighting the overhead of its lock-free implementation.

- The `FlatCombiningQueue` improved significantly over `SGLQueue` by reducing contention through batch processing.

  

2.  **Perf Metrics:**

- A low instruction-per-cycle (IPC) ratio of 0.51 indicates potential stalls or memory access bottlenecks.

- Cache performance was efficient with only 1.11% branch mispredictions.

- Context switches were minimal, suggesting good thread utilization without excessive contention.

- Cache misses were significant at 37.85% of cache references, indicating room for improvement in data locality.

  

## Code Organization

The implementation is divided into several reusable classes, each representing a concurrent data structure. Key components include:

  

-  **SGLQueue:** Implements a simple queue using a mutex for synchronization.

-  **MSQueue:** A lock-free queue using atomic operations.

-  **FlatCombiningQueue:** Optimizes synchronization by batching operations.

-  **SGLEliminationStack:** A stack with a mutex-based synchronization strategy.

-  **TreiberEliminationStack:** A lock-free stack using atomic operations.

-  **FlatCombiningEliminationStack:** A stack with batched operation combining.

  

## Description of Submitted Files

-  **`main.cpp`:** Contains all implementations of stacks and queues, along with test functions.

-  **`Makefile`:** Provides build instructions for the project.

-  **`README.md`:** Explains usage and description of the program.

-  **`test_containers`:** Test binary for running the experiments.

-  **`autograde.sh`:** Script for automated grading and testing.

-  **`concurrent_containers`:** Compiled executable for the project.

  

## Compilation Instructions

To compile the program, use the provided `Makefile`:

  

```bash

make

```

  

This will generate the executable `concurrent_containers`.

  

## Execution Instructions

To run the program and collect results, use the following syntax:

  

```bash

./concurrent_containers <num_threads> <num_operations>

```

  

For example, to run with 3 threads and 4000 operations:

  

```bash

./concurrent_containers 3 4000

```



To collect performance metrics using `perf`:

  

```bash

perf stat ./concurrent_containers 3 4000

```

  

## Extant Bugs

-  **Memory Leaks:** Memory is not properly deallocated in some implementations, as permitted by the prompt.

-  **Scalability:** Performance may degrade with a very high thread count due to increased contention and cache coherence overhead.