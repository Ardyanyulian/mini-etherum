/*
 * digunakan agar libary buatan manualku hanya di load oleh kompiler 1 kali
 */

#ifndef COMMON_TYPES_PRIMITIVES_H
#define COMMON_TYPES_PRIMITIVES_H

/*
 *  menggunakan libary yang sudah dibuat orang lain secara manual
 */

#include <array>
#include <cstdint>
#include <string>

/*
 * Sistem penamaan library yang nanti dipanggil gimana ??
 */

namespace common {

    namespace types {
        // variabel Address yang berbentuk struct, hanya bisa menampung 20 saja, dan maksimal 1 bytes
        struct Address {
            std::array<uint8_t, 20> bytes;
        };


        struct Hash {
            std::array<uint8_t, 32> bytes;
        };

        struct PublicKey {
            std::array<uint8_t, 48> bytes;
        };

        struct Signature {
            std::array<uint8_t , 96> bytes;
        };

        struct Root {
            std::array<uint8_t, 32> bytes;
        };

        // Sistem penamaan tipe data atau tipe data buatan untuk menjadi alias tipe data lain.
        using Slot = uint64_t;
        using Epoch = uint64_t;
        using ValidatorIndex = uint64_t;
        using Gwei = uint64_t;
        using uint256 = std::array<uint64_t, 4>;
    }

}

// penutup if
#endif
