def encode(b):
    return ((b * 31 + 17) ^ 0xAA) & 0xFF

inv = {encode(i): i for i in range(256)}
TARGET = bytes([0x75,0xE6,0xF1,0x38,0xE6,0x7A,0x54,0x12])
flag = ''.join(chr(inv[b]) for b in TARGET)
print("Eclipse{" + flag + "}")