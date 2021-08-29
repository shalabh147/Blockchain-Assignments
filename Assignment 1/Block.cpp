#include "classes.h"

int Block::total_blocks_created; 

Block::Block()
{
    
}
Block::Block(Transaction * t) // constructor for genesis block creation

{
    block_id = total_blocks_created;
    total_blocks_created++;
    id_block_mapping[block_id] = this;
    set<Transaction *> s;
    s.insert(t);
    transactions = s;
    previous_id = -1;
}



Block::Block(set<Transaction *> s, int prev_id)
{
    block_id = total_blocks_created;
    total_blocks_created++;
    id_block_mapping[block_id] = this;
    previous_id = prev_id;
    transactions = s;
}
