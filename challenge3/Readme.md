# Writeup — Challenge 3: "Shadow Table"

**Category:** Reverse Engineering  
**Difficulty:** Hard  
**Flag:** `Eclipse{m4tr1x_br34k}`

---

## Step 1 — Run the binary

```bash
./chall
Enter flag: test
Wrong.
```

Simple enough. Time to look inside.

---

## Step 2 — First look with strings

```bash
strings chall
```

No readable flag. We see some suspicious strings like `Wrong.` and `Eclipse{%s}` but nothing useful. The table data looks like random bytes.

---

## Step 3 — Open in Ghidra

Loading the binary in Ghidra reveals several things:

**A fake check function — red herring:**
```c
static int fake_validate(const char *s) {
    uint32_t h = 5381;
    while (*s) h = ((h << 5) + h) ^ (uint8_t)*s++;
    return h == 0xDEADBEEF;  // impossible to satisfy
}
```
This is djb2 hashing comparing to `0xDEADBEEF`. It always fails — ignore it.

**Two suspicious byte arrays:**
```c
static const uint8_t TABLE_LO[128] = { ... };  // first 128 bytes
static const uint8_t TABLE_HI[128] = { ... };  // last 128 bytes
```

**A constructor that assembles them:**
```c
__attribute__((constructor))
static void init_table(void) {
    memcpy(MATRIX,     TABLE_LO, 128);
    memcpy(MATRIX+128, TABLE_HI, 128);
}
```

The table is split into two halves and only assembled at runtime — this is intentional to slow down static analysis.

**The real validation:**
```c
for (int i = 0; i < FLAG_LEN; i++)
    out[i] = MATRIX[(uint8_t)input[i]];

memcmp(out, TARGET, FLAG_LEN) == 0
```

Each input byte is used as an index into MATRIX. The result must match TARGET.

---

## Step 4 — Spot the anti-debug tricks

The binary has two traps:

**Trap 1 — ptrace/sysctl check:**
```c
static int being_debugged(void) {
    // macOS: checks P_TRACED flag via sysctl
    // Linux: ptrace(PTRACE_TRACEME) returns -1 if already traced
}
```
If GDB is attached, this returns 1 and the binary exits with "Wrong."

**Trap 2 — timing check:**
```c
static int timing_check(void) {
    // measures time for a busy loop
    // returns 1 if elapsed > 2000ms (debugger slows things down)
}
```

**How to bypass:** Solve statically — never run under a debugger at all. The solver script does not touch the binary.

---

## Step 5 — Reconstruct the matrix

The table is built with `random.seed(0xCAFE)` — visible from static analysis of the two half-arrays. We replicate it in Python:

```python
import random
random.seed(0xCAFE)

matrix = list(range(256))
random.shuffle(matrix)
```

---

## Step 6 — Build the inverse and solve

Since MATRIX maps `plaintext -> ciphertext`, we need the reverse: `ciphertext -> plaintext`.

```python
import random
random.seed(0xCAFE)

matrix = list(range(256))
random.shuffle(matrix)

# Build inverse lookup
inv = [0] * 256
for i, v in enumerate(matrix):
    inv[v] = i

TARGET = bytes([
    0xDB,0x99,0xC6,0xF5,0x55,0xD5,0x7A,0x6A,
    0xF5,0x92,0x99,0x84
])

flag = ''.join(chr(inv[b]) for b in TARGET)
print("Eclipse{" + flag + "}")
```

```bash
python3 solve.py
Eclipse{m4tr1x_br34k}
```

---

## Step 7 — Verify

```bash
./chall
Enter flag: m4tr1x_br34k
Eclipse{m4tr1x_br34k}
```

---

## Key Takeaways

> When you see two split byte arrays assembled by a constructor, always combine them — the split is a deliberate obfuscation trick.

> Anti-debug checks are bypassed by solving **statically** — never run the binary under GDB if it checks for a debugger.

> A 256-entry substitution table is always invertible: build a reverse map from ciphertext byte to plaintext byte.

**Flag:** `Eclipse{m4tr1x_br34k}`