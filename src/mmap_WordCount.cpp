#include <bits/stdc++.h>
#include <omp.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

using CountMap = unordered_map<string, uint32_t>;

static inline bool is_word_char(unsigned char c)
{
    return std::isalnum(c);
}

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
        {
            out.push_back((char)std::tolower(c));
        }
        else
        {
            out.push_back(' ');
        }
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

static CountMap count_words_mmap(const string &path, uint64_t &total_tokens)
{
    // Open file
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0)
        throw runtime_error("Failed to open file: " + path);

    // Get size
    struct stat st;
    if (fstat(fd, &st) < 0)
        throw runtime_error("fstat failed");
    size_t filesize = st.st_size;

    // Memory-map file
    char *data = (char *)mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED)
        throw runtime_error("mmap failed");
    close(fd);

    // Parallel word count
    int nthreads = omp_get_max_threads();
    vector<CountMap> local_counts(nthreads);
    vector<uint64_t> local_tokens(nthreads, 0);

#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        size_t chunk_size = filesize / nthreads;
        size_t start = tid * chunk_size;
        size_t end = (tid == nthreads - 1) ? filesize : (tid + 1) * chunk_size;

        // Adjust boundaries so we don’t cut a word in half
        while (start > 0 && is_word_char((unsigned char)data[start]))
            start--;
        while (end < filesize && is_word_char((unsigned char)data[end]))
            end++;

        CountMap &local = local_counts[tid];

        // Tokenize directly from mmap buffer
        size_t i = start;
        string token;
        while (i < end)
        {
            unsigned char c = data[i];
            if (is_word_char(c) || c == '\'')
            {
                token.push_back((char)tolower(c));
            }
            else
            {
                if (!token.empty())
                {
                    string norm = normalize_token(token);
                    if (!norm.empty())
                    {
                        ++local[norm];
                        ++local_tokens[tid];
                    }
                    token.clear();
                }
            }
            i++;
        }
        if (!token.empty())
        {
            string norm = normalize_token(token);
            if (!norm.empty())
            {
                ++local[norm];
                ++local_tokens[tid];
            }
        }
    }

    // Merge results
    CountMap global;
    total_tokens = 0;
    for (int t = 0; t < nthreads; t++)
    {
        total_tokens += local_tokens[t];
        for (auto &kv : local_counts[t])
        {
            global[kv.first] += kv.second;
        }
    }

    // Cleanup
    munmap(data, filesize);
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
    CountMap counts = count_words_mmap(path, total_tokens);

    auto t1 = chrono::high_resolution_clock::now();

    cout << "File: " << path << "\n";
    cout << "Total tokens: " << total_tokens << "\n";
    cout << "Unique words: " << counts.size() << "\n";
    cout << "Top " << topK << " words:\n";
    print_top_k(counts, topK);

    auto dt = chrono::duration<double>(t1 - t0).count();
    cerr << fixed << setprecision(3);
    cerr << "\nElapsed (mmap+parallel): " << dt << " s\n";

    return 0;
}
