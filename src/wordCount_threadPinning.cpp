#include <bits/stdc++.h>
#include <omp.h>
#include <pthread.h>
#include <sched.h>
using namespace std;

// Configurable alias
using CountMap = unordered_map<string, uint32_t>;

// ASCII helper
static inline bool is_word_char(unsigned char c)
{
    return std::isalnum(c);
}

// Token normalization
static string normalize_token(const string &s)
{
    size_t n = s.size();
    size_t i = 0, j = n ? n - 1 : 0;

    while (i < n && !is_word_char((unsigned char)s[i]))
        ++i;
    if (i == n)
        return {};
    while (j > i && !is_word_char((unsigned char)s[j]))
        --j;

    string out;
    out.reserve(j - i + 1);

    for (size_t k = i; k <= j; ++k)
    {
        unsigned char c = (unsigned char)s[k];
        if (is_word_char(c) || c == '\'')
            out.push_back((char)std::tolower(c));
        else
            out.push_back(' ');
    }

    string cleaned;
    cleaned.reserve(out.size());
    bool prev_alnum = false;
    for (size_t k = 0; k < out.size(); ++k)
    {
        char c = out[k];
        if (std::isalnum((unsigned char)c))
        {
            cleaned.push_back(c);
            prev_alnum = true;
        }
        else if (c == '\'')
        {
            bool left_ok = prev_alnum;
            bool right_ok = (k + 1 < out.size()) && std::isalnum((unsigned char)out[k + 1]);
            if (left_ok && right_ok)
                cleaned.push_back(c);
            prev_alnum = false;
        }
        else
        {
            if (!cleaned.empty() && cleaned.back() != ' ')
                cleaned.push_back(' ');
            prev_alnum = false;
        }
    }

    return cleaned;
}

// Sequential tokenizer for one line → list of tokens
static vector<string> tokenize_line(const string &line)
{
    string norm = normalize_token(line);
    vector<string> tokens;
    const char *s = norm.c_str();
    size_t len = norm.size();
    size_t start = 0;

    while (start < len)
    {
        while (start < len && s[start] == ' ')
            ++start;
        if (start >= len)
            break;
        size_t end = start;
        while (end < len && s[end] != ' ')
            ++end;
        if (end > start)
            tokens.emplace_back(s + start, s + end);
        start = end;
    }
    return tokens;
}

// Parallel word count with thread pinning
static CountMap count_words_parallel(const string &path, uint64_t &total_tokens)
{
    ifstream in(path);
    if (!in)
        throw runtime_error("Failed to open file: " + path);

    // Read all lines first (sequential)
    vector<string> lines;
    string line;
    while (getline(in, line))
    {
        lines.push_back(move(line));
    }

    size_t n = lines.size();
    total_tokens = 0;

    omp_set_num_threads(4); // set your preferred number of threads here
    int nthreads = omp_get_max_threads();
    vector<CountMap> local_counts(nthreads);

#pragma omp parallel
    {
        ////int tid = omp_get_thread_num();
        ////int nprocs = omp_get_num_procs();

        // Map threads to physical cores (skip hyperthreads)
        ////int core_id = (tid * 2) % nprocs; // 0→0, 1→2 (for your 4 logical CPUs)

        ////cpu_set_t cpuset;
        ////CPU_ZERO(&cpuset);
        ////CPU_SET(core_id, &cpuset);
        ////pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        ///////////////
        int tid = omp_get_thread_num();
        int nprocs = omp_get_num_procs();

        // --- THREAD PINNING SECTION ---
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(tid % nprocs, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        // ------------------------------

        // (Optional) Verification: print pinning info once per thread
#pragma omp critical
        {
            cpu_set_t get;
            CPU_ZERO(&get);
            pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &get);
            cout << "Thread " << tid << " pinned to cores: ";
            for (int i = 0; i < nprocs; ++i)
                if (CPU_ISSET(i, &get))
                    cout << i << " ";
            cout << endl;
        }

        CountMap &local = local_counts[tid];
        uint64_t local_tokens = 0;

#pragma omp for schedule(dynamic, 1000)
        for (size_t i = 0; i < n; i++)
        {
            auto tokens = tokenize_line(lines[i]);
            for (auto &tok : tokens)
            {
                ++local[tok];
                ++local_tokens;
            }
        }

#pragma omp atomic
        total_tokens += local_tokens;
    }

    // Merge local maps into one global map
    CountMap global;
    for (auto &local : local_counts)
    {
        for (auto &kv : local)
        {
            global[kv.first] += kv.second;
        }
    }

    return global;
}

// Print top-K words
static void print_top_k(const CountMap &counts, size_t k)
{
    vector<pair<string, uint32_t>> vec(counts.begin(), counts.end());

    sort(vec.begin(), vec.end(), [](const auto &a, const auto &b)
         {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first; });

    size_t limit = min(k, vec.size());
    for (size_t i = 0; i < limit; i++)
    {
        cout << setw(6) << vec[i].second << "  " << vec[i].first << '\n';
    }
}

int main(int argc, char **argv)
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <file> [topK]\n";
        return 1;
    }
    string path = argv[1];
    size_t topK = (argc >= 3) ? (size_t)stoull(argv[2]) : 20;

    auto t0 = chrono::high_resolution_clock::now();

    uint64_t total_tokens = 0;
    CountMap counts = count_words_parallel(path, total_tokens);

    auto t1 = chrono::high_resolution_clock::now();

    cout << "File: " << path << "\n";
    cout << "Total tokens: " << total_tokens << "\n";
    cout << "Unique words: " << counts.size() << "\n";
    cout << "Top " << topK << " words:\n";
    print_top_k(counts, topK);

    auto dt = chrono::duration<double>(t1 - t0).count();
    cerr << fixed << setprecision(3);
    cerr << "\nElapsed (parallel + pinned): " << dt << " s\n";

    return 0;
}