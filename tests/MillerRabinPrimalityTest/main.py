
import secrets
import time
from sympy import isprime

# Assignment parameters
BIT_LENGTH = 512

# 20 specified Miller-Rabin bases
BASES = [
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
    31, 37, 41, 43, 47, 53, 59, 61, 67, 71
]


def miller_rabin(n, bases):
    """Test whether n passes Miller-Rabin for all given bases."""
    if n < 2:
        return False
    if n in (2, 3):
        return True
    if n % 2 == 0:
        return False

    # Express n - 1 = d * 2^s, where d is odd
    d = n - 1
    s = 0

    while d % 2 == 0:
        d //= 2
        s += 1

    for base in bases:
        # Map each specified base into [2, n - 2]
        a = 2 + (base - 2) % (n - 3)

        x = pow(a, d, n)

        if x == 1 or x == n - 1:
            continue

        for _ in range(s - 1):
            x = pow(x, 2, n)

            if x == n - 1:
                break
        else:
            return False

    return True


def generate_probable_prime(bits, bases):
    candidates_tested = 0
    start_time = time.perf_counter()

    while True:
        # Generate a random integer of exactly 'bits' bits.
        # Set the highest bit and lowest bit to 1.
        candidate = secrets.randbits(bits)
        candidate |= (1 << (bits - 1)) | 1

        candidates_tested += 1

        if miller_rabin(candidate, bases):
            elapsed = time.perf_counter() - start_time
            return candidate, candidates_tested, elapsed


def print_table(rows):
    """Display experimental results in a formatted table."""
    print("\n" + "=" * 90)
    print("             EXPERIMENTAL RESULTS")
    print("=" * 90)

    print(f"{'Parameter':<38} | Value")
    print("-" * 90)

    for parameter, value in rows:
        print(f"{parameter:<38} | {value}")

    print("=" * 90)


def main():
    print("Generating a 512-bit probable prime...")
    print("Please wait.\n")

    prime, count, elapsed = generate_probable_prime(
        BIT_LENGTH, BASES
    )

    # Verify the result
    actual_bits = prime.bit_length()
    mr_result = miller_rabin(prime, BASES)
    independent_result = bool(isprime(prime))

    # Experimental results table
    results = [
        ("Assigned bit length", f"{BIT_LENGTH} bits"),
        ("Generated prime candidate", prime),
        ("Actual bit length of candidate", f"{actual_bits} bits"),
        ("Number of candidates tested", count),
        ("Number of Miller-Rabin rounds", len(BASES)),
        ("Total execution time", f"{elapsed:.6f} seconds"),
    ]

    print_table(results)

    # Verification results
    print("\nVERIFICATION RESULTS")
    print("-" * 55)
    print(f"Specified bases: {BASES}")
    print(f"Bit length is exactly 512: {actual_bits == BIT_LENGTH}")
    print(f"Passes all Miller-Rabin rounds: {mr_result}")
    print(f"Independent SymPy check: {independent_result}")

    # Final validation
    assert actual_bits == BIT_LENGTH
    assert mr_result
    assert independent_result

    print("\nFINAL RESULT: ALL CHECKS PASSED")
    print("The generated number is a probable prime.")


if __name__ == "__main__":
    main()

