#include "classes.h"

Event::Event(int e_id, int s_id, int n_id)
{
    sender_id = s_id;
    event_id = e_id;
    node_id = n_id;
}

Event::Event(int e_id, int n_id){
        event_id = e_id;
        node_id = n_id;
}

void Event::addBlockInfo(Block * c)
{
    b = c;
}

void Event::addTransactionInfo(Transaction * t)
{
    txn = t;
}

void Event::gen_trans_event(int curr_time){
    id_node_mapping[node_id]->generateTransaction(curr_time);
}

void Event::receive_trans_event(int curr_time){
    id_node_mapping[node_id]->receiveTransaction(this->txn, curr_time, sender_id);
}

void Event::receive_block_event(int curr_time){
    id_node_mapping[node_id]->receiveBlock(this->b, curr_time);
}

void Event::broadcast_block_event(int curr_time){
    id_node_mapping[node_id]->broadcastBlock(this->b, curr_time);
}