import random
random.seed(0xCAFE)

matrix = list(range(256))
random.shuffle(matrix)

# Build inverse lookup
inv = [0] * 256
for i, v in enumerate(matrix):
    inv[v] = i

TARGET = bytes([0x87,0x90,0x0F,0xFA,0x4D,0xC2,0xF1,0x09,0xFA,0xCE,0x90,0xF9])
flag = ''.join(chr(inv[b]) for b in TARGET)
print("Eclipse{" + flag + "}")