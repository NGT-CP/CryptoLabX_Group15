from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
import os

BLOCK_SIZE = 16
SECRET_KEY = os.urandom(16)
oracle_queries = 0


def pkcs7_pad(data):
    n = BLOCK_SIZE - (len(data) % BLOCK_SIZE)
    return data + bytes([n]) * n


def pkcs7_unpad(data):
    n = data[-1]

    if n < 1 or n > BLOCK_SIZE:
        raise ValueError("Invalid padding")

    if data[-n:] != bytes([n]) * n:
        raise ValueError("Invalid padding")

    return data[:-n]


def valid_padding(data):
    if len(data) == 0 or len(data) % BLOCK_SIZE != 0:
        return False

    n = data[-1]

    if n < 1 or n > BLOCK_SIZE:
        return False

    return data[-n:] == bytes([n]) * n


def aes_cbc_encrypt(key, iv, plaintext):
    cipher = Cipher(
        algorithms.AES(key),
        modes.CBC(iv)
    ).encryptor()

    padded = pkcs7_pad(plaintext)

    return cipher.update(padded) + cipher.finalize()


def padding_oracle(iv, ciphertext):
    global oracle_queries
    oracle_queries += 1

    try:
        cipher = Cipher(
            algorithms.AES(SECRET_KEY),
            modes.CBC(iv)
        ).decryptor()

        plaintext = cipher.update(ciphertext) + cipher.finalize()

        return valid_padding(plaintext)

    except Exception:
        return False


def recover_block(iv, previous_block, target_block, block_number):
    recovered = bytearray(BLOCK_SIZE)

    print("\n" + "=" * 60)
    print(f"ATTACKING P{block_number}")
    print("=" * 60)

    for position in range(BLOCK_SIZE - 1, -1, -1):

        padding = BLOCK_SIZE - position
        modified = bytearray(previous_block)

        for j in range(BLOCK_SIZE - 1, position, -1):
            modified[j] = (
                previous_block[j]
                ^ recovered[j]
                ^ padding
            )

        found = None

        for guess in range(256):

            modified[position] = guess

            valid = padding_oracle(
                iv,
                bytes(modified) + target_block
            )

            if not valid:
                continue

            # Avoid the common false positive when attacking
            # the last byte.
            if position == BLOCK_SIZE - 1:
                check = bytearray(modified)
                check[BLOCK_SIZE - 2] ^= 1

                if not padding_oracle(
                    iv,
                    bytes(check) + target_block
                ):
                    continue

            found = guess
            break

        if found is None:
            raise RuntimeError(
                f"Failed to recover byte {position} of P{block_number}"
            )

        recovered[position] = (
            found
            ^ previous_block[position]
            ^ padding
        )

        print(
            f"byte {position:2d} | "
            f"padding {padding:02X} | "
            f"guess {found:02X} | "
            f"plaintext {recovered[position]:02X} "
            f"({chr(recovered[position]) if 32 <= recovered[position] <= 126 else '.'})"
        )

    print(f"P{block_number} = {recovered.hex()}")
    print(
        "ASCII =",
        ''.join(
            chr(x) if 32 <= x <= 126 else '.'
            for x in recovered
        )
    )

    return bytes(recovered)


def padding_oracle_attack(iv, ciphertext):
    blocks = [
        ciphertext[i:i + BLOCK_SIZE]
        for i in range(0, len(ciphertext), BLOCK_SIZE)
    ]

    recovered_plaintext = bytearray()

    for i, target_block in enumerate(blocks):

        if i == 0:
            previous_block = iv
        else:
            previous_block = blocks[i - 1]

        recovered_block = recover_block(
            iv,
            previous_block,
            target_block,
            i + 1
        )

        recovered_plaintext.extend(recovered_block)

    return pkcs7_unpad(bytes(recovered_plaintext))


def show_original_cbc_structure(iv, ciphertext):
    blocks = [
        ciphertext[i:i + BLOCK_SIZE]
        for i in range(0, len(ciphertext), BLOCK_SIZE)
    ]

    print("\n" + "=" * 60)
    print("CBC STRUCTURE")
    print("=" * 60)

    print(f"IV = {iv.hex()}")

    for i, block in enumerate(blocks):
        if i == 0:
            previous = iv
            previous_name = "IV"
        else:
            previous = blocks[i - 1]
            previous_name = f"C{i}"

        print(f"\nC{i + 1} = {block.hex()}")
        print(f"P{i + 1} uses {previous_name}")
        print(f"Previous = {previous.hex()}")

    print()


def main():
    plaintext = (
        b"Padding Oracle Attack using AES CBC. "
        b"This is the assignment."
    )

    # ---------------------------------------------------------
    # Challenge creation
    # The key stays inside the oracle.
    # The attack never receives SECRET_KEY.
    # ---------------------------------------------------------

    iv = os.urandom(BLOCK_SIZE)

    ciphertext = aes_cbc_encrypt(
        SECRET_KEY,
        iv,
        plaintext
    )

    print("=" * 60)
    print("PADDING ORACLE ATTACK")
    print("=" * 60)

    print("\nOriginal plaintext:")
    print(plaintext.decode())

    print("\nIV:")
    print(iv.hex())

    print("\nCiphertext:")
    print(ciphertext.hex())

    show_original_cbc_structure(iv, ciphertext)

    # ---------------------------------------------------------
    # Attack
    # ---------------------------------------------------------

    recovered = padding_oracle_attack(
        iv,
        ciphertext
    )

    print("\n" + "=" * 60)
    print("FINAL RESULT")
    print("=" * 60)

    print("\nRecovered plaintext:")
    print(recovered.decode())

    print("\nOracle queries:")
    print(oracle_queries)

    print("\nCorrect:")
    print(recovered == plaintext)


if __name__ == "__main__":
    main()