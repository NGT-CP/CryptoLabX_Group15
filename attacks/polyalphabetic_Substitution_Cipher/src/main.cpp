#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <filesystem>

using namespace std;

// Standard English letter frequencies (A-Z)
const double ENGLISH_FREQS[26] = {
    0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228, 0.02015, 0.06094,
    0.06966, 0.00153, 0.00772, 0.04025, 0.02406, 0.06749, 0.07507, 0.01929,
    0.00095, 0.05987, 0.06327, 0.09056, 0.02758, 0.00978, 0.02360, 0.00150,
    0.01974, 0.00074};

const double ENGLISH_IC = 0.0667;   // typical IC of English text
const int MAX_KEY_LEN = 20;         // cap on candidate key lengths we'll ever consider
const int MAX_CANDIDATES_SHOWN = 5; // how many Kasiski candidates to display/try

// ---------------------------------------------------------------------
// 1. Preprocess the ciphertext: strip everything but letters, uppercase
// ---------------------------------------------------------------------
string clean_ciphertext(const string &text)
{
    string cleaned;
    cleaned.reserve(text.size());
    for (char c : text)
    {
        if (isalpha(static_cast<unsigned char>(c)))
        {
            cleaned += static_cast<char>(toupper(static_cast<unsigned char>(c)));
        }
    }
    return cleaned;
}

// ---------------------------------------------------------------------
// Identify repeated sequences (length 3..5) in the ciphertext
// ---------------------------------------------------------------------
map<string, vector<int>> find_repeated_patterns(const string &text, int min_len = 3, int max_len = 5)
{
    map<string, vector<int>> patterns;
    for (int len = min_len; len <= max_len; ++len)
    {
        if (static_cast<int>(text.length()) < len)
            break;
        for (size_t i = 0; i + len <= text.length(); ++i)
        {
            patterns[text.substr(i, len)].push_back(static_cast<int>(i));
        }
    }
    // keep only sequences that actually repeat
    map<string, vector<int>> filtered;
    for (const auto &p : patterns)
    {
        if (p.second.size() > 1)
            filtered[p.first] = p.second;
    }
    return filtered;
}

// ---------------------------------------------------------------------
// Distances between successive occurrences of each repeated pattern
// ---------------------------------------------------------------------
vector<int> calculate_distances(const map<string, vector<int>> &patterns)
{
    vector<int> distances;
    for (const auto &p : patterns)
    {
        const vector<int> &idx = p.second;
        for (size_t i = 0; i + 1 < idx.size(); ++i)
        {
            distances.push_back(idx[i + 1] - idx[i]);
        }
    }
    return distances;
}

// ---------------------------------------------------------------------
// Factor each distance (bounded to MAX_KEY_LEN, O(sqrt d) per distance)
// ---------------------------------------------------------------------
map<int, int> find_factors(const vector<int> &distances)
{
    map<int, int> factor_counts;
    for (int d : distances)
    {
        if (d <= 0)
            continue;
        for (int i = 2; static_cast<long long>(i) * i <= d; ++i)
        {
            if (d % i == 0)
            {
                int j = d / i;
                if (i <= MAX_KEY_LEN)
                    factor_counts[i]++;
                if (j != i && j <= MAX_KEY_LEN)
                    factor_counts[j]++;
            }
        }
        // d itself is also a "factor" of d (only useful if small enough)
        if (d <= MAX_KEY_LEN && d >= 2)
            factor_counts[d]++;
    }
    return factor_counts;
}

// ---------------------------------------------------------------------
// Index of Coincidence for a group of letters
// ---------------------------------------------------------------------
double calculate_ic(const string &group)
{
    int n = static_cast<int>(group.length());
    if (n <= 1)
        return 0.0;

    int freqs[26] = {0};
    for (char c : group)
        freqs[c - 'A']++;

    double num = 0.0;
    for (int i = 0; i < 26; ++i)
        num += static_cast<double>(freqs[i]) * (freqs[i] - 1);

    return num / (static_cast<double>(n) * (n - 1));
}

