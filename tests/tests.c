/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <polyseed.h>

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef bool test_func(void);
typedef void multitest_func(void);

static int g_test_no = 0;

#define SEED_TIME1 ((time_t)1638446400) /* Dec 2021 */
#define SEED_TIME2 ((time_t)3118651200) /* Oct 2068 */
#define SEED_TIME3 ((time_t)4305268800) /* Jun 2106 */

static int g_num_langs;

static polyseed_data g_seed1;
static polyseed_data g_seed2;
static polyseed_data g_seed3;

static const char* g_phrase_en1 =
    "episode able assist rocket orphan rebuild assume have "
    "viable cable august tomato pledge remind accident dinosaur";
static const char* g_phrase_en2 =
    "episod able assi rock orph rebui assu have "
    "viab cabl augu toma pled remi accide dino";
static const char* g_phrase_en3 =
    "epis ablexxx assi rock orph rebu assu have "
    "viab cabl augu toma pled remi acci dino";
static const char* g_phrase_en4 =
    "episode able assist rocket orphan rebuild assume have "
    "viable cable august tomato pledge remind accident dinosaurus";
static const char* g_phrase_en5 =
    "episode able assist rocket orphan rebuild assume have "
    "viable cable august tomato pledge remind accident dinosaur ";

static const char* g_phrase_es1 =
    u8"puesto mente aliado olivo ábaco anular fondo labio "
    u8"empresa actuar feo truco acoso órgano cañón reír";
static const char* g_phrase_es2 =
    "puesto mente aliado olivo abaco anular fondo labio "
    "empresa actuar feo truco acoso organo canon reir";
static const char* g_phrase_es3 =
    "puest ment alia oliv abac anul fond labi "
    "empr actu feo truc acos orga cano reir";
static const char* g_phrase_es4 =
    "puesxxx ment alia oliv abac anul fond labi "
    "empr actu feo truc acos orga cano reir";
static const char* g_phrase_es5 =
    "pues ment ali oliv abac anul fond labi "
    "empr actu feo truc acos orga cano reir";

static const char* g_phrase_garbage1 = "xxx xxx";

static const char* g_phrase_garbage2 =
"xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx "
"xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx "
"xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx "
"xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx "
"xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx "
"xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx "
"xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx "
"xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx ";

static polyseed_str g_phrase_out;

static const polyseed_lang* g_lang_en;
static const polyseed_lang* g_lang_es;

#define RUN_TEST(x) run_test(#x, &x)
#define RUN_MULT(x) run_multitest(#x, &x)

static void run_test(const char* name, test_func* func) {
	printf("[%2i] %-40s ... ", ++g_test_no, name);
	printf(func() ? "PASSED\n" : "SKIPPED\n");
}

static void run_multitest(const char* name, multitest_func* func) {
    printf("[%2i] %s\n", ++g_test_no, name);
    func();
}

static void subtest_begin(const char* name) {
    printf("     %-40s ... ", name);
}

static void subtest_end(bool passed) {
    printf(passed ? "PASSED\n" : "SKIPPED\n");
}

static void output_hex(const char* data, size_t length) {
	for (unsigned i = 0; i < length; ++i)
		printf("%02x", data[i] & 0xff);
}

static inline char parse_nibble(char hex) {
	hex &= ~0x20;
	return (hex & 0x40) ? hex - ('A' - 10) : hex & 0xf;
}

static inline bool equals_hex(const void* bin, const char* hex) {
	size_t hexlen = strlen(hex);
	assert((hexlen % 2) == 0);
	const char* pbin = bin;
	for (int i = 0; i < hexlen; i += 2) {
		char nibble1 = parse_nibble(*hex++);
		char nibble2 = parse_nibble(*hex++);
		uint8_t val = (uint8_t)*pbin++;
		uint8_t ref = (uint8_t)nibble1 << 4 | nibble2;
		if (val != ref) {
			return false;
		}
	}
	return true;
}

static const polyseed_lang* get_lang(const char* name) {
	int count = polyseed_get_num_langs();
	for (int i = 0; i < count; ++i) {
		const polyseed_lang* lang = polyseed_get_lang(i);
		if (0 == strcmp(polyseed_get_lang_name_en(lang), name)) {
			return lang;
		}
	}
	return NULL;
}

#define TEST_KEYLEN 32

static const char rand_bytes1[] = {
	0xdd, 0x76, 0xe7, 0x35, 0x9a, 0x0d, 0xed, 0x37,
	0xcd, 0x0f, 0xf0, 0xf3, 0xc8, 0x29, 0xa5, 0xae,
	0x01, 0x67, 0xf3, 0xdb, 0x0f, 0x6d, 0x1b, 0x8f,
	0xad, 0x7d, 0x52, 0x81, 0x1c, 0x14, 0x00, 0xb8,
};

