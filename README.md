#  Thread Pinning WordCount (OpenMP + pthread)

A **parallel WordCount** implementation using **OpenMP** combined with **pthread-based CPU affinity (thread pinning)**.  
This optimization improves cache locality and minimizes thread migration overhead, leading to faster and more stable performance compared to a regular OpenMP WordCount.

---

##  Methodology

1. **Data Preprocessing:**  
   The input text is cleaned by removing unnecessary special characters, keeping only alphabets, digits, spaces, and valid apostrophes.  
   Each line is then tokenized into individual words for efficient processing.

2. **Dataset Loading:**  
   The text file is read sequentially into memory to ensure consistent ordering and easy partitioning for threads.

3. **Parallel Word Counting:**  
   - Each thread maintains its own **unordered map** to count words locally.  
   - This avoids lock contention between threads and enables independent processing.  
   - **Dynamic scheduling** is used to balance work between threads based on availability.

4. **Thread Pinning:**  
   Each OpenMP thread is pinned to a specific CPU core using the `pthread_setaffinity_np()` function.  
   This ensures that threads remain fixed to one core, reducing cache misses and preventing thread migration.

5. **Result Merging:**  
   After all threads complete their local counting, the individual maps are merged into a **global word count map**, producing the final result efficiently.

---

##  Compilation

```bash
g++ -O3 -fopenmp -lpthread src/wordcount_pinned.cpp -o results/wordcount_pinned_dataset1
```

---

##  Execution

```bash
./results/wordcount_pinned_dataset1 data/sample.txt 20 > results/out_pinned_dataset1.txt 2> results/time_pinned_dataset1.txt
```

---

##  Dataset and Results

**Dataset 2 (102 MB):**
i) Elapsed time:
  Avg Execution time (across 3 runs):
  -  1.238 s [4 Threads]
  -  1.568 s [2 Threads]
    
  ii) Speedup: 
  -  1.987 (4 threads)
  -  1.569 (2 threads)
    
  iii) Efficiency:
  -  49.6% (4 threads)
  -  78.45%   (2 threads)

**Conclusion:**  
Thread pinning using pthread affinity enhances cache efficiency and CPU utilization, resulting in smoother, more predictable parallel performance.

---