// ---------------------------------------------------------------------
// Split ciphertext into key_len interleaved groups
// ---------------------------------------------------------------------
vector<string> split_into_groups(const string &text, int key_len)
{
    vector<string> groups(key_len);
    for (size_t i = 0; i < text.length(); ++i)
    {
        groups[i % key_len] += text[i];
    }
    return groups;
}

// ---------------------------------------------------------------------
// A-Z relative frequency table for a group
// ---------------------------------------------------------------------
map<char, double> frequency_analysis(const string &group)
{
    map<char, double> freqs;
    int n = static_cast<int>(group.length());
    for (int i = 0; i < 26; ++i)
        freqs['A' + i] = 0.0;
    if (n == 0)
        return freqs; // guard: avoid divide-by-zero, return all-zero table

    int counts[26] = {0};
    for (char c : group)
        counts[c - 'A']++;
    for (int i = 0; i < 26; ++i)
        freqs['A' + i] = static_cast<double>(counts[i]) / n;
    return freqs;
}

// ---------------------------------------------------------------------
// Estimate the Caesar shift of a group via chi-squared statistic
// ---------------------------------------------------------------------
int find_shift(const string &group)
{
    if (group.empty())
        return 0;
    map<char, double> group_freqs = frequency_analysis(group);

    double min_chi_sq = 1e18;
    int best_shift = 0;

    for (int shift = 0; shift < 26; ++shift)
    {
        double chi_sq = 0.0;
        for (int i = 0; i < 26; ++i)
        {
            char c = 'A' + ((i + shift) % 26);
            double observed = group_freqs[c];
            double expected = ENGLISH_FREQS[i];
            if (expected > 0)
            {
                chi_sq += (observed - expected) * (observed - expected) / expected;
            }
        }
        if (chi_sq < min_chi_sq)
        {
            min_chi_sq = chi_sq;
            best_shift = shift;
        }
    }
    return best_shift;
}

// ---------------------------------------------------------------------
// Combine per-group shifts into the probable Vigenere key
// ---------------------------------------------------------------------
string find_key(const string &text, int key_len)
{
    vector<string> groups = split_into_groups(text, key_len);
    string key;
    for (const string &g : groups)
    {
        key += static_cast<char>('A' + find_shift(g));
    }
    return key;
}

// ---------------------------------------------------------------------
// Decrypt ciphertext with the recovered key
// ---------------------------------------------------------------------
string vigenere_decrypt(const string &text, const string &key)
{
    string plaintext;
    plaintext.reserve(text.size());
    int key_len = static_cast<int>(key.length());
    for (size_t i = 0; i < text.length(); ++i)
    {
        int shift = key[i % key_len] - 'A';
        plaintext += static_cast<char>(((text[i] - 'A' - shift + 26) % 26) + 'A');
    }
    return plaintext;
}

// ---------------------------------------------------------------------
// Re-encrypt plaintext for verification
// ---------------------------------------------------------------------
string vigenere_encrypt(const string &text, const string &key)
{
    string ciphertext;
    ciphertext.reserve(text.size());
    int key_len = static_cast<int>(key.length());
    for (size_t i = 0; i < text.length(); ++i)
    {
        int shift = key[i % key_len] - 'A';
        ciphertext += static_cast<char>(((text[i] - 'A' + shift) % 26) + 'A');
    }
    return ciphertext;
}

// ---------------------------------------------------------------------
// Verify re-encryption reproduces the original ciphertext
// ---------------------------------------------------------------------
bool verify(const string &original_cipher, const string &re_encrypted_cipher)
{
    return original_cipher == re_encrypted_cipher;
}

