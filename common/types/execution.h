#ifndef EXECUTION_H
#define EXECUTION_H

#include <vector>
#include <cstdint>
#include "primitives.h"
namespace common {
    namespace execution {
        struct Transaction {
            uint64_t nonce;
            common::types::Address to;
            common::types::uint256 value;
            uint64_t gas_limit;
            std::vector<uint8_t> data;
            common::types::Signature signature;
        };

        struct Header {
            common::types::Hash parent_hash;
            common::types::Root state_root;
            common::types::Address beneficiary;
            uint64_t number;
            uint64_t timestamp;
            common::types::uint256 base_fee_per_gas;
        };
}
}
#endif
