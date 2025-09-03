## 🔹 Project Overview
This project implements a parallel word count system using C++ and leveraging OpenMP in large text datasets. Word count is a common task in fields such as text analytics.

The workflow involves exploring multiple optimization methods such as different I/O approaches (pread and mmap)

## 🔹 Methodology
1. Chunk size = filesize / num_threads.

2. Use pread (explicit chunks) or mmap (array-like access).

3. Fix split words at edges.

4. Each thread finds words in its chunk.

5. Each thread builds its own unordered_map.

6. Combine thread-local results into a global map.
   
7. Compare performance with baseline methods (without parallelization) as well as approaches such as Hadoop MapReduce 

- Using **OpenMP** for shared-memory parallelism to parallely work on chunks of the text data.

---
## 🔹 Evaluation Metrics:
Wall-clock time: elapsed time.
Speedup: T1 / Tp (sequential time divided by parallel time).
