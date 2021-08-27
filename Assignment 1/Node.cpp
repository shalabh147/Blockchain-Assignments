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


 



void Node::generateTransaction(int T)
{
    int receiving_id = rand()% num_nodes ;
    
    // teke bitcoin balance from head of longest chain
  
    
    if(T!=0){

        if(longest_chain_head->bitcoin_balances[node_id] != 0){
            int coins = rand() % longest_chain_head->bitcoin_balances[node_id];
            Transaction * Txn = new Transaction(node_id, receiving_id, coins);
            broadcastTransaction(Txn,T);
        }
    }

    int sampled_next_interarrival_time = (int) transac_exp_distr(gen); // fill this
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



void Node::generateBlock(set<int> transac_pool,int T, int parent_id)
{
    //choose some subset of transactions from transac_pool, say sub
    Transaction* coinbase_tr = new Transaction(node_id);       //coinbase transaction
    transac_pool.insert(coinbase_tr->transac_id);

    set<Transaction*> txns ;
    for(auto id : transac_pool){
        txns.insert(id_txn_mapping[id]);
    }
    Block* b =new Block(txns, parent_id ); // give transactions

    int t_k = (int) mining_exp_distr(gen);

    int new_time = T + t_k;


    Event * e = new Event(ID_FOR_CHECK_AND_BROADCAST_BLOCK,node_id);
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

void Node::checkAndBroadcastBlock(Block *b, int T){
    int parent_id = id_block_mapping[b->block_id]->previous_id;
    if(longest_chain_head == id_block_mapping[parent_id]){
        Event * e = new Event(ID_FOR_RECEIVE_BLOCK, node_id);
        e->addBlockInfo(b);
        Simulate::AddEvent(e, T);

    }
}
void Node::receiveBlock(Block *b, int T)
{   
    received[b->block_id] = true;
    broadcastBlock(b,T);
    int parent_id = b->previous_id; 



    // received genesis block
    if(parent_id == -1){
        BlockTreeNode* genesis_tree_node = new BlockTreeNode();
        genesis_tree_node->arrival_time = T;
        genesis_tree_node->block_id = b->block_id;
        genesis_tree_node->level = 1;
        set<Transaction*> txns = b->transactions;
        map<int,int> btc_balances;
        for(auto txn : txns){ // only coin base transaction present
            btc_balances[txn->idy] = txn->c;
            all_transactions.insert(txn->transac_id);
            longest_chain_txns.insert(txn->transac_id);
        }
        genesis_tree_node->bitcoin_balances = btc_balances;
        genesis_tree_node->parent = NULL;

        block_chain_leaves.insert(genesis_tree_node->block_id);
        longest_chain_head = genesis_tree_node;
        return;
        
    }

    //if parent not yet arrived at node, then wait till it comes
    if(!received[parent_id])
    {
        pending_blocks.insert(b->block_id);
        return;
    }
    
    ////////////////////// if parent was received but discarded //////////
    if(!present[parent_id]){

        /////// discuss this ////////////////////////////////////////////////
        // **** if parent was discared, this block also needs to be discarded. ***** //
        return;



    }

    //if parent present

    bool valid = validateAndAddTreeNode(T, parent_id, b->block_id);


    // new block is valid and added to tree.
    if(valid){ 

        present[b->block_id] = true;

        if(id_blockTreeNode_mapping[b->block_id]->level > longest_chain_head->level){ // block on which mining is done is changed


            if(longest_chain_head->block_id == parent_id){
                // can use longest_chain_txns
                set<Transaction*> txns = b->transactions;
                for(auto txn : txns){
                    longest_chain_txns.insert(txn->transac_id);
                }
            }

            else{
                // recompute longest_chain_txns
                set<int> new_txns;
                BlockTreeNode* n = id_blockTreeNode_mapping[b->block_id];
                while(n->parent){
                    Block* b = id_block_mapping[n->block_id];
                    set<Transaction*> txns = b->transactions;
                    for(auto txn : txns){
                        new_txns.insert(txn->transac_id);
                    }
                    n = n->parent;
                }
                longest_chain_txns = new_txns;
            }

            // longest chain changed
            longest_chain_head = id_blockTreeNode_mapping[b->block_id];



            /////// create new txn set //////////////////////
            set<int> utxos;
            set_difference(all_transactions.begin(), all_transactions.end(), longest_chain_txns.begin(), longest_chain_txns.end(), utxos.begin());
            set<int> chosen_txns;
            map<int, int> new_balances;
            map<int,int> parent_balances = longest_chain_head->bitcoin_balances;


/////////////////// can try other strategies for picking txns////////////////////////////////////// 
            for(auto txn_id : utxos){
                if(chosen_txns.size() == MAX_TXNS){
                    break;
                }
                Transaction* txn = id_txn_mapping[txn_id];
                int idx = txn->idx;
                int idy = txn->idy;
                int coins = txn->c;

                if(idx!= -1 && parent_balances.find(idx) == parent_balances.end()){
                    continue;
                }
                if(idx!= -1 && parent_balances[idx] < coins){
                    continue;
                }

                if(idx != -1){
                    new_balances[idx] = parent_balances[idx] - coins;
                }
                new_balances[idy] = parent_balances[idy] + coins;
                chosen_txns.insert(txn_id);
                
            }

            generateBlock(chosen_txns,T, longest_chain_head->block_id);

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


// ********************** discuss this *********************************** //
            if(valid || present[id_block_mapping[blck_id]->previous_id]){ // same bools??
            Event *e = new Event(ID_FOR_RECEIVE_BLOCK, node_id);
            e->addBlockInfo(id_block_mapping[blck_id]);
            Simulate::AddEvent(e,T);
            }
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