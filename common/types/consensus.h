#ifndef CONSENSUS_H
#define CONSENSUS_H

#include "execution.h"
#include "primitives.h"

namespace common {
    namespace consensus {
        struct Validator {
            types::PublicKey pubkey;
            types::Gwei effective_balance;
            bool slashed;
        };

        struct BeaconBlock {
            types::Slot slot;
            types::ValidatorIndex proposer_index;
            types::Root parent_root;
            types::Hash execution_payload_hash;
        };
    }
}
#endif
