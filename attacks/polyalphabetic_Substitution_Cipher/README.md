# Vigenère Cipher Cryptanalysis

## Overview

This project implements cryptanalysis of the **Vigenère Cipher** without requiring the encryption key as input.

The program uses:

* **Kasiski Examination** to identify possible key lengths.
* **Index of Coincidence (IC)** to evaluate candidate key lengths.
* **Frequency Analysis** and **Chi-Squared Testing** to recover the individual key characters.
* **Vigenère Decryption** to recover the plaintext.
* **Re-encryption Verification** to check whether the recovered plaintext and key reproduce the original ciphertext.

---

## Assignment Requirements

The program performs the following operations:

1. Preprocess the ciphertext.
2. Estimate the key length using Kasiski Examination.
3. Divide the ciphertext into groups according to the estimated key length.
4. Perform frequency analysis on each group.
5. Determine the probable Vigenère key.
6. Decrypt the ciphertext using the recovered key.
7. Display:

   * Estimated key length
   * Frequency table for each group
   * Recovered key
   * Recovered plaintext
8. Re-encrypt the recovered plaintext and verify that it matches the original ciphertext.

---

## Directory Structure

```text
polyalphabetic_Substitution_Cipher/
│
├── src/
│   ├── main.cpp
│   └── main
│
├── data/
│   └── input.txt
│
├── outputs/
│   └── decryption.txt
│
└── README.md
```

---

## Input

The program reads the ciphertext from:

```text
../data/input.txt
```

The ciphertext may contain:

* Spaces
* New lines
* Lowercase letters
* Uppercase letters
* Other non-alphabetic characters

The preprocessing stage removes non-alphabetic characters and converts all letters to uppercase.

### Example input

```text
DAZFISFSPAVQLSNPXYSZWXALCDAFGQUIS
MTPHZGAMKTTFTCCFX...
```

---

## Compilation

Navigate to the `src` directory:

```bash
cd attacks/polyalphabetic_Substitution_Cipher/src
```

Compile using C++17:

```bash
g++ main.cpp -o main -std=c++17
```

---

## Execution

Run the program:

```bash
./main
```

The program automatically reads:

```text
../data/input.txt
```

It also creates the `outputs` directory automatically when necessary.

The decrypted result is saved to:

```text
../outputs/decryption.txt
```

---

# Cryptanalysis Method

## 1. Ciphertext Preprocessing

The function:

```cpp
clean_ciphertext()
```

removes spaces, punctuation, digits, and other non-alphabetic characters.

For example:

```text
DAZFIS FSPA-VQLS
```

becomes:

```text
DAZFISFSPAVQLS
```

---

## 2. Kasiski Examination

The function:

```cpp
find_repeated_patterns()
```

searches for repeated sequences of 3 to 5 characters.

For every repeated sequence, the program records the positions where it occurs.

The function:

```cpp
calculate_distances()
```

calculates the distance between consecutive occurrences of each repeated sequence.

For example:

```text
ABC occurs at positions:

20
50

Distance = 50 - 20 = 30
```

The program then factors these distances.

For example:

```text
30 = 2 × 3 × 5
```

Possible key lengths are therefore:

```text
2
3
5
6
10
15
30
```

The program limits candidate key lengths to a maximum of 20.

The factor frequencies are used to rank candidate key lengths.

---

## 3. Index of Coincidence

Kasiski Examination may produce multiple possible key lengths.

The program therefore calculates the **Index of Coincidence (IC)** for the groups produced by each candidate key length.

For English text, the typical IC is approximately:

```text
0.0667
```

The candidate whose average group IC is closest to this value is selected as the estimated key length.

The function used is:

```cpp
calculate_ic()
```

and the helper function:

```cpp
average_ic_for_key_length()
```

calculates the average IC for all groups for a candidate key length.

---

## 4. Splitting the Ciphertext

Once the estimated key length is known, the ciphertext is divided into groups.

For example, for a key length of 4:

```text
Ciphertext positions:

1 2 3 4 5 6 7 8 9 10 11 12 ...

Group 1:
1 5 9 ...

Group 2:
2 6 10 ...

Group 3:
3 7 11 ...

Group 4:
4 8 12 ...
```

Each group corresponds to one position of the repeating Vigenère key.

Therefore, each group behaves approximately like a **Caesar cipher encrypted using one fixed shift**.

The function responsible for this is:

```cpp
split_into_groups()
```

---

## 5. Frequency Analysis

The function:

```cpp
frequency_analysis()
```

calculates the relative frequency of each letter from `A` to `Z` for every group.

Example:

```text
Group 1

A: 0.103
B: 0.000
C: 0.034
D: 0.034
...
```

