#ifndef KECCAK_H
#define KECCAK_H

#include <cstddef>
#include <vector>
#include <cstdint>
#include <iostream>

namespace common {
    namespace keccak {

        /**
         * Digunakan untuk tipe data yang ukurannya sudah pasti di Stack (struct/array).
         * Kita cukup pakai &anydata karena isinya nempel langsung di situ.
         */
        template <typename datatype>
        void hash(const datatype& anydata) {

            // Ini teknik kacamata X-Ray: Memaksa CPU melihat data apapun sebagai
            // satuan terkecil (8-bit/1 byte) lewat pointer bytesPtr.
            // Karena ini statis, kita ambil alamat si objeknya langsung (&anydata).
            const uint8_t *bytesPtr = reinterpret_cast<const uint8_t*>(&anydata);

            // sizeof(anydata) di sini aman karena ukurannya sudah 'fixed' sejak lahir.
            std::size_t bytesSize = sizeof(anydata);

            std::cout << "Reading Stack Memory (Static): " << std::endl;
            for (size_t i = 0; i < bytesSize; ++i) {
                // (int) dipake supaya yang muncul angka mentah (0-255), bukan simbol ASCII aneh.
                std::cout << (int)bytesPtr[i] << " ";
            }
            std::cout << std::endl;
        }

        /**
         * Khusus untuk std::vector karena ia adalah Turing Complete (bisa melar).
         * Overloading ini supaya compiler gak bingung mana yang 'rumah' mana yang 'gudang'.
         */
        template <typename datatype>
        void hash(const std::vector<datatype>& anydata) {

            // JANGAN pakai &anydata! Itu cuma alamat si Komandan di Stack.
            // Kita pakai .data() untuk minta koordinat langsung ke "Barak" di Heap.
            // Ini return alamat dimana seluruh Chapter berada, bukan alamat si Komandan.
            const uint8_t* bytesPtr = reinterpret_cast<const uint8_t*>(anydata.data());

            // Matematika Actual: Jumlah item (size) dikali ukuran asli tiap prajurit (datatype).
            // Kalau lupa dikali sizeof(datatype), kita cuma baca 'permukaan' nya doang.
            size_t bytesSize = anydata.size() * sizeof(datatype);

            std::cout << "Reading Heap Memory (Dynamic Vector): " << std::endl;
            for (size_t i = 0; i < bytesSize; ++i) {
                std::cout << (int)bytesPtr[i] << " ";
            }
            std::cout << std::endl;
        }
    }
}

#endif
