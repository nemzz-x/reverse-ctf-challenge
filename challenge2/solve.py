# solve2.py — two approaches

# ── Approach A: static (read the encrypted blob, XOR with 0x5A) ──────────
ENCRYPTED_BLOB = bytes([
    0x29,0x37,0x05,0x39,0x6A,0x3E,0x69,0x05,0x28,0x6A,0x39,0x31
])

flag = ''.join(chr(b ^ 0x5A) for b in ENCRYPTED_BLOB)
print("Eclipse{" + flag + "}")