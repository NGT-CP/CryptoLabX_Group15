
from pathlib import Path
from collections import Counter
import numpy as np

FILES = {
    "Linux /dev/urandom": "data/linux_urandom.bin",
    "OpenSSL rand": "data/openssl_rand.bin",
    "Windows CNG/.NET": "data/Windows_cng.bin",
}


def analyze_file(name, filename):
    path = Path(filename)

    if not path.exists():
        print(f"\n{name}: FILE NOT FOUND: {path}")
        return

    data = path.read_bytes()

    if len(data) == 0:
        print(f"\n{name}: EMPTY FILE")
        return

    b = np.frombuffer(data, dtype=np.uint8)
    n = len(b)

    print("\n" + "=" * 65)
    print(name)
    print("=" * 65)
    print(f"File size: {n:,} bytes")

    # 1. Byte-frequency distribution
    counts = np.bincount(b, minlength=256)
    expected = n / 256

    chi_square = np.sum((counts - expected) ** 2 / expected)

    print("\n1. BYTE-FREQUENCY DISTRIBUTION")
    print(f"Expected count per byte value: {expected:.2f}")
    print(f"Minimum observed frequency: {counts.min()}")
    print(f"Maximum observed frequency: {counts.max()}")
    print(f"Chi-square statistic: {chi_square:.4f}")

    # 2. Bit distribution
    bits = np.unpackbits(b)
    zeros = int(np.count_nonzero(bits == 0))
    ones = int(np.count_nonzero(bits == 1))

    print("\n2. BIT DISTRIBUTION")
    print(f"Total bits: {len(bits):,}")
    print(f"Zero bits: {zeros:,} ({100 * zeros / len(bits):.4f}%)")
    print(f"One bits:  {ones:,} ({100 * ones / len(bits):.4f}%)")

    # 3. Repeated patterns
    adjacent_equal = int(np.count_nonzero(b[1:] == b[:-1]))

    # Count repeated non-overlapping 4-byte blocks
    usable = n - n % 4
    blocks = [data[i:i+4] for i in range(0, usable, 4)]
    block_counts = Counter(blocks)

    repeated_block_types = sum(
        count > 1 for count in block_counts.values()
    )
    extra_block_occurrences = sum(
        count - 1 for count in block_counts.values() if count > 1
    )

    # Longest consecutive run of identical bytes
    longest_run = 1
    current_run = 1

    for i in range(1, n):
        if b[i] == b[i - 1]:
            current_run += 1
            longest_run = max(longest_run, current_run)
        else:
            current_run = 1

    print("\n3. REPEATED PATTERNS")
    print(f"Adjacent equal-byte pairs: {adjacent_equal}")
    print(f"Repeated 4-byte block types: {repeated_block_types}")
    print(f"Extra occurrences of repeated blocks: {extra_block_occurrences}")
    print(f"Longest identical-byte run: {longest_run}")

    # 4. Serial correlation
    if n > 1 and np.std(b[:-1]) > 0 and np.std(b[1:]) > 0:
        correlation = np.corrcoef(
            b[:-1].astype(np.float64),
            b[1:].astype(np.float64)
        )[0, 1]
    else:
        correlation = float("nan")

    print("\n4. SERIAL CORRELATION")
    print(f"Lag-1 correlation: {correlation:.8f}")


for name, filename in FILES.items():
    analyze_file(name, filename)
