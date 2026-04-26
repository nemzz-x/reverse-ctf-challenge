# Writeup — Challenge 1: "Broken Clock"

**Category:** Reverse Engineering  
**Difficulty:** Medium  
**Flag:** `Eclipse{rev_easy}`

---

## Step 1 — Run the binary

```bash
./chall1
Enter the flag body: test
Wrong.
```

The binary takes input and checks it against something. Time to look inside.

---

## Step 2 — Static analysis

Opening the binary in Ghidra reveals three key elements:

**A hardcoded TARGET array:**
```c
static const unsigned char TARGET[] = {
    0x75,0xE6,0xF1,0x38,0xE6,0x7A,0x54,0x12
};
```

**A transformation loop applied to our input:**
```c
for (size_t i = 0; i < FLAG_LEN; i++)
    buf[i] = ((buf[i] * 31 + 17) ^ 0xAA) & 0xFF;
```

**A comparison against TARGET:**
```c
memcmp(buf, TARGET, FLAG_LEN) == 0
```

So the program transforms our input byte by byte and compares to TARGET. If they match, the flag is printed.

---

## Step 3 — Understand the cipher

The transform is:

```
f(x) = ((x * 31 + 17) ^ 0xAA) & 0xFF
```

This is a **substitution cipher** — each input byte maps to exactly one output byte. That means it is **invertible**: for every possible byte value (0–255) we can precompute `f(x)` and build a reverse lookup table.

We do not need to bruteforce anything. We just need to find which input byte produces each TARGET byte.

---

## Step 4 — Write the solver

```python
def encode(b):
    return ((b * 31 + 17) ^ 0xAA) & 0xFF

# Build reverse map: ciphertext byte -> plaintext byte
inv = {encode(i): i for i in range(256)}

TARGET = bytes([0x75, 0xE6, 0xF1, 0x38, 0xE6, 0x7A, 0x54, 0x12])

flag = ''.join(chr(inv[b]) for b in TARGET)
print("Eclipse{" + flag + "}")
```

Run it:

```bash
python3 solve1.py
Eclipse{rev_easy}
```

---

## Step 5 — Verify against the binary

```bash
./chall1
Enter the flag body: rev_easy
Eclipse{rev_easy}
```

---

## Key Takeaway

> When a binary applies a byte-by-byte mathematical transform and compares to a fixed target, always ask: **is this invertible?**  
> If yes, build a reverse lookup table — no bruteforce needed.

**Flag:** `Eclipse{rev_easy}`