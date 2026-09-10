#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iomanip>
#include <limits>

using namespace std;
namespace fs = std::filesystem;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string ENCRYPTION_KEY = "XQMWZKJHDFYVOTNRCBULPGEIAS";

// --- HELPER FUNCTIONS ---

string read_file(const string& path) {
    ifstream file(path);
    if (!file) {
        cerr << "Error: Could not open " << path << '\n';
        return "";
    }
    string text, line;
    while (getline(file, line)) {
        text += line + '\n';
    }
    return text;
}

void write_file(const string& path, const string& content) {
    ofstream file(path);
    if (!file) {
        cerr << "Error: Could not create " << path << '\n';
        return;
    }
    file << content;
}

string encrypt_text(const string& plaintext, const string& key) {
    string ciphertext = plaintext;
    for (int i = 0; i < (int)plaintext.size(); i++) {
        char ch = plaintext[i];
        if (ch >= 'A' && ch <= 'Z') {
            if (key[ch - 'A'] != '?') ciphertext[i] = key[ch - 'A'];
        } else if (ch >= 'a' && ch <= 'z') {
            if (key[ch - 'a'] != '?') ciphertext[i] = tolower(key[ch - 'a']);
        }
    }
    return ciphertext;
}

vector<string> extract_words(const string& text) {
    vector<string> words;
    string word;
    for (char ch : text) {
        if (isalpha(static_cast<unsigned char>(ch))) {
            word += toupper(static_cast<unsigned char>(ch));
        } else {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
    }
    if (!word.empty()) words.push_back(word);
    return words;
}

// --- REQUIRED FUNCTION 1: Letter Frequency Analysis ---
void frequency_analysis(const string& ciphertext) {
    int frequency[26] = {0};
    int total_letters = 0;

    for (char ch : ciphertext) {
        if (isalpha(static_cast<unsigned char>(ch))) {
            frequency[toupper(static_cast<unsigned char>(ch)) - 'A']++;
            total_letters++;
        }
    }

    vector<pair<char, int>> sorted_freq;
    for (int i = 0; i < 26; i++) {
        sorted_freq.push_back({char('A' + i), frequency[i]});
    }

    sort(sorted_freq.begin(), sorted_freq.end(), [](const pair<char, int>& a, const pair<char, int>& b) {
        return a.second > b.second;
    });

    ofstream output("../outputs/frequency_analysis.txt");
    cout << "\n========== LETTER FREQUENCY ANALYSIS ==========\n";
    output << "========== LETTER FREQUENCY ANALYSIS ==========\n\n";

    string header = "Total letters: " + to_string(total_letters) + "\n\n";
    cout << header; output << header;

    cout << left << setw(10) << "Letter" << setw(10) << "Count" << "Percentage\n";
    output << left << setw(10) << "Letter" << setw(10) << "Count" << "Percentage\n";

    for (int i = 0; i < 26; i++) {
        double percentage = total_letters > 0 ? (sorted_freq[i].second * 100.0 / total_letters) : 0.0;
        cout << left << setw(10) << sorted_freq[i].first << setw(10) << sorted_freq[i].second
             << fixed << setprecision(2) << percentage << "%\n";
        output << left << setw(10) << sorted_freq[i].first << setw(10) << sorted_freq[i].second
               << fixed << setprecision(2) << percentage << "%\n";
    }

    cout << "\nMost frequent ciphertext letters:\n";
    output << "\nMost frequent ciphertext letters:\n";
    for (int i = 0; i < 5; i++) {
        cout << sorted_freq[i].first << " -> " << sorted_freq[i].second << "\n";
        output << sorted_freq[i].first << " -> " << sorted_freq[i].second << "\n";
    }

    output.close();
}

// --- REQUIRED FUNCTION 2: Word Frequency Analysis ---
void word_frequency_analysis(const string& ciphertext) {
    vector<string> words = extract_words(ciphertext);
    map<string, int> frequency;
    for (const string& word : words) frequency[word]++;

    vector<pair<string, int>> sorted_words(frequency.begin(), frequency.end());
    sort(sorted_words.begin(), sorted_words.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second;
    });

    ofstream output("../outputs/word_analysis.txt");
    output << "========== WORD FREQUENCY ANALYSIS ==========\n\n";

    output << "Total words: " << words.size() << "\n\n";

    auto print_by_length = [&](size_t len, const string& label) {
        output << "\n" << label << ":\n";
        for (const auto& entry : sorted_words) {
            if (entry.first.size() == len) output << entry.first << " -> " << entry.second << '\n';
        }
    };

    print_by_length(1, "One-letter words");
    print_by_length(2, "Two-letter words");
    print_by_length(3, "Three-letter words");

    output << "\nRepeated words:\n";
    for (const auto& entry : sorted_words) {
        if (entry.second > 1) output << entry.first << " -> " << entry.second << '\n';
    }
    output.close();
    cout << "Word frequency analysis saved to outputs/word_analysis.txt\n";
}

