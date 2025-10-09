First a basic wordcount is implemented which is considered as the baseline. The optimised/parallelized versions of WordCOunt operations are compared to the baseline to obtain results and observe improvements.
The following is the OMP + ifstream paralellized approach.

## 🔹 Methodology

Basic Parallelization using OMP:

   i) Normalize the text dataset by removing special characters except ' (apostrophes) in between words. THe cleaned text only contains alphabets, numbers, spaces       and apostrophes.

   ii) Each line is converted into a vector of strings with each word as each element in the vector.

   iii) Sequentially read the dataset into the code.

   iv) Create a vector of unordered maps. THe vector is of size n where n is the number of threads. This enables each thread to have its own local unordered map.

   v) In the parallel region, dynamic scheduling helps synamically assign chunks of lines of the text dataset to the threads to perform the WordCount operation and          store the results in their respective local unordered maps.

   vi) Each thread then atomically merges their WordCount to the global unordered map.

To compile:
```bash
g++ -O3 -fopenmp src/wordcount_omp.cpp -o results/wordcount_omp_dataset1
```
To run: 
```bash
./results/wordcount_omp_dataset1 data/sample.txt 20 > results/out_omp_dataset1.txt 2> results/time_omp_dataset1.txt
```
   
- Using **OpenMP** for shared-memory parallelism to parallely work on chunks of the text data.

---
## 🔹 Results:
  ## Dataset 1 (5.1 Mb):
```bash
  i) Total tokens: 989195

  ii) Unique words: 24876

  iii) Elapsed time:
  Avg Execution time (across 5 runs):
  -  0.137 s [4 Threads]
  -  0.155 s [2 THreads]
    
  iv) Speedup: 
  -  1.116 (4 threads)
  -  0.98 (2 threads)
    
  v) Efficiency:
  -  27.9% (4 threads)
  -  49%   (2 threads)
  ```


  ## Dataset 2 (102 Mb):
   ```bash
  i) Total tokens: 16603612

  ii) Unique words: 250323

  iii) Elapsed time:
  Avg Execution time (across 3 runs):
  -  1.279 s [4 Threads]
  -  1.611 s [2 THreads]
    
  iv) Speedup: 
  -  1.924 (4 threads)
  -  1.527 (2 threads)
    
  v) Efficiency:
  -  48.1% (4 threads)
  -  76.35%   (2 threads)
   ```