// ---------------------------------------------------------------------
// Average IC across all groups for a candidate key length
// (helper, not required by the spec, but used to pick among Kasiski
//  candidates instead of blindly trusting a single "best factor")
// ---------------------------------------------------------------------
double average_ic_for_key_length(const string &text, int key_len)
{
    vector<string> groups = split_into_groups(text, key_len);
    double total = 0.0;
    int counted = 0;
    for (const string &g : groups)
    {
        if (g.length() > 1)
        {
            total += calculate_ic(g);
            ++counted;
        }
    }
    return counted ? total / counted : 0.0;
}

// ---------------------------------------------------------------------
// Kasiski's test: return candidate key lengths ranked by how often
// they appear as a factor of repeated-pattern distances (descending).
// Falls back to a fixed search range if no repeats were found at all.
// ---------------------------------------------------------------------
vector<int> kasiski_analysis(const string &text)
{
    auto patterns = find_repeated_patterns(text);
    auto distances = calculate_distances(patterns);
    auto factors = find_factors(distances);

    vector<pair<int, int>> ranked(factors.begin(), factors.end()); // (key_len, count)
    sort(ranked.begin(), ranked.end(),
         [](const pair<int, int> &a, const pair<int, int> &b)
         {
             if (a.second != b.second)
                 return a.second > b.second; // higher count first
             return a.first < b.first;       // tie-break: shorter key
         });

    vector<int> candidates;
    for (auto &p : ranked)
    {
        candidates.push_back(p.first);
        if (static_cast<int>(candidates.size()) >= MAX_CANDIDATES_SHOWN)
            break;
    }

    if (candidates.empty())
    {
        // No repeated sequences found (short/unlucky ciphertext) -
        // fall back to trying every length in a reasonable range.
        for (int k = 1; k <= 8; ++k)
            candidates.push_back(k);
    }
    return candidates;
}