// --- REQUIRED FUNCTION 3: Pattern Analysis ---
string get_pattern(const string& word) {
    map<char, int> mapping;
    string pattern;
    int next_number = 1;
    for (char ch : word) {
        if (mapping.find(ch) == mapping.end()) mapping[ch] = next_number++;
        pattern += to_string(mapping[ch]);
    }
    return pattern;
}

void pattern_analysis(const string& ciphertext) {
    vector<string> words = extract_words(ciphertext);
    map<string, int> word_counts;
    for (const string& w : words) word_counts[w]++;

    // Map a pattern to a list of distinct words and their counts
    map<string, vector<pair<string, int>>> patterns;
    for (const auto& pair : word_counts) {
        patterns[get_pattern(pair.first)].push_back(pair);
    }

    ofstream output("../outputs/pattern_analysis.txt");
    output << "========== PATTERN ANALYSIS ==========\n\n";

    for (auto& entry : patterns) {
        int total_occurrences = 0;
        for (const auto& w : entry.second) total_occurrences += w.second;

        // Only output if the pattern appears more than once overall
        if (total_occurrences > 1) {
            // Sort words within the pattern by frequency
            sort(entry.second.begin(), entry.second.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
                return a.second > b.second;
            });

            output << "Pattern " << entry.first << " : ";
            for (size_t i = 0; i < entry.second.size(); i++) {
                output << entry.second[i].first << " (" << entry.second[i].second << ")";
                if (i + 1 < entry.second.size()) output << ", ";
            }
            output << '\n';
        }
    }
    output.close();
    cout << "Pattern analysis saved to outputs/pattern_analysis.txt\n";
}

// --- REQUIRED FUNCTION 4: Apply Substitution ---
string apply_substitution(const string& ciphertext, const string& substitution) {
    string result = ciphertext;
    for (size_t i = 0; i < ciphertext.size(); i++) {
        char ch = ciphertext[i];
        if (ch >= 'A' && ch <= 'Z') {
            char mapped = substitution[ch - 'A'];
            result[i] = (mapped == '?') ? '_' : mapped;
        } else if (ch >= 'a' && ch <= 'z') {
            char mapped = substitution[ch - 'a'];
            result[i] = (mapped == '?') ? '_' : tolower(mapped);
        }
    }
    return result;
}

// --- REQUIRED FUNCTION 5: Display Partial Plaintext ---
void display_partial_plaintext(const string& ciphertext, const string& substitution) {
    string partial = apply_substitution(ciphertext, substitution);
    cout << "\n========== PARTIAL PLAINTEXT ==========\n\n" << partial << "\n";
    write_file("../outputs/partial_plaintext.txt", partial);
}

// --- REQUIRED FUNCTION 6: Verify Solution ---
bool verify_solution(const string& recovered_plaintext, const string& ciphertext, const string& recovered_key) {
    string inverse_key(26, '?');
    for (int i = 0; i < 26; i++) {
        if (recovered_key[i] != '?') {
            int plain_idx = recovered_key[i] - 'A';
            inverse_key[plain_idx] = char('A' + i);
        }
    }

    string regenerated = encrypt_text(recovered_plaintext, inverse_key);
    return regenerated == ciphertext;
}

// --- INTERACTIVE CRYPTANALYSIS LOOP ---
bool valid_substitution(const string& key, char cipher, char plain) {
    int index = cipher - 'A';
    if (key[index] != '?' && key[index] != plain) return false;
    for (int i = 0; i < 26; i++) {
        if (i != index && key[i] == plain) return false;
    }
    return true;
}

void display_key(const string& key) {
    cout << "\nCipher : ";
    for (char ch : ALPHABET) cout << ch << " ";
    cout << "\nPlain  : ";
    for (char ch : key) cout << ch << " ";
    cout << "\n";
}

// Parses a single free-form line such as "Q->E", "Q-E", "Q E", "QE" or
// "Q->E most frequent letter" into a cipher letter and a plain letter.
// The FIRST alphabetic character found is treated as the cipher letter,
// the SECOND alphabetic character found is treated as the plain letter.
// Returns false if fewer than two alphabetic characters are present.
bool parse_mapping(const string& input, char& cipher, char& plain) {
    string letters;
    for (char ch : input) {
        if (isalpha(static_cast<unsigned char>(ch))) letters += toupper(static_cast<unsigned char>(ch));
        if (letters.size() == 2) break;
    }
    if (letters.size() < 2) return false;
    cipher = letters[0];
    plain = letters[1];
    return true;
}

