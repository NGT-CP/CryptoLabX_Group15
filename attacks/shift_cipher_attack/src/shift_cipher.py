def encrypt(text, key):
    result = ""

    for char in text:
        if char.isalpha():
            start = ord('A') if char.isupper() else ord('a')
            result += chr((ord(char) - start + key) % 26 + start)
        else:
            result += char

    return result


def decrypt(text, key):
    return encrypt(text, -key)