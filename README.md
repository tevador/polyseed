## Features

* each seed consists of 16 mnemonic words (36% shorter than the original 25-word seed)
* the seed includes wallet birthday to optimize restoring from the seed (only blocks after the wallet birthday have to be scanned for transactions)
* advanced checksum based on a Reed-Solomon linear code
* 5 bits reserved for future updates
* built-in way to make seeds incompatible between different coins, e.g. a seed for Aeon cannot be accidentally used to restore a Monero wallet

Supported languages:

1. English
2. Korean
3. French
4. Italian
5. Portuguese
6. Japanese
7. Spanish
8. Chinese (Simplified)
9. Chinese (Traditional)
10. Czech

For languages based on the latin alphabet, just the first 4 characters of each word need to be provided when restoring a seed. French and Spanish seeds can be input with or without accents. Wordlists are based on [BIP-39](https://github.com/bitcoin/bips/blob/master/bip-0039/bip-0039-wordlists.md).

## Build

```
git clone https://github.com/tevador/polyseed.git
cd polyseed
mkdir build
cd build
cmake ..
make
```

This will build a static library, a dynamic library and an executable with functional tests.

## API

The API is documented in the public header file [polyseed.h](include/polyseed.h).

## Dependency injection

Polyseed uses dependency injection. The following 5 functions must be provided by calling `polyseed_inject`:

| dependency | description | implemented in |
|------------|-------------|----------------|
| randbytes  | Function to generate cryptographically secure random bytes | [libsodium](https://github.com/jedisct1/libsodium), [OpenSSL](https://github.com/openssl/openssl) |
| pbkdf2_sha256 | Function to calculate PBKDF2 based on HMAC-SHA256 | [libsodium](https://github.com/jedisct1/libsodium), [OpenSSL](https://github.com/openssl/openssl) |
| memzero | Function to securely erase memory | [libsodium](https://github.com/jedisct1/libsodium), [OpenSSL](https://github.com/openssl/openssl) |
| u8_nfc | Function to convert a UTF8 string to the composed canonical form. | [Boost.Locale](https://www.boost.org/doc/libs/1_77_0/libs/locale/doc/html/), [utf8proc](https://github.com/JuliaStrings/utf8proc) |
| u8_nfkd | Function to convert a UTF8 string to the decomposed canonical form. | [Boost.Locale](https://www.boost.org/doc/libs/1_77_0/libs/locale/doc/html/), [utf8proc](https://github.com/JuliaStrings/utf8proc) |

These functions are implemented in widely used and tested libraries (see above for some examples) and it would be out of the scope of this library to implement them. It also reduces the security risks (polyseed doesn't contain any cryptographic code).

Additional 3 functions are optional dependencies. If they are not provided (the corresponding function pointer is `NULL`), polyseed will use the default implementation from the Standard C Library.

| dependency | description | libc function |
|------------|-------------|----------------|
| time  | Function to get the current time | `time_t time(time_t *arg);` |
| alloc | Function to allocate memory | `void* malloc(size_t size);` |
| free | Function to free memory | `void free(void* ptr)` |

These are mostly needed for testing purposes, but can be also used to provide a custom memory allocator.

## Security

Polyseed was designed for the 128-bit security level. This corresponds to the security of the ed25519 elliptic curve, which requires [about 2<sup>126</sup> operations](https://safecurves.cr.yp.to/rho.html) to break a key.

Internally, polyseed uses a 150-bit secret seed and a 10-bit counter that increases every month and serves as the wallet birthday indicator. Additionally, seeds for different currencies are domain-separated. This provides a comfortable security margin against [multi-target attacks](https://blog.cr.yp.to/20151120-batchattacks.html).

## License

The libray is released under the LGPLv3 license. No restrictions are placed on software that just links to the library.