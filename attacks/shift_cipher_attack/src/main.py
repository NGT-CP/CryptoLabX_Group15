from brute_force_dictionary import brute_force, dictionary_scoring

dictionary = set()

with open("shift_cipher_attack/dictionary/english_words.txt", "r") as file:
    for word in file:
        dictionary.add(word.strip().lower())


txt = input("Enter the ciphertext: ")

print("Choose the type of attack:")
print("1. Brute Force")
print("2. Dictionary Scoring")
print("3. Chi-Square Analysis")
print("4. All")

x = input("Enter your choice: ")


if x == "1":
    results = brute_force(txt)

    for key, plaintext in results:
        print(f"Key {key}: {plaintext}")


elif x == "2":
    key, plaintext, score = dictionary_scoring(txt, dictionary)

    print(f"Predicted Key: {key}")
    print(f"Plaintext: {plaintext}")
    print(f"Dictionary Score: {score}")


elif x == "3":
    pass


elif x == "4":
    pass


else:
    print("Invalid choice")