The frequencies are compared with standard English letter frequencies.

---

## 6. Finding the Caesar Shift

For every group, the function:

```cpp
find_shift()
```

tests all 26 possible Caesar shifts.

For every candidate shift, the program calculates a **Chi-Squared statistic** comparing the resulting distribution with standard English letter frequencies.

The shift with the smallest Chi-Squared value is selected as the most probable shift.

The shift is converted into a key character:

```text
0  -> A
1  -> B
2  -> C
...
25 -> Z
```

For example:

```text
Best shift = 10

10 -> K
```

Therefore the corresponding key character is:

```text
K
```

---

## 7. Recovering the Key

The function:

```cpp
find_key()
```

performs frequency analysis on every group and combines the recovered shifts.

For example:

```text
Group 1  -> A
Group 2  -> M
Group 3  -> B
Group 4  -> R
Group 5  -> O
...
```

produces:

```text
AMBROISETHOMAS
```

---

## 8. Decryption

The recovered key is used by:

```cpp
vigenere_decrypt()
```

to decrypt the complete ciphertext.

The resulting plaintext is displayed in the terminal and written to:

```text
outputs/decryption.txt
```

---

## 9. Verification

The recovered plaintext is encrypted again using:

```cpp
vigenere_encrypt()
```

The resulting ciphertext is compared with the original cleaned ciphertext using:

```cpp
verify()
```

If they are identical:

```text
[+] SUCCESS: Re-encryption matches the original ciphertext.
```

This confirms that the encryption and decryption operations are consistent for the recovered key.

### Important Note

A successful re-encryption check does **not independently prove that the recovered key is the original encryption key**.

It proves that:

```text
Recovered Plaintext
        +
Recovered Key
        ↓
Original Ciphertext
```

The readability of the recovered plaintext is therefore also used as evidence that the cryptanalysis succeeded.

---

# User-Defined Functions

The implementation provides the following functions required by the assignment:

| Function                   | Purpose                                                          |
| -------------------------- | ---------------------------------------------------------------- |
| `clean_ciphertext()`       | Removes non-alphabetic characters and converts text to uppercase |
| `find_repeated_patterns()` | Finds repeated sequences in the ciphertext                       |
| `calculate_distances()`    | Calculates distances between repeated occurrences                |
| `find_factors()`           | Finds factors of Kasiski distances                               |
| `kasiski_analysis()`       | Produces candidate key lengths                                   |
| `calculate_ic()`           | Calculates Index of Coincidence                                  |
| `split_into_groups()`      | Divides ciphertext according to key length                       |
| `frequency_analysis()`     | Calculates A-Z frequency for a group                             |
| `find_shift()`             | Estimates the Caesar shift using Chi-Squared analysis            |
| `find_key()`               | Combines shifts to recover the Vigenère key                      |
| `vigenere_decrypt()`       | Decrypts ciphertext                                              |
| `vigenere_encrypt()`       | Re-encrypts plaintext                                            |
| `verify()`                 | Verifies the re-encrypted ciphertext                             |

---

# Example Result

For a sample ciphertext, the program may produce:

```text
[*] Kasiski candidate key lengths (ranked): 2, 7, 14, 3, 6

[*] Candidate evaluation:
    key length  2 -> avg IC = 0.0448
    key length  7 -> avg IC = 0.0508
    key length 14 -> avg IC = 0.0644
    key length  3 -> avg IC = 0.0430
    key length  6 -> avg IC = 0.0441

[*] Estimated Key Length: 14

[*] Recovered Probable Key: AMBROISETHOMAS

[*] Recovered Plaintext:
DOYOUKNOWTHELANDWHERETHEORANGETREEBLOSSOMSTHECOUNTRYOFGOLDENFRUITS...
```

Verification:

```text
--- VERIFICATION ---
[+] SUCCESS: Re-encryption matches the original ciphertext.
```

The complete result is stored in:

```text
outputs/decryption.txt
```

---

# Limitations

The cryptanalysis is statistical and therefore depends on the ciphertext containing enough English text.

Accuracy may decrease when:

* The ciphertext is very short.
* The plaintext is not English.
* The plaintext has an unusual letter distribution.
* Repeated patterns are insufficient for Kasiski Examination.
* The estimated key length is incorrect.

For this implementation, candidate key lengths are limited to:

```text
1–20
```

---

## Technologies Used

* **Language:** C++
* **Standard:** C++17
* **Input:** `data/input.txt`
* **Output:** `outputs/decryption.txt`
* **Techniques:** Kasiski Examination, Index of Coincidence, Frequency Analysis, Chi-Squared Testing, Vigenère Cryptanalysis
