#include "classes.h"

Transaction::Transaction()
{
    transac_id = num_transactions;
    num_transactions++;
}

Transaction::Transaction(int s_id, int r_id, int co)
{
    transac_id = num_transactions;
    num_transactions++;
    idx = s_id;
    idy = r_id;
    c = co;
}

// this constructor is only used for creating coinbase transactions (r_id?)
Transaction::Transaction(int s_id){
    transac_id = num_transactions;
    num_transactions++;
    idx = s_id;
    idy = -1;
    c = COINBASE_REWARD;

}