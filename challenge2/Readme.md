# Writeup — Challenge 2: "Stack of Lies"

**Category:** Reverse Engineering  
**Difficulty:** Hard  
**Flag:** `Eclipse{sm_c0d3_r0ck}`

---

## Step 1 — Run the binary

```bash
./chall2
Attempt 1: test
Wrong.
Attempt 2: test
Wrong.
Attempt 3: test
Wrong.
```

Three attempts, all fail. Interesting — why three attempts?

---

## Step 2 — Static analysis (Ghidra / strings)

First, run `strings` on the binary:

```bash
strings chall2
```

No plaintext flag visible. The flag is hidden.

Opening in Ghidra, we find `main()` calls three checks:

- **Attempt 1** → `fake_check1()` — computes a hash and compares to `0xDEADBEEF`. Impossible to satisfy with short printable input. Red herring.
- **Attempt 2** → `fake_check2()` — checks `strlen(input) == 99`. Another red herring.
- **Attempt 3** → calls `dispatch(input)` — a **function pointer**.

Static analysis shows `dispatch` is initialized to `fake_check1`. So attempt 3 looks like another fake check. But wait...

---

## Step 3 — Spot the constructor

Looking more carefully in Ghidra, there is a function outside `main()` with a special attribute:

```c
__attribute__((constructor))
static void setup(void) {
    dispatch = real_check;
}
```

`__attribute__((constructor))` means this function runs **before `main()`** — the dynamic linker calls it at startup. Static analysis of `main()` alone misses this completely.

So by the time attempt 3 runs, `dispatch` actually points to `real_check()`, not `fake_check1()`.

---

## Step 4 — Analyze real_check()

```c
static int real_check(const char *s) {
    if (strlen(s) != BLOB_LEN) return 0;

    unsigned char decrypted[BLOB_LEN];
    for (int i = 0; i < BLOB_LEN; i++)
        decrypted[i] = ENCRYPTED_BLOB[i] ^ 0x5A;

    return memcmp(s, decrypted, BLOB_LEN) == 0;
}
```

The real check:
1. Verifies input length is 12
2. Decrypts a blob by XORing each byte with `0x5A`
3. Compares decrypted blob to our input

The encrypted blob is visible in the binary:
```
0x29,0x37,0x05,0x39,0x6A,0x3E,0x69,0x05,0x28,0x6A,0x39,0x31
```

---

## Step 5 — Write the solver

XOR is its own inverse — XOR with the same key decrypts:

```python
ENCRYPTED_BLOB = bytes([
    0x29,0x37,0x05,0x39,0x6A,0x3E,0x69,0x05,0x28,0x6A,0x39,0x31
])

flag = ''.join(chr(b ^ 0x5A) for b in ENCRYPTED_BLOB)
print("Eclipse{" + flag + "}")
```

```bash
python3 solve2.py
Eclipse{sm_c0d3_r0ck}
```

---

## Step 6 — Verify

```bash
./chall2
Attempt 1: wrong
Wrong.
Attempt 2: wrong
Wrong.
Attempt 3: sm_c0d3_r0ck
Eclipse{sm_c0d3_r0ck}
```

---

## Alternative approach — GDB (dynamic analysis)

If you missed the XOR blob statically, GDB works too:

```bash
gdb ./chall2
(gdb) break real_check
(gdb) run
# enter anything for attempts 1 and 2
# breakpoint hits on attempt 3
(gdb) x/12xb decrypted   # read decrypted bytes after XOR
```

This dumps the flag directly from memory.

---

## Key Takeaways

> `__attribute__((constructor))` runs before `main()` — always check for constructor/destructor functions in Ghidra's function list, not just `main()`.

> When you see a function pointer in `main()`, trace every place it gets assigned — not just its initial value.

> XOR encryption with a fixed single-byte key is trivially reversible: just XOR the ciphertext with the same key.

**Flag:** `Eclipse{sm_c0d3_r0ck}`