static void gen_rand_bytes1(void* result, size_t n) {
	assert(n == sizeof(rand_bytes1));
	memcpy(result, rand_bytes1, sizeof(rand_bytes1));
}

static const char rand_bytes2[] = {
    0x95, 0x45, 0x00, 0x00, 0x73, 0x5c, 0x6f, 0x21,
    0x30, 0x01, 0xb5, 0x71, 0xe0, 0x00, 0xbd, 0x2b,
    0x29, 0xd8, 0xad, 0x5d, 0xe2, 0x58, 0xa0, 0x69,
    0x64, 0x24, 0x4f, 0x0e, 0x99, 0x5d, 0x33, 0x2e,
};

static void gen_rand_bytes2(void* result, size_t n) {
	assert(n == sizeof(rand_bytes2));
	memcpy(result, rand_bytes2, sizeof(rand_bytes2));
}

static const char rand_bytes3[] = {
	0x67, 0xb9, 0x36, 0xdf, 0xa4, 0xda, 0x6a, 0xe8,
	0xd3, 0xb3, 0xcd, 0xb3, 0xb9, 0x37, 0xf4, 0x02,
	0x7b, 0x0e, 0x3b, 0x15, 0xd3, 0x9b, 0xea, 0xef,
	0x25, 0x3e, 0x21, 0x15, 0xae, 0xd0, 0x45, 0xdc,
};

static void gen_rand_bytes3(void* result, size_t n) {
	assert(n == sizeof(rand_bytes3));
	memcpy(result, rand_bytes3, sizeof(rand_bytes3));
}

static void pbkdf2_dummy1(const uint8_t* pw, size_t pwlen,
	const uint8_t* salt, size_t saltlen, uint64_t iterations,
	uint8_t* key, size_t keylen) {
	assert(equals_hex(pw,
		"dd76e7359a0ded37cd0ff0f3c829a5ae01673300000000000000000000000000"));
	assert(equals_hex(salt,
		"504f4c5953454544207631000000000001000000000000000000000000000000"));
}

static void pbkdf2_dummy2(const uint8_t* pw, size_t pwlen,
    const uint8_t* salt, size_t saltlen, uint64_t iterations,
    uint8_t* key, size_t keylen) {
    assert(equals_hex(pw,
        "95450000735c6f213001b571e000bd2b29d82d00000000000000000000000000"));
    assert(equals_hex(salt,
        "504f4c5953454544207631000000000033020000000000000000000000000000"));
}

static void pbkdf2_dummy3(const uint8_t* pw, size_t pwlen,
    const uint8_t* salt, size_t saltlen, uint64_t iterations,
    uint8_t* key, size_t keylen) {
    assert(equals_hex(pw,
        "67b936dfa4da6ae8d3b3cdb3b937f4027b0e3b00000000000000000000000000"));
    assert(equals_hex(salt,
        "504f4c59534545442076310001000000f7030000000000000000000000000000"));
}

static void u8_nfc_donothing(const char* str, polyseed_str norm) {
	/* do not compose */
    strncpy(norm, str, POLYSEED_STR_SIZE);
}

static void u8_nfkd_spaces(const char* str, polyseed_str norm) {
	/* normalize only ideographic spaces to allow Japanese phrases to roundtrip */
	int i = 0;
	for (; i < POLYSEED_STR_SIZE && *str != '\0'; ++i) {
		if (str[0] == '\xe3' && str[1] == '\x80' && str[2] == '\x80') {
			norm[i] = ' ';
            str += 3;
		}
		else {
			norm[i] = *str;
            ++str;
		}
	}
	if (i < POLYSEED_STR_SIZE) {
		norm[i] = '\0';
	}
}

static time_t time1(time_t* t) {
	return SEED_TIME1;
}

static time_t time2(time_t* t) {
	return SEED_TIME2;
}

static time_t time3(time_t* t) {
	return SEED_TIME3;
}

static void check_key(polyseed_data* data, polyseed_coin coin) {
	char key[TEST_KEYLEN];
	polyseed_keygen(data, coin, TEST_KEYLEN, key);
}

static bool test_inject1(void) {
	const polyseed_dependency deps = {
		.rand = &gen_rand_bytes1,
		.pbkdf2_sha256 = &pbkdf2_dummy1,
		.u8_nfc = &u8_nfc_donothing,
		.u8_nfkd = &u8_nfkd_spaces,
		.time = &time1,
	};
	polyseed_inject(&deps);
	return true;
}

static bool test_num_langs(void) {
    g_num_langs = polyseed_get_num_langs();
	assert(g_num_langs > 0);
	return true;
}

