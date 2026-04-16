    #ifndef COMMON_CRYPTO_KECCAK_H
    #define COMMON_CRYPTO_KECCAK_H

    #include <array>
    #include <cstddef>
    #include <vector>
    #include <cstdint>
    #include <iostream>


    namespace common {
        namespace crypto {

            typedef uint64_t State[5][5];

            inline uint64_t ROTL64(uint64_t x, int n) {
                return (x << n) | (x >> (64 - n));
            }



            inline void theta(State& A) {
                uint64_t C[5];

                for (int x = 0; x < 5;++x){
                    C[x] = A[x][0] ^ A[x][1] ^ A[x][2] ^ A[x][3] ^ A[x][4];
                }

                for (int x = 0; x < 5; ++x) {
                    uint64_t resultRiaks = C[(x + 4) % 5] ^ ROTL64(C[(x + 1) % 5], 1);
                    for (int y = 0; y < 5; ++y) {
                        A[x][y] ^= resultRiaks;
                    }
                }
            }

            inline void rho(State& A){
                static const  uint64_t rhoOffsets[5][5] = {
                    {0, 1, 62, 28, 27},
                    {36, 44, 6, 55, 20},
                    {3, 10, 43, 25 , 39},
                    {41, 45, 15, 21, 8},
                    {18, 2, 61, 56, 14}
                };

                for (int x = 0; x < 5; ++x) {
                    for (int y = 0; y < 5; ++y) {
                        A[x][y] = ROTL64(A[x][y], rhoOffsets[x][y]);
                    }
                }
            }

            inline void phi(State& A){
                uint64_t temp[5][5];

                for (int x = 0; x < 5; ++x) {
                    for (int y = 0; y < 5; ++y) {
                        temp[y][(2 * x + 3 * y) % 5] = A[x][y];
                    }
                }
                for (int x = 0; x < 5;++x) {
                    for (int y = 0; y < 5; ++y) {
                        A[x][y] = temp[x][y];
                    }
                }


            }

            inline void  chi(State& A){
                uint64_t temp[5][5];
                for (int x = 0; x < 5;++x) {
                    for (int y = 0; y < 5; ++y) {
                        temp[x][y] = A[x][y];
                    }
                }

                for (int x = 0; x < 5;++x) {
                    for (int y = 0; y < 5; ++y) {
                        A[x][y] = temp[x][y] ^ ((~temp[(x+1) % 5][y]) & temp[(x+2) % 5][y]);
                    }
                }
            }

            inline void iota(State& A, int round) {
                static const uint64_t roundConstants[24] = {
                    0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
                    0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
                    0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
                    0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
                    0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
                    0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
                    0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
                    0x8000000000008080, 0x0000000080000001, 0x8000000080008008
                };

                A[0][0] ^= roundConstants[round];
            }

            inline void keccak_f1600(State& data) {
                for (int round = 0; round < 24; ++round) {
                    theta(data);
                    rho(data);
                    phi(data);
                    chi(data);
                    iota(data, round);
                }
            }

            inline void absorb(State& data, const std::vector<uint8_t>& massage){
                // variabel dengan isi setiap 8 bit dan berubah ubah sebagai penampung
                std::vector<uint8_t> padded_massage = massage;
                // menambahkan pembuka yakni 0x01 padding setelah massage tersebut
                padded_massage.push_back(0x01);
                // lakkukan padding sampai size % 136 = 0
                while (padded_massage.size() % 136 != 135) {
                    // tambahkan 0x00 sampai size ini 135
                    padded_massage.push_back(0x00);
                }
                // tutup padding dengan 0x80 sebagai penutup
                padded_massage.push_back(0x80);

                // perulangan untuk memasukkan data 136 bytes kedalam setiap sel di array yang hanya bisa muat 64 bit
                for (size_t offset = 0; offset < padded_massage.size();  offset += 136) {
                    // perulangan untuk memasukkan 136 bytes kedalam array tapi hanya memasukkan dengan xor di 17 selnya saja
                    for (size_t i = 0; i < 17; ++i ) {
                        // penampung data yang di ambil di setiap  8 bytenya
                        uint64_t lane_value = 0;

                        // memakstikan semua kolom di masukkan datanya tapi hanya sampai pada bagian kolom 1 dan baris3 / (1,3)
                        size_t x = i % 5;
                        size_t y = i / 5;

                        // perulangan untuk menumpuk 8 bit dikalikan 8 kali
                        for (size_t j = 0; j < 8; ++j) {
                            // menumpuk dengan memakai xor yakni memaksa indeks sekarang ini untuk dibaca sebagai 64 bit dan di geser kekiri sejauh 8 bit
                            // jadi  8 bit pertama -> 8 bit kedua ... 8 bit kedelapan
                            lane_value ^= (static_cast<uint64_t>(padded_massage[offset + (i * 8) + j]) << (j*8));
                        }

                        // tumpuk semua datanya kedalam kolom dengan fungsi xor
                        data[x][y] ^= lane_value;
                    }

                    keccak_f1600(data);
                }

            }

            inline std::vector<uint8_t> squeeze(State& data){

                std::vector<uint8_t> hash_result;

                for (int i = 0; i < 4; ++i) {
                    uint64_t lane = data[i][0];

                    for (int j = 0; j < 8; ++j) {
                        hash_result.push_back(static_cast<uint8_t>((lane >> (j*8)) & 0xFF));
                    }
                }

                return hash_result;
            }

            inline std::vector<uint8_t> keccak256(const std::vector<uint8_t>& input){
                State state = {0};

                absorb(state, input);

                return squeeze(state);
            }

            inline std::vector<uint8_t> keccak256(const std::string &input) {
                std::vector<uint8_t> data(input.begin(), input.end());
                return keccak256(data);
            }

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
            * Khusus untuk std::vector karena ia adalah Turing Complete).
            * Overloading
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
