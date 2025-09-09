## 🔹 Overview:
- Reads all the lines first into vector<string> 
- Uses unordered_map for word-frequency storage.
- Each thread has its own local hash map for word counts.
- Output: Word frequency list + execution time.

---
## 🔹 Results:
- Avg Execution time (across 5 runs):
  -  0.136 s [6 Threads]
  -  0.137 s [4 Threads]
  -  0.155 s [2 THreads]
- Total tokens: 989195
  - Unique words: 24876