void cryptanalysis(const string& ciphertext) {
    string recovered_key(26, '?');
    ofstream table("../outputs/cryptanalysis_table.txt");

    table << "========== CRYPTANALYSIS TABLE ==========\n\n";
    table << left << setw(6) << "Step" << setw(40) << "Observation / Mapping"
          << setw(25) << "Substitution Tested" << setw(35) << "Result" << "Decision\n";
    table << string(140, '-') << "\n";

    cout << "\nAt each step, enter ONE line that both records your observation and\n"
         << "states the hypothesized mapping, e.g.:  Q->E  (Q occurs most frequently)\n"
         << "The first two letters typed are read as CipherLetter -> PlainLetter.\n"
         << "Enter '0' alone to finish.\n";

    int step = 1;
    while (true) {
        cout << "\n========== CURRENT KEY ==========\n";
        display_key(recovered_key);

        cout << "\nStep " << step << " - Observation & mapping (e.g. \"Q->E most frequent\", or 0 to finish): ";
        string entry;
        getline(cin >> ws, entry);

        // trim
        while (!entry.empty() && isspace(static_cast<unsigned char>(entry.back()))) entry.pop_back();

        if (entry == "0") break;

        char cipher, plain;
        if (!parse_mapping(entry, cipher, plain)) {
            cout << "Could not find two letters (cipher, plain) in that line. Try again, e.g. \"Q->E\".\n";
            continue;
        }

        string tested = string(1, cipher) + " -> " + string(1, plain);

        if (!valid_substitution(recovered_key, cipher, plain)) {
            string result = "Contradiction with existing mapping.";
            cout << "\n" << result << "\nDecision: REJECTED\n";
            table << left << setw(6) << step << setw(40) << entry
                  << setw(25) << tested << setw(35) << result << "Rejected\n";
            step++;
            continue;
        }

        string temporary = recovered_key;
        temporary[cipher - 'A'] = plain;
        display_partial_plaintext(ciphertext, temporary);

        cout << "\nAccept this substitution? (y/n): ";
        char decision; cin >> decision;

        if (tolower(decision) == 'y') {
            recovered_key = temporary;
            cout << "Decision: ACCEPTED\n";
            table << left << setw(6) << step << setw(40) << entry
                  << setw(25) << tested << setw(35) << "Partial plaintext improved." << "Accepted\n";
        } else {
            cout << "Decision: REJECTED\n";
            table << left << setw(6) << step << setw(40) << entry
                  << setw(25) << tested << setw(35) << "Text did not support hypothesis." << "Rejected\n";
        }
        step++;
    }
    table.close();

    string recovered_plaintext = apply_substitution(ciphertext, recovered_key);
    write_file("../outputs/recovered_plaintext.txt", recovered_plaintext);
    write_file("../outputs/recovered_key.txt", "Cipher : " + ALPHABET + "\nPlain  : " + recovered_key + "\n");

    cout << "\n========== FINAL RECOVERED PLAINTEXT ==========\n\n" << recovered_plaintext << "\n";

    // Validate using required verify_solution function
    bool is_complete = (recovered_key.find('?') == string::npos);
    ofstream verification("../outputs/verification.txt");
    verification << "========== VERIFICATION ==========\n\n";

    if (!is_complete) {
        cout << "\nVerification Note: The recovered key is incomplete. Skipping full re-encryption verification.\n";
        verification << "Verification skipped: The recovered key is incomplete (missing letters). Full re-encryption requires a complete 26-letter mapping.\n";
    } else {
        bool verified = verify_solution(recovered_plaintext, ciphertext, recovered_key);
        if (verified) {
            cout << "\nVerification successful: Re-encryption matches original ciphertext.\n";
            verification << "Verification successful.\nRecovered plaintext re-encrypts to the exact original ciphertext.\n";
        } else {
            cout << "\nVerification failed: Re-encryption does not match original ciphertext (Key contains errors).\n";
            verification << "Verification failed.\nRecovered key contains errors.\n";
        }
    }
    verification.close();
}

int main() {
    fs::create_directories("../outputs");
    fs::create_directories("../data");

    string plaintext = read_file("../data/plaintext.txt");
    if (plaintext.empty()) {
        cout << "Please create 'plaintext.txt' in a '../data/' folder containing your book excerpt.\n";
        return 1;
    }

    cout << "========== ASSIGNMENT 5: MONOALPHABETIC CRYPTANALYSIS ==========\n\n";

    // Save original key for record, but keep it isolated from the cryptanalysis loop
    write_file("../outputs/original_key.txt", "Original Encryption Key: " + ENCRYPTION_KEY);

    string ciphertext = encrypt_text(plaintext, ENCRYPTION_KEY);
    write_file("../outputs/ciphertext.txt", ciphertext);
    cout << "Ciphertext generated successfully.\n";

    frequency_analysis(ciphertext);
    word_frequency_analysis(ciphertext);
    pattern_analysis(ciphertext);

    cryptanalysis(ciphertext);

    cout << "\nAll assignment files generated in the ../outputs/ directory.\n";
    return 0;
}