static bool test_get_lang(void) {
	for (int i = 0; i < g_num_langs; ++i) {
		const polyseed_lang* lang = polyseed_get_lang(i);
		assert(lang != NULL);
	}
	return true;
}

static void test_get_lang_name(void) {
	for (int i = 0; i < g_num_langs; ++i) {
		const polyseed_lang* lang = polyseed_get_lang(i);
        subtest_begin(polyseed_get_lang_name_en(lang));
		assert(polyseed_get_lang_name(lang) != NULL);
		assert(polyseed_get_lang_name_en(lang) != NULL);
        subtest_end(true);
	}
}

static bool test_create1(void) {
	polyseed_create(&g_seed1);
	return true;
}

static bool test_birthday1(void) {
	time_t birthday = polyseed_get_birthday(&g_seed1);
	assert(birthday <= SEED_TIME1);
	return true;
}

static bool test_keygen1(void) {
	char key[TEST_KEYLEN];
	polyseed_keygen(&g_seed1, POLYSEED_MONERO, TEST_KEYLEN, key);
	return true;
}

static bool test_encode_en(void) {
	g_lang_en = get_lang("English");
	if (g_lang_en == NULL) {
		return false;
	}
	polyseed_encode(&g_seed1, g_lang_en, POLYSEED_MONERO, g_phrase_out);
	assert(0 == strcmp(g_phrase_out, g_phrase_en1));
	return true;
}

static bool test_decode_en(void) {
	if (g_lang_en == NULL) {
		return false;
	}
	const polyseed_lang* lang;
	polyseed_data seed;
	polyseed_status res = polyseed_decode(g_phrase_en1, POLYSEED_MONERO, &lang, &seed);
	assert(res == POLYSEED_OK);
    assert(lang == g_lang_en);
	check_key(&seed, POLYSEED_MONERO);
    return true;
}

static bool test_decode_en_prefix(void) {
    if (g_lang_en == NULL) {
        return false;
    }
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_en2, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_OK);
    assert(lang == g_lang_en);
    check_key(&seed, POLYSEED_MONERO);
    return true;
}

static bool test_decode_en_suffix1(void) {
    if (g_lang_en == NULL) {
        return false;
    }
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_en3, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_ERR_LANG);
    return true;
}

static bool test_decode_en_suffix2(void) {
    if (g_lang_en == NULL) {
        return false;
    }
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_en4, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_ERR_LANG);
    return true;
}

static bool test_decode_en_space(void) {
    if (g_lang_en == NULL) {
        return false;
    }
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_en5, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_OK);
    assert(lang == g_lang_en);
    check_key(&seed, POLYSEED_MONERO);
    return true;
}

static bool test_decode_en_coin(void) {
    if (g_lang_en == NULL) {
        return false;
    }
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_en1, POLYSEED_AEON, &lang, &seed);
    assert(res == POLYSEED_ERR_CHECKSUM);
    return true;
}

static void test_roundtrip1(void) {
    for (int i = 0; i < g_num_langs; ++i) {
        const polyseed_lang* lang = polyseed_get_lang(i);
        subtest_begin(polyseed_get_lang_name_en(lang));
        const polyseed_lang* lang_out;
        polyseed_str phrase;
        polyseed_data seed;
        polyseed_encode(&g_seed1, lang, POLYSEED_MONERO, phrase);
        polyseed_status res = polyseed_decode(phrase, POLYSEED_MONERO, &lang_out, &seed);
        assert(res == POLYSEED_OK);
        assert(lang == lang_out);
        check_key(&seed, POLYSEED_MONERO);
        subtest_end(true);
    }
}

static bool test_inject2(void) {
    const polyseed_dependency deps = {
        .rand = &gen_rand_bytes2,
        .pbkdf2_sha256 = &pbkdf2_dummy2,
        .u8_nfc = &u8_nfc_donothing,
        .u8_nfkd = &u8_nfkd_spaces,
        .time = &time2,
    };
    polyseed_inject(&deps);
    return true;
}

static bool test_create2(void) {
    polyseed_create(&g_seed2);
    return true;
}

static bool test_birthday2(void) {
    time_t birthday = polyseed_get_birthday(&g_seed2);
    assert(birthday <= SEED_TIME2);
    return true;
}

static bool test_keygen2(void) {
    char key[TEST_KEYLEN];
    polyseed_keygen(&g_seed2, POLYSEED_MONERO, TEST_KEYLEN, key);
    return true;
}

static bool test_encode_es(void) {
    g_lang_es = get_lang("Spanish");
    if (g_lang_es == NULL) {
        return false;
    }
    polyseed_encode(&g_seed2, g_lang_es, POLYSEED_MONERO, g_phrase_out);
    assert(0 == strcmp(g_phrase_out, g_phrase_es1));
    return true;
}

