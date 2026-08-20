from shift_cipher import decrypt


english_frequencies = {  
    'a': 8.17,
    'b': 1.49,
    'c': 2.78,
    'd': 4.25,
    'e': 12.70,
    'f': 2.23,
    'g': 2.02,
    'h': 6.09,
    'i': 6.97,
    'j': 0.15,
    'k': 0.77,
    'l': 4.03,
    'm': 2.41,
    'n': 6.75,
    'o': 7.51,
    'p': 1.93,
    'q': 0.10,
    'r': 5.99,
    's': 6.33,
    't': 9.06,
    'u': 2.76,
    'v': 0.98,
    'w': 2.36,
    'x': 0.15,
    'y': 1.97,
    'z': 0.07
}


def chi_square_attack(ciphertext):
    best_key = 0
    best_score = float('inf')

    for key in range(26):
        plaintext = decrypt(ciphertext, key)

        total_letters = 0
        observed = {letter: 0 for letter in english_frequencies}

        for char in plaintext.lower():
            if char in observed:
                observed[char] += 1
                total_letters += 1

        score = 0

        for letter in english_frequencies:
            expected = (english_frequencies[letter] / 100) * total_letters
            observed_count = observed[letter]

            if expected > 0:
                score += ((observed_count - expected) ** 2) / expected

        if score < best_score:
            best_score = score
            best_key = key

    best_plaintext = decrypt(ciphertext, best_key)

    return best_key, best_plaintext, best_score