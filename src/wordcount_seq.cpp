// src/wordcount_seq.cpp
#include <bits/stdc++.h>
using namespace std;

// Change this to `map<string, uint32_t>` if you want an always-sorted map by word.
using CountMap = unordered_map<string, uint32_t>;

// ASCII helper: is letter or digit
static inline bool is_word_char(unsigned char c)
{
    return std::isalnum(c);
}

// Normalize a raw token to our "word":
// - Lowercase
// - Keep letters/digits
// - Allow inner apostrophes (e.g., don't) but trim leading/trailing apostrophes
static string normalize_token(const string &s)
{
    // Trim to first/last alnum to avoid leading/trailing punctuation quickly
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
            // treat other punctuation as separators: convert to space marker
            out.push_back(' ');
        }
    }

    // Collapse spaces and keep apostrophes only if between alnum blocks.
    // Example: "rock'n'roll" -> "rock'n'roll"; "'hello'" -> "hello"
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
            // keep apostrophe only if surrounded by alnum on both sides
            bool left_ok = prev_alnum;
            bool right_ok = (k + 1 < out.size()) && std::isalnum((unsigned char)out[k + 1]);
            if (left_ok && right_ok)
                cleaned.push_back(c);
            prev_alnum = false;
        }
        else
        {
            // separator -> single space to delimit tokens
            if (!cleaned.empty() && cleaned.back() != ' ')
                cleaned.push_back(' ');
            prev_alnum = false;
        }
    }

    // Now split by spaces and return only if single token; if multiple tokens, caller will split anyway.
    // For baseline simplicity, we return cleaned as-is and let caller split.
    return cleaned;
}

// Read file streaming, tokenize, and count
static CountMap count_words_streaming(const string &path, uint64_t &total_tokens)
{
    ifstream in(path, ios::in | ios::binary);
    if (!in)
    {
        throw runtime_error("Failed to open file: " + path);
    }

    CountMap counts;
    counts.reserve(1 << 16); // pre-reserve to reduce rehashes (tweak as needed)

    string line;
    total_tokens = 0;

    while (std::getline(in, line))
    {
        string norm = normalize_token(line);

        // Fast split on spaces
        const char *s = norm.c_str();
        size_t len = norm.size();
        size_t start = 0;
        while (start < len)
        {
            // skip spaces
            while (start < len && s[start] == ' ')
                ++start;
            if (start >= len)
                break;
            size_t end = start;
            while (end < len && s[end] != ' ')
                ++end;

            // token = [start, end)
            if (end > start)
            {
                ++counts[string(s + start, s + end)];
                ++total_tokens;
            }
            start = end;
        }
    }
    return counts;
}

static void print_top_k(const CountMap &counts, size_t k)
{
    vector<pair<string, uint32_t>> vec;
    vec.reserve(counts.size());
    for (auto &kv : counts)
        vec.emplace_back(kv.first, kv.second);

    // Sort by frequency desc, then lex asc
    std::sort(vec.begin(), vec.end(), [](const auto &a, const auto &b)
              {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first; });

    size_t limit = std::min(k, vec.size());
    for (size_t i = 0; i < limit; ++i)
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
    CountMap counts = count_words_streaming(path, total_tokens);

    auto t1 = chrono::high_resolution_clock::now();

    // Output summary
    cout << "File: " << path << "\n";
    cout << "Total tokens: " << total_tokens << "\n";
    cout << "Unique words: " << counts.size() << "\n";
    cout << "Top " << topK << " words:\n";
    print_top_k(counts, topK);

    auto dt = chrono::duration<double>(t1 - t0).count();
    cerr << fixed << setprecision(3);
    cerr << "\nElapsed: " << dt << " s\n";

    return 0;
}
