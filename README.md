## 🔹 Project Overview
This project implements a parallel word count system using C++ and leveraging OpenMP in large text datasets. Word count is a common task in fields such as text analytics.

The workflow involves exploring multiple optimization methods such as different I/O approaches (ifstream and mmap)

## 🔹 Implementations
1. Baseline sequential implementation of Wordcount operation.
2. OMP + ifstream parallel implementation
3. OMP + mmap parallel implementation
4. OMP + ifstream implementation along with thread pinning

---
## 🔹 Evaluation Metrics:
- Wall-clock time: elapsed time.
- Speedup: T1 / Tp (sequential time divided by parallel time).
- Efficiency: Speedup * 100 / no. of threads
