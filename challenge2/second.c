#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// ── The real flag body (12 chars) ───────────────────────────────────────
// flag = "sm_c0d3_r0ck"
// Each byte is XOR-encrypted with a fixed key (0x5A) to hide it statically

static const unsigned char ENCRYPTED_BLOB[] = {
    // These are the flag bytes XOR'd with 0x5A
    // 's'^0x5A=0x29, 'm'^0x5A=0x37, '_'^0x5A=0x05, 'c'^0x5A=0x39,
    // '0'^0x5A=0x6A, 'd'^0x5A=0x3E, '3'^0x5A=0x69, '_'^0x5A=0x05,
    // 'r'^0x5A=0x28, '0'^0x5A=0x6A, 'c'^0x5A=0x39, 'k'^0x5A=0x31
    0x29,0x37,0x05,0x39,0x6A,0x3E,0x69,0x05,0x28,0x6A,0x39,0x31
};
#define BLOB_LEN 12

// ── Two fake checks (red herrings) ──────────────────────────────────────
static int fake_check1(const char *s) {
    // Looks like it checks something meaningful — it doesn't
    unsigned int h = 0;
    for (int i = 0; s[i]; i++) h = h * 31 + s[i];
    return h == 0xDEADBEEF;   // impossible for short printable input
}

static int fake_check2(const char *s) {
    // Another red herring — hardcoded wrong length
    return strlen(s) == 99;
}

// ── Real check: decrypt blob at runtime, compare to input ───────────────
static int real_check(const char *s) {
    if (strlen(s) != BLOB_LEN) return 0;

    // Decrypt: XOR each byte with 0x5A
    unsigned char decrypted[BLOB_LEN];
    for (int i = 0; i < BLOB_LEN; i++)
        decrypted[i] = ENCRYPTED_BLOB[i] ^ 0x5A;

    return memcmp(s, decrypted, BLOB_LEN) == 0;
}

// ── Function pointer patched by constructor ─────────────────────────────
static int (*dispatch)(const char *) = fake_check1;

__attribute__((constructor))
static void setup(void) {
    // Runs BEFORE main — patches dispatch to real_check
    // Static analysis shows fake_check1; dynamic analysis reveals this
    dispatch = real_check;
}

// ── main ────────────────────────────────────────────────────────────────
int main(void) {
    char buf[64] = {0};

    // Attempt 1 — fake
    printf("Attempt 1: ");
    scanf("%32s", buf);
    if (fake_check1(buf)) { printf("Eclipse{%s}\n", buf); return 0; }
    puts("Wrong.");

    // Attempt 2 — fake
    printf("Attempt 2: ");
    scanf("%32s", buf);
    if (fake_check2(buf)) { printf("Eclipse{%s}\n", buf); return 0; }
    puts("Wrong.");

    // Attempt 3 — real (via patched dispatch)
    printf("Attempt 3: ");
    scanf("%32s", buf);
    if (dispatch(buf)) { printf("Eclipse{%s}\n", buf); return 0; }
    puts("Wrong.");

    return 0;
}