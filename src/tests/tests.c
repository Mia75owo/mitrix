#include "tests.h"
#include "memory/memory.h"
#include "util/mem.h"
#include "util/types.h"
#include "util/debug.h"

// clang-format off
void debug_tests() {
    // ================================================================
    // ASSERT
    assert(1 == 1);
    assert_msg(1 == 1,                      "TEST: assert_msg");

    // ================================================================
    // MEMCMP
    static u8 buf1[10] = {0,1,2,3,4,5,6,7,8,9};
    static u8 buf2[10] = {0,1,2,3,4,5,6,7,8,9};
    assert_msg(memcmp(buf1, buf2, 10) == 1, "TEST: memcmp true");
    buf1[9]--;
    assert_msg(memcmp(buf1, buf2, 10) == 0, "TEST: memcmp false 1");
    buf1[9]++;
    buf2[0]++;
    assert_msg(memcmp(buf1, buf2, 10) == 0, "TEST: memcmp false 2");

    // ================================================================
    // STRLEN
    assert_msg(strlen("abc") == 3,      "TEST: strlen 3");
    assert_msg(strlen("") == 0,         "TEST: strlen 0");

    // ================================================================
    // CTOI
    assert_msg(ctoi('0') == 0,          "TEST: ctoi 0");
    assert_msg(ctoi('9') == 9,          "TEST: ctoi 9");
    assert_msg(ctoi('a') == 0xa,        "TEST: ctoi a");
    assert_msg(ctoi('f') == 0xf,        "TEST: ctoi f");
    assert_msg(ctoi('A') == 0xa,        "TEST: ctoi A");
    assert_msg(ctoi('F') == 0xf,        "TEST: ctoi F");
    assert_msg(ctoi('p') == 0,          "TEST: ctoi p");

    // ================================================================
    // ATOI
    assert_msg(atoi("123", 10) == 123,  "TEST: atoi b10");
    assert_msg(atoi("1fa", 16) == 0x1fa,"TEST: atoi b16");
    assert_msg(atoi("0", 10) == 0,      "TEST: atoi 0 b10");
    assert_msg(atoi("0", 16) == 0x0,    "TEST: atoi 0 b16");
    assert_msg(atoi("18446744073709551615", 10) == 18446744073709551615ull, "TEST: atoi MAX 10");
    assert_msg(atoi("ffffffffffffffff", 16) == 0xffffffffffffffffull,       "TEST: atoi MAX b16");


    // ================================================================
    // MEMSET

    static u8 buf4[8] = {9,9,9,9,9,9,9,9};
    static u8 buf3[8] = {0,0,0,0,0,0,0,0};

    memset(buf3, 9, 8);
    assert_msg(memcmp(buf3, buf4, 8) == true, "TEST: memset");

    // ================================================================
    // MEMCPY
    static u8 buf5[8] = {1,2,3,4,5,6,7,8};
    static u8 buf6[8] = {0,0,0,0,0,0,0,0};

    memcpy(buf5, buf6, 8);
    assert_msg(memcmp(buf5, buf6, 8) == true, "TEST: memcpy");

    // ================================================================
    // MEMREV

    static u8 buf7[8] = {8,7,6,5,4,3,2,1};
    static u8 buf8[8] = {1,2,3,4,5,6,7,8};

    memrev(buf8, 8);
    assert_msg(memcmp(buf7, buf8, 8) == true, "TEST: memrev");
}
// clang-format on
