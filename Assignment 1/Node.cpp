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
            
            if(idx != -1){
                if(bitcoin_map.find(idx) == bitcoin_map.end()){
                    bitcoin_map[idx] = -c;
                }
                else{
                    bitcoin_map[idx] = bitcoin_map.find(idx)->second - c;
                }
            }

            
                if(bitcoin_map.find(idy) == bitcoin_map.end()){
                    bitcoin_map[idy] = c;
                }
                else{
                    bitcoin_map[idy] = bitcoin_map.find(idy)->second + c;
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
    
    // teke bitcoin balance from head of longest chain
  
    int coins = rand() % 100;
    Transaction * Txn = new Transaction(node_id, receiving_id, coins);
    broadcastTransaction(Txn,T);


    int sampled_next_interarrival_time = transac_exp_distr(gen); // fill this
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

bool Node::validateAndAddTreeNode( int arrival_time, int parent_id, int b_id){
    BlockTreeNode* parent_tree_node = id_blockTreeNode_mapping[parent_id];
    map<int,int> parent_btc_balances = parent_tree_node->bitcoin_balances;
    set<Transaction*> current_txns = id_block_mapping[b_id]->transactions;
    map<int,int> new_btc_balances;
    for(auto txn : current_txns){
         int idx = txn->idx;
            int idy = txn->idy;
            int c = txn->c;
            
            if(idx != -1){
                if(parent_btc_balances.find(idx) == parent_btc_balances.end()){
                    new_btc_balances[idx] = -c;
                }
                else{
                    new_btc_balances[idx] = parent_btc_balances[idx] - c;
                }
            }

            
                if(parent_btc_balances.find(idy) == parent_btc_balances.end()){
                    new_btc_balances[idy] = c;
                }
                else{
                    new_btc_balances[idy] = parent_btc_balances[idy] + c;
                }

            if(idx!=-1 && new_btc_balances[idx] < 0){
                return false;
            }

            if(new_btc_balances[idy] < 0){
                return false;
            }
            

        // if balance becomes negtive at any point not valid chain.

        }

        // block is valid. Add to blockTree.
    

    BlockTreeNode* tree_node = new BlockTreeNode();
    tree_node->arrival_time = arrival_time;
    tree_node->parent = parent_tree_node;
    tree_node->block_id = b_id;
    tree_node->level = parent_tree_node->level+1;
    tree_node->bitcoin_balances = new_btc_balances;
    id_blockTreeNode_mapping[b_id] = tree_node;

    if(block_chain_leaves.find(parent_id) != block_chain_leaves.end()){
        block_chain_leaves.erase(parent_id);
        block_chain_leaves.insert(b_id);
    }

   
    return true;


}

void Node::receiveBlock(Block *b, int T)
{   
    
    broadcastBlock(b,T);
    int parent_id = b->previous_id; 



    // received genesis block
    if(parent_id == -1){
        BlockTreeNode* genesis_tree_node = new BlockTreeNode();
        
    }

    //if parent not yet present with node, then wait till it comes
    if(!present[parent_id])
    {
        pending_blocks.insert(b->block_id);
        return;
    }
    
    

    //if parent present

    bool valid = validateAndAddTreeNode(T, parent_id, b->block_id);


    // new block is valid and added to tree.
    if(valid){ 

        present[b->block_id] = true;

        if(id_blockTreeNode_mapping[b->block_id]->level > longest_chain_head->level){
            // longest chain changed
            longest_chain_head = id_blockTreeNode_mapping[b->block_id];


            if(last_block_created_time + last_wait_interval > T)
            {
                //cancel event scheduled
                int time_to_cancel = last_block_created_time + last_wait_interval;
                Simulate::removeEvent(ID_FOR_BROADCASTING_BLOCK,node_id,time_to_cancel);
            }

            /////// create new txn set //////////////////////
            generateBlock(transac_pool,T);

        }


    }
    

    //check in pending list if any block is waiting for the received block
   set<int> to_remove; 
    for(int blck_id: pending_blocks)
    {
        if(id_block_mapping[blck_id]->previous_id == b->block_id)
        {   
            to_remove.insert(blck_id);
//////////////////////////////////////// change to account for arrival time /////////////////////////////
            Event *e = new Event(ID_FOR_RECEIVE_BLOCK, node_id);
            Simulate::AddEvent(e,T);
        }
    }

   for(int id : to_remove){
       pending_blocks.erase(id);
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