from brute_force_dictionary import brute_force, dictionary_scoring
from chi_square_attack import chi_square_attack
from shift_cipher import encrypt


dictionary = set()

with open("shift_cipher_attack/dictionary/english_words.txt", "r") as file:
    for word in file:
        dictionary.add(word.strip().lower())


def load_test_cases():
    test_file = "shift_cipher_attack/testcases/testcases.txt"

    test_cases = []

    with open(test_file, "r") as file:
        lines = [line.strip() for line in file if line.strip()]

    i = 0

    while i < len(lines):
        if lines[i].startswith("Test Case"):
            test_case = lines[i]

            actual_key = int(
                lines[i + 1].split(":", 1)[1].strip()
            )

            plaintext = (
                lines[i + 2].split(":", 1)[1].strip()
            )

            ciphertext = encrypt(plaintext, actual_key)

            test_cases.append(
                (test_case, plaintext, actual_key, ciphertext)
            )

            i += 3
        else:
            i += 1

    return test_cases


def run_test_cases(choice):

    test_cases = load_test_cases()

    output_file = "shift_cipher_attack/outputs/results.txt"

    results = []

    for test_case, plaintext, actual_key, ciphertext in test_cases:

        print(f"\n{'=' * 80}")
        print(test_case)
        print(f"Actual Key: {actual_key}")
        print(f"Ciphertext: {ciphertext}")

        if choice == "1":

            print("\n--- Brute Force ---")

            brute_results = brute_force(ciphertext)

            for key, decrypted_text in brute_results:
                print(f"Key {key}: {decrypted_text}")

        elif choice == "2":

            dictionary_key, dictionary_plaintext, dictionary_score = (
                dictionary_scoring(ciphertext, dictionary)
            )

            dictionary_correct = dictionary_key == actual_key

            print("\n--- Dictionary Scoring ---")
            print(f"Predicted Key: {dictionary_key}")
            print(f"Plaintext: {dictionary_plaintext}")
            print(f"Dictionary Score: {dictionary_score}")
            print(f"Dictionary Correct: {dictionary_correct}")

            results.append(
                (
                    test_case,
                    actual_key,
                    dictionary_key,
                    dictionary_correct
                )
            )

        elif choice == "3":

            chi_key, chi_plaintext, chi_score = (
                chi_square_attack(ciphertext)
            )

            chi_square_correct = chi_key == actual_key

            print("\n--- Chi-Square Analysis ---")
            print(f"Predicted Key: {chi_key}")
            print(f"Plaintext: {chi_plaintext}")
            print(f"Chi-Square Score: {chi_score}")
            print(f"Chi-Square Correct: {chi_square_correct}")

            results.append(
                (
                    test_case,
                    actual_key,
                    chi_key,
                    chi_square_correct
                )
            )

        elif choice == "4":

            dictionary_key, dictionary_plaintext, dictionary_score = (
                dictionary_scoring(ciphertext, dictionary)
            )

            chi_key, chi_plaintext, chi_score = (
                chi_square_attack(ciphertext)
            )

            dictionary_correct = dictionary_key == actual_key
            chi_square_correct = chi_key == actual_key

            print("\n--- Dictionary Scoring ---")
            print(f"Predicted Key: {dictionary_key}")
            print(f"Plaintext: {dictionary_plaintext}")
            print(f"Dictionary Score: {dictionary_score}")
            print(f"Dictionary Correct: {dictionary_correct}")

            print("\n--- Chi-Square Analysis ---")
            print(f"Predicted Key: {chi_key}")
            print(f"Plaintext: {chi_plaintext}")
            print(f"Chi-Square Score: {chi_score}")
            print(f"Chi-Square Correct: {chi_square_correct}")

            results.append(
                (
                    test_case,
                    actual_key,
                    dictionary_key,
                    chi_key,
                    dictionary_correct,
                    chi_square_correct,
                    dictionary_score,
                    chi_score
                )
            )

    if choice == "1":
        return

    with open(output_file, "w") as file:

        file.write("SHIFT CIPHER CRYPTANALYSIS RESULTS\n\n")

        if choice == "2":

            file.write(
                "Test Case | Actual Key | Dictionary Key | "
                "Dictionary Correct\n"
            )

            file.write("-" * 70 + "\n")

            for result in results:

                test_case, actual_key, dictionary_key, correct = result

                file.write(
                    f"{test_case} | "
                    f"{actual_key} | "
                    f"{dictionary_key} | "
                    f"{correct}\n"
                )

        elif choice == "3":

            file.write(
                "Test Case | Actual Key | Chi-Square Key | "
                "Chi-Square Correct\n"
            )

            file.write("-" * 70 + "\n")

            for result in results:

                test_case, actual_key, chi_key, correct = result

                file.write(
                    f"{test_case} | "
                    f"{actual_key} | "
                    f"{chi_key} | "
                    f"{correct}\n"
                )

        elif choice == "4":

            file.write(
                "Test Case | Actual Key | Dictionary Key | "
                "Chi-Square Key | Dictionary Correct | "
                "Chi-Square Correct\n"
            )

            file.write("-" * 100 + "\n")

            for result in results:

                (
                    test_case,
                    actual_key,
                    dictionary_key,
                    chi_key,
                    dictionary_correct,
                    chi_square_correct,
                    dictionary_score,
                    chi_score
                ) = result

                file.write(
                    f"{test_case} | "
                    f"{actual_key} | "
                    f"{dictionary_key} | "
                    f"{chi_key} | "
                    f"{dictionary_correct} | "
                    f"{chi_square_correct}\n"
                )

            file.write("\nDetailed Scores\n\n")

            for result in results:

                (
                    test_case,
                    actual_key,
                    dictionary_key,
                    chi_key,
                    dictionary_correct,
                    chi_square_correct,
                    dictionary_score,
                    chi_score
                ) = result

                file.write(
                    f"{test_case}\n"
                    f"Actual Key: {actual_key}\n"
                    f"Dictionary Key: {dictionary_key}\n"
                    f"Chi-Square Key: {chi_key}\n"
                    f"Dictionary Score: {dictionary_score}\n"
                    f"Chi-Square Score: {chi_score}\n"
                    f"Dictionary Correct: {dictionary_correct}\n"
                    f"Chi-Square Correct: {chi_square_correct}\n\n"
                )

    print(f"\nResults saved to: {output_file}")


print("\nChoose the type of attack:")
print("1. Brute Force")
print("2. Dictionary Scoring")
print("3. Chi-Square Analysis")
print("4. All")

choice = input("Enter your choice: ")

if choice in ["1", "2", "3", "4"]:
    run_test_cases(choice)
else:
    print("Invalid choice")