static bool test_decode_es(void) {
    if (g_lang_es == NULL) {
        return false;
    }
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_out, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_OK);
    assert(lang == g_lang_es);
    check_key(&seed, POLYSEED_MONERO);
    return true;
}

static bool test_decode_es_noaccent(void) {
    if (g_lang_es == NULL) {
        return false;
    }
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_es2, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_OK);
    assert(lang == g_lang_es);
    check_key(&seed, POLYSEED_MONERO);
    return true;
}

static bool test_decode_es_prefix1(void) {
    if (g_lang_es == NULL) {
        return false;
    }
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_es3, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_OK);
    assert(lang == g_lang_es);
    check_key(&seed, POLYSEED_MONERO);
    return true;
}

static bool test_decode_es_suffix(void) {
    if (g_lang_es == NULL) {
        return false;
    }
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_es4, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_ERR_LANG);
    return true;
}

static bool test_decode_es_prefix2(void) {
    if (g_lang_es == NULL) {
        return false;
    }
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_es5, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_ERR_LANG);
    return true;
}

static bool test_inject3(void) {
    const polyseed_dependency dep = {
        .rand = &gen_rand_bytes3,
        .pbkdf2_sha256 = &pbkdf2_dummy3,
        .u8_nfc = &u8_nfc_donothing,
        .u8_nfkd = &u8_nfkd_spaces,
        .time = &time3,
    };
    polyseed_inject(&dep);
    return true;
}

static bool test_create3(void) {
    polyseed_create(&g_seed3);
    return true;
}

static bool test_birthday3(void) {
    time_t birthday = polyseed_get_birthday(&g_seed3);
    assert(birthday <= SEED_TIME3);
    return true;
}

static bool test_keygen3(void) {
    char key[TEST_KEYLEN];
    polyseed_keygen(&g_seed3, POLYSEED_AEON, TEST_KEYLEN, key);
    return true;
}

static void test_roundtrip3(void) {
    for (int i = 0; i < g_num_langs; ++i) {
        const polyseed_lang* lang = polyseed_get_lang(i);
        subtest_begin(polyseed_get_lang_name_en(lang));
        const polyseed_lang* lang_out;
        polyseed_str phrase;
        polyseed_data seed;
        polyseed_encode(&g_seed3, lang, POLYSEED_AEON, phrase);
        polyseed_status res = polyseed_decode(phrase, POLYSEED_AEON, &lang_out, &seed);
        assert(res == POLYSEED_OK);
        assert(lang == lang_out);
        check_key(&seed, POLYSEED_AEON);
        subtest_end(true);
    }
}

static bool test_decode_garbage1(void) {
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_garbage1, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_ERR_NUM_WORDS);
    return true;
}

static bool test_decode_garbage2(void) {
    const polyseed_lang* lang;
    polyseed_data seed;
    polyseed_status res = polyseed_decode(g_phrase_garbage2, POLYSEED_MONERO, &lang, &seed);
    assert(res == POLYSEED_ERR_NUM_WORDS);
    return true;
}

int main() {
	RUN_TEST(test_inject1);
	RUN_TEST(test_num_langs);
	RUN_TEST(test_get_lang);
	RUN_MULT(test_get_lang_name);
	RUN_TEST(test_create1);
	RUN_TEST(test_birthday1);
	RUN_TEST(test_keygen1);
	RUN_TEST(test_encode_en);
	RUN_TEST(test_decode_en);
    RUN_TEST(test_decode_en_prefix);
    RUN_TEST(test_decode_en_suffix1);
    RUN_TEST(test_decode_en_suffix2);
    RUN_TEST(test_decode_en_space);
    RUN_TEST(test_decode_en_coin);
    RUN_MULT(test_roundtrip1);
    RUN_TEST(test_inject2);
    RUN_TEST(test_create2);
    RUN_TEST(test_birthday2);
    RUN_TEST(test_keygen2);
    RUN_TEST(test_encode_es);
    RUN_TEST(test_decode_es);
    RUN_TEST(test_decode_es_noaccent);
    RUN_TEST(test_decode_es_prefix1);
    RUN_TEST(test_decode_es_suffix);
    RUN_TEST(test_decode_es_prefix2);
    RUN_TEST(test_inject3);
    RUN_TEST(test_create3);
    RUN_TEST(test_birthday3);
    RUN_TEST(test_keygen3);
    RUN_MULT(test_roundtrip3);
    RUN_TEST(test_decode_garbage1);
    RUN_TEST(test_decode_garbage2);

	printf("\nAll tests were successful\n");
	return 0;
}
