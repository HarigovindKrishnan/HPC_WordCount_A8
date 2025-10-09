# High-Performance Parallel Word Counter

A blazingly fast C++ word frequency analyzer that uses memory-mapped I/O and parallel processing to count words in large text files.

## Features

- **Memory-mapped I/O**: Efficiently reads large files without loading entire contents into memory
- **Parallel Processing**: Uses OpenMP to distribute work across multiple CPU cores
- **Smart Tokenization**: Handles contractions (e.g., "don't", "I'm") and normalizes words
- **Case-insensitive**: Converts all words to lowercase for accurate counting
- **Top-K Results**: Displays the most frequent words with customizable output size

## Requirements

- C++11 or later
- OpenMP support
- POSIX-compliant system (Linux, macOS, Unix)
- GCC or Clang compiler

## Compilation

```bash
g++ -std=c++11 -O3 -fopenmp -o word_counter word_counter.cpp
```

Or with Clang:

```bash
clang++ -std=c++11 -O3 -fopenmp -o word_counter word_counter.cpp
```

## Usage

```bash
./word_counter <file> [topK]
```

### Arguments

- `<file>`: Path to the text file to analyze (required)
- `[topK]`: Number of top words to display (optional, default: 20)

### Examples

Count words and show top 20:
```bash
./word_counter document.txt
```

Count words and show top 50:
```bash
./word_counter document.txt 50
```

## Output

The program displays:
- File path
- Total token count
- Number of unique words
- Top K most frequent words with their counts
- Processing time

Example output:
```
File: sample.txt
Total tokens: 45821
Unique words: 8432
Top 20 words:
  2341  the
  1876  and
  1234  of
   987  to
   ...
```

## How It Works

1. **Memory Mapping**: The file is mapped directly into memory using `mmap()`, avoiding expensive read operations
2. **Parallel Chunking**: The file is divided into chunks, one per CPU thread
3. **Boundary Alignment**: Chunk boundaries are adjusted to avoid splitting words
4. **Local Counting**: Each thread counts words in its chunk independently
5. **Merging**: Thread-local counts are merged into a global frequency map
6. **Sorting**: Words are sorted by frequency (descending) and alphabetically for ties

## Word Normalization Rules

- Leading and trailing non-alphanumeric characters are removed
- Words are converted to lowercase
- Apostrophes are preserved only between alphanumeric characters (for contractions)
- Other punctuation is treated as word separators
- Multiple spaces are collapsed into single spaces

## Performance

The parallel implementation provides significant speedup on multi-core systems, especially for large files. Processing time scales approximately linearly with file size and inversely with the number of available CPU cores.

## License

This code is provided as-is for educational and practical use.