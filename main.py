import os
import logging
from collections import Counter

LOG_FILE = "outputs/cryptolab.log"

logging.basicConfig(
    filename=LOG_FILE,
    level=logging.INFO,
    format="%(asctime)s - %(message)s"
)

logging.info("Program Started")

def write_log(message):
    logging.info(message)

def show_menu():
    print("\n========== CryptoLabX ==========")
    print("1. Encrypt")
    print("2. Decrypt")
    print("3. Attack")
    print("4. Analyze")
    print("5. Exit")
    print("================================")

def coming_soon(feature):
    write_log(f"{feature} Selected")
    print(f"\n{feature} module is under development.\nComing Soon!\n")

def analyze_file():
    filename = input("\nEnter text file name (example: s1.txt): ")
    filepath = os.path.join("datasets", filename)

    if not os.path.exists(filepath):
        print("\nFile does not exist!\n")
        write_log(f"File Not Found : {filename}")
        return

    with open(filepath, "r", encoding="utf-8") as file:
        data = file.read()

    print("\n========== File Analysis ==========")
    print(f"File Name           : {filename}")
    print(f"Characters          : {len(data)}")
    print(f"Words               : {len(data.split())}")
    print(f"Lines               : {len(data.splitlines())}")
    print(f"Unique Characters   : {len(set(data))}")

    letters = [c.lower() for c in data if c.isalpha()]
    frequency = Counter(letters)

    print("\nLetter Frequency")
    print("----------------")

    for letter in sorted(frequency):
        print(f"{letter} : {frequency[letter]}")

    print("===================================\n")

    write_log(f"Analyzed File : {filename}")

while True:
    show_menu()

    choice = input("Enter your choice: ")

    if choice == "1":
        coming_soon("Encrypt")

    elif choice == "2":
        coming_soon("Decrypt")

    elif choice == "3":
        coming_soon("Attack")

    elif choice == "4":
        analyze_file()

    elif choice == "5":
        write_log("Program Terminated")
        print("\nThank you for using CryptoLabX.\n")
        break

    else:
        write_log("Invalid Menu Choice")
        print("\nInvalid choice. Please try again.\n")
