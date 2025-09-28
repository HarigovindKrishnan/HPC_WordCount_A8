## 🔹 Project Overview
This project implements a parallel word count system using C++ and leveraging OpenMP in large text datasets. Word count is a common task in fields such as text analytics.

The workflow involves exploring multiple optimization methods such as different I/O approaches (pread and mmap)

## 🔹 Methodology
First a basic wordcount is implemented which is considered as the baseline. The optimised/parallelized versions of WordCOunt operations are compared to the baseline to obtain results and observe improvements.
1. Basic Parallelization using OMP:
   
   i) Normalize the text dataset by removing special characters except ' (apostrophes) in between words. THe cleaned text only contains alphabets, numbers, spaces       and apostrophes.

   ii) Each line is converted into a vector of strings with each word as each element in the vector.

   iii) Sequentially read the dataset into the code.

   iv) Create a vector of unordered maps. THe vector is of size n where n is the number of threads. This enables each thread to have its own local unordered map.

   v) In the parallel region, dynamic scheduling helps synamically assign chunks of lines of the text dataset to the threads to perform the WordCount operation and          store the results in their respective local unordered maps.

   vi) Each thread then atomically merges their WordCount to the global unordered map.

   To compile: g++ -O3 -fopenmp src/wordcount_omp.cpp -o results/wordcount_omp_dataset1

   To run: ./results/wordcount_omp_dataset1 data/sample.txt 20 > results/out_omp_dataset1.txt 2> results/time_omp_dataset1.txt


- Using **OpenMP** for shared-memory parallelism to parallely work on chunks of the text data.

---
## 🔹 Results:
- WordCount:
  
  i) Total number of words
  
  ii) Top 20 most frequent words
  
- Wall-clock time: elapsed time.
- Speedup: T1 / Tp (sequential time divided by parallel time).
