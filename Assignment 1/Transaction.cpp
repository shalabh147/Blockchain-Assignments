#include "classes.h"

int Transaction::num_transactions;

Transaction::Transaction()
{
    transac_id = num_transactions;
    num_transactions++;
    id_txn_mapping[transac_id] = this;
}

Transaction::Transaction(int s_id, int r_id, int co)
{
    transac_id = num_transactions;
    num_transactions++;
    idx = s_id;
    idy = r_id;
    c = co;
    id_txn_mapping[transac_id] = this;
}

// this constructor is only used for creating coinbase transactions (r_id?)
Transaction::Transaction(int s_id){
    transac_id = num_transactions;
    num_transactions++;
    idx = -1;
    idy = s_id;
    c = COINBASE_REWARD;
    id_txn_mapping[transac_id] = this;

}