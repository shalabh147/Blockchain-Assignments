#include "classes.h"

Node::Node()
{
    node_id = num_nodes;
    num_nodes++;
    id_node_mapping[node_id] = this;
    
    parent[0] = -1; // genesis block has no parent
    heights[0] =1;
    max_height = 1;
    speed = fast;
    
}

void Node::set_speed_to_slow(){
    this->speed = slow;  
}


// vrinda does not like this :( hmm
bool Node::findBalances(int block_id)         //traverse the tree up from block_id to find balances and validate transactions
{
    bitcoin_map.clear();
    
    
    stack<int> longest_chain;

    while(parent[block_id] != -1) {
        longest_chain.push(block_id);
    }


    while(!longest_chain.empty()) {
        Block * b = id_block_mapping[longest_chain.top()];
        longest_chain.pop();
        set<Transaction> txns = b->transactions;
        
        for(auto txn: txns) {
            int idx = txn.idx;
            int idy = txn.idy;
            int c = txn.c;

            if(bitcoin_map.find(idx) == bitcoin_map.end()){
                bitcoin_map[idx] = -c;
            }
            else{
                bitcoin_map[idx] = bitcoin_map.find(idx)->second - c;
            }

            if(idy != -1){
                if(bitcoin_map.find(idy) == bitcoin_map.end()){
                    bitcoin_map[idy] = c;
                }
                else{
                    bitcoin_map[idy] = bitcoin_map.find(idy)->second + c;
                }
            }

        // if balance becomes negtive at any point not valid chain.

        if(bitcoin_map.find(idx)->second < 0){
            return false;
        }
        if(idy!=-1 && bitcoin_map.find(idy) -> second < 0){
            return false;
        }
        }
    }

    return true;
}
    



void Node::generateTransaction(int T)
{
    int receiving_id = rand()% num_nodes ;
    
    // ??????
    int coins = rand() % 100;
    Transaction * Txn = new Transaction(node_id, receiving_id, coins);
    broadcastTransaction(Txn,T);


    int sampled_next_interarrival_time = transc_exp_distr(gen); // fill this
    int next_txn_time = T + sampled_next_interarrival_time;
    Event * e = new Event(ID_FOR_GEN_TRANS, node_id);  
    
    Simulate::AddEvent(e,next_txn_time);
}

void Node::receiveTransaction(Transaction *Txn, int T, int sender_node_id)
{
    all_transactions.insert(Txn->transac_id);

                                // sender_node_id
    broadcastTransaction(Txn,T, node_id);
}

void Node::broadcastTransaction(Transaction *txn, int T, int sender_node_id)
{
    // node already has received this txn, and has already broadcasted.
    if(all_transactions.find(txn->transac_id) != all_transactions.end()){ 
        return;
    }
    vector<int> neighbours = adj[node_id];

    for(int neighbour: neighbours)
    {
        if(neighbour == sender_node_id){ // dont broadcast to sender again. It has the transaction.
            continue;
        }

        int l = latency[node_id][neighbour];
        //some calculation
                                                    // sender  // receiver
        Event * f = new Event(ID_FOR_RECEIVE_TRANS, node_id, neighbour);
        f->addTransactionInfo(txn);
        Simulate::AddEvent(f,T+l);
    }
}



void Node::generateBlock(set<int> transac_pool,int T)
{
    //choose some subset of transactions from transac_pool, say sub
    Transaction coinbase_tr = Transaction(node_id);       //coinbase transaction
    Block* b =new Block(coinbase_tr); // give transactions
    last_block_created_time = T;

    int t_k = rand(something);
    last_wait_interval = t_k;

    int new_time = T + t_k;
    Event * e = new Event(ID_FOR_BROADCASTING_BLOCK,b->block_id);
    e->addBlockInfo(b);
    Simulate::AddEvent(e,new_time);

}

void Node::receiveBlock(Block *b, int T)
{   
    broadcastBlock(b,T);
    int parent_id = b->previous_id; 

    

    
    //if parent not yet present with node, then wait till it comes



    //if parent present
    

    bool invalid = false;
    invalid = !(findBalances(b->block_id));        //validate transactions of received block using this chain of parents.

    if(invalid == true)
    {
        return;
    }
        
    if(heights[parent_id] == max_height)
    {
        if(last_block_created_time + last_wait_interval > T)
        {
            //cancel event scheduled
            int time_to_cancel = last_block_created_time + last_wait_interval;
            Simulator.removeEvent(ID_FOR_BROADCASTING_BLOCK,node_id,time_to_cancel);
        }


        tree_blocks[parent_id].push_back(b->id);
        
        parent[b->block_id] = b->previous_id;
        heights[b->block_id] = heights[parent_id] + 1;
        max_height = heights[b->block_id];
    

        //calculate transaction pool by removing all transactions corresponding to this longest chain
        generateBlock(transac_pool,T);
    
    }


}

void Node::broadcastBlock(Block *b, int T)
{
    if(max_height == heights[b->id])         //if still the largest chain, only then broadcast last formed block
    {
        vector<int> neighbours = adj[node_id];

        for(int v: neighbours)
        {
            int l = latency[node_id][v];
            //some calculation

            Event* f = new Event(ID_FOR_RECEIVE_BLOCK, v);
            f->addBlockInfo(b);
            Simulate::AddEvent(f,T+l);
        }

    }
}