// ---------------------------------------------------------------------
// Read the ciphertext file, trying a few likely locations.
// ---------------------------------------------------------------------
bool load_input(int argc, char *argv[], string &out_raw_text, string &used_path)
{
    vector<string> candidates;
    if (argc > 1)
        candidates.push_back(argv[1]);
    candidates.push_back("input.txt");
    candidates.push_back("data/input.txt");
    candidates.push_back("../data/input.txt");

    for (const string &path : candidates)
    {
        ifstream infile(path);
        if (infile.is_open())
        {
            ostringstream ss;
            ss << infile.rdbuf();
            out_raw_text = ss.str();
            used_path = path;
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[])
{
    string raw_text, used_path;
    if (!load_input(argc, argv, raw_text, used_path))
    {
        cerr << "Error: could not find an input file.\n"
             << "Tried: " << (argc > 1 ? string(argv[1]) + ", " : string())
             << "input.txt, data/input.txt, ../data/input.txt\n"
             << "Usage: " << (argc > 0 ? argv[0] : "vigenere_cryptanalysis")
             << " [path_to_ciphertext_file]\n";
        return 1;
    }

    // 1. Preprocess
    string ciphertext = clean_ciphertext(raw_text);
    cout << "--- VIGENERE CRYPTANALYSIS ---\n";
    cout << "[*] Read " << ciphertext.length() << " letters from \"" << used_path << "\"\n\n";

    if (ciphertext.empty())
    {
        cerr << "Error: ciphertext contains no alphabetic characters after cleaning.\n";
        return 1;
    }
    if (ciphertext.length() < 20)
    {
        cout << "[!] Warning: ciphertext is very short - Kasiski/IC results may be unreliable.\n\n";
    }

    cout << "Ciphertext (truncated): " << ciphertext.substr(0, 50)
         << (ciphertext.length() > 50 ? "...\n\n" : "\n\n");

    // 2. Estimate candidate key lengths (Kasiski)
    vector<int> candidates = kasiski_analysis(ciphertext);
    cout << "[*] Kasiski candidate key lengths (ranked): ";
    for (size_t i = 0; i < candidates.size(); ++i)
    {
        cout << candidates[i] << (i + 1 < candidates.size() ? ", " : "\n");
    }

    // Pick the candidate whose average group IC is closest to English (~0.0667)
    int key_length = candidates[0];
    double best_diff = 1e18;
    cout << "[*] Candidate evaluation (avg. Index of Coincidence per group):\n";
    for (int k : candidates)
    {
        double ic = average_ic_for_key_length(ciphertext, k);
        double diff = fabs(ic - ENGLISH_IC);
        cout << "    key length " << setw(2) << k
             << " -> avg IC = " << fixed << setprecision(4) << ic
             << (diff < best_diff ? "   <-- closest to English IC" : "") << "\n";
        if (diff < best_diff)
        {
            best_diff = diff;
            key_length = k;
        }
    }
    cout << "\n[*] Estimated Key Length: " << key_length << "\n\n";

    // 3. Divide into groups
    vector<string> groups = split_into_groups(ciphertext, key_length);

    // 4. Frequency analysis per group (display as a table)
    cout << "[*] Frequency Analysis per Group:\n";
    for (size_t g = 0; g < groups.size(); ++g)
    {
        map<char, double> freqs = frequency_analysis(groups[g]);
        cout << "\n    Group " << (g + 1) << " (" << groups[g].length()
             << " letters, IC = " << fixed << setprecision(4) << calculate_ic(groups[g]) << ")\n    ";
        int col = 0;
        for (int i = 0; i < 26; ++i)
        {
            char c = 'A' + i;
            cout << c << ":" << fixed << setprecision(3) << freqs[c] << "  ";
            if (++col % 6 == 0)
                cout << "\n    ";
        }
        cout << "\n";
    }

    // 5. Determine probable key
    string key = find_key(ciphertext, key_length);
    cout << "\n[*] Recovered Probable Key: " << key << "\n";

    // 6. Decrypt
    string plaintext = vigenere_decrypt(ciphertext, key);
    cout << "\n[*] Recovered Plaintext (truncated): " << plaintext.substr(0, 150)
         << (plaintext.length() > 150 ? "...\n\n" : "\n\n");

    // 8. Verify
    string re_encrypted = vigenere_encrypt(plaintext, key);
    bool ok = verify(ciphertext, re_encrypted);

    cout << "--- VERIFICATION ---\n";

    if (ok)
    {
        cout << "[+] SUCCESS: Re-encryption matches the original ciphertext.\n";
    }
    else
    {
        cout << "[-] FAILURE: Re-encryption does not match the original ciphertext.\n";
    }

    cout << "\nNote: this check only confirms encrypt/decrypt are correct inverses of\n"
            "each other for this key - it does NOT independently confirm the key is\n"
            "actually correct. Judge that from whether the recovered plaintext reads\n"
            "as sensible English.\n";

    // ---------------------------------------------------------
    // Save decryption result to outputs folder
    // ---------------------------------------------------------
    filesystem::create_directories("../outputs");

    string output_path = "../outputs/decryption.txt";
    ofstream outfile(output_path);

    if (!outfile.is_open())
    {
        cerr << "\n[-] ERROR: Could not create output file: "
             << output_path << "\n";
    }
    else
    {
        outfile << "==============================================\n";
        outfile << "       VIGENERE CRYPTANALYSIS RESULT\n";
        outfile << "==============================================\n\n";

        outfile << "Input File: " << used_path << "\n\n";

        outfile << "Estimated Key Length: "
                << key_length << "\n";

        outfile << "Recovered Key: "
                << key << "\n\n";

        outfile << "Recovered Plaintext:\n";
        outfile << plaintext << "\n\n";

        outfile << "Original Ciphertext:\n";
        outfile << ciphertext << "\n\n";

        outfile << "Re-encrypted Ciphertext:\n";
        outfile << re_encrypted << "\n\n";

        outfile << "Verification: "
                << (ok ? "SUCCESS" : "FAILURE") << "\n";

        outfile.close();

        cout << "\n[+] Decryption result saved to: "
             << output_path << "\n";
    }

    return 0;
}