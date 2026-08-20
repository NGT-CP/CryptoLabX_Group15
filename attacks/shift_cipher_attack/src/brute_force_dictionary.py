from shift_cipher import decrypt


def brute_force(ciphertext):
    results = []

    for key in range(26):
        plaintext = decrypt(ciphertext, key)
        results.append((key, plaintext))

    return results


def dictionary_scoring(ciphertext, dictionary):
    results = []

    for key in range(26):
        plaintext = decrypt(ciphertext, key)
        words = plaintext.lower().split()

        score = 0

        for word in words:
            if word in dictionary:
                score += 1

        results.append((key, plaintext, score))

    best_key, best_plaintext, best_score = max(results, key=lambda x: x[2])

    return best_key, best_plaintext, best_score