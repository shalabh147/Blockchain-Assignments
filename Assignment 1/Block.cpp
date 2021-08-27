#include "classes.h"

Block::Block()
{
    
}
Block::Block(Transaction t)
{
    block_id = total_blocks_created;
    total_blocks_created++;
    id_block_mapping[block_id] = this;
}



Block::Block(set<Transaction> s, int prev_id)
{
    block_id = total_blocks_created;
    total_blocks_created++;
    id_block_mapping[block_id] = this;
    previous_id = prev_id;
    transactions = s;
}
