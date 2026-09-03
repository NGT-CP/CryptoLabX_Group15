#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main()
{
    fs::create_directories("../outputs");

    ofstream ciphertext("../outputs/ciphertext.txt");
    if (!ciphertext)
    {
        cerr << "Error creating ciphertext.txt\n";
        return 1;
    }
    ciphertext << "DUMMY CIPHERTEXT\n";
    ciphertext.close();

    ofstream frequency("../outputs/frequency_analysis.txt");
    if (!frequency)
    {
        cerr << "Error creating frequency_analysis.txt\n";
        return 1;
    }
    frequency << "========== FREQUENCY ANALYSIS ==========\n";
    frequency << "A -> 10 -> 5.00%\n";
    frequency << "B -> 8  -> 4.00%\n";
    frequency << "C -> 6  -> 3.00%\n";
    frequency.close();

    ofstream word("../outputs/word_analysis.txt");
    if (!word)
    {
        cerr << "Error creating word_analysis.txt\n";
        return 1;
    }
    word << "========== WORD FREQUENCY ANALYSIS ==========\n";
    word << "One-letter words: X\n";
    word << "Two-letter words: AB, XY\n";
    word << "Three-letter words: QRM, XYZ\n";
    word << "Repeated words: QRM -> 3 times\n";
    word.close();

    ofstream pattern("../outputs/pattern_analysis.txt");
    if (!pattern)
    {
        cerr << "Error creating pattern_analysis.txt\n";
        return 1;
    }
    pattern << "========== PATTERN ANALYSIS ==========\n";
    pattern << "Pattern 123: ABC\n";
    pattern << "Pattern 122: ABB\n";
    pattern << "Pattern 1231: ABCA\n";
    pattern.close();

    ofstream log("../outputs/cryptanalysis_log.txt");
    if (!log)
    {
        cerr << "Error creating cryptanalysis_log.txt\n";
        return 1;
    }
    log << "========== CRYPTANALYSIS LOG ==========\n\n";
    log << "Step 1\n";
    log << "Observation: Q is the most frequent ciphertext letter.\n";
    log << "Possible substitution: Q -> E\n";
    log << "Tested substitution: Q -> E\n";
    log << "Result: Partial plaintext improved.\n";
    log << "Decision: ACCEPTED\n\n";
    log << "Step 2\n";
    log << "Observation: X is a one-letter word.\n";
    log << "Possible substitution: X -> A / I\n";
    log << "Tested substitution: X -> A\n";
    log << "Result: Contradiction found.\n";
    log << "Decision: REJECTED\n";
    log.close();

    ofstream key("../outputs/recovered_key.txt");
    if (!key)
    {
        cerr << "Error creating recovered_key.txt\n";
        return 1;
    }
    key << "========== RECOVERED KEY ==========\n";
    key << "Cipher : ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
    key << "Plain  : ??????????????????????????\n";
    key.close();

    cout << "Dummy Assignment-5 program executed successfully.\n";
    cout << "Output files created inside ../outputs/\n";

    return 0;
}