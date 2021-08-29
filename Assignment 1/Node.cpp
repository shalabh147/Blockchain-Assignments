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



void Node::generateTransaction(double T)
{
    int receiving_id = rand()% num_nodes ;
    
  
  
    if(T!=0){ // if T==0, this is the trigger for generating transactions

        if(longest_chain_head->bitcoin_balances[node_id] != 0){
            int coins = rand() % longest_chain_head->bitcoin_balances[node_id];
            Transaction * Txn = new Transaction(node_id, receiving_id, coins);
            broadcastTransaction(Txn,T,node_id);
        }
    }
    std::exponential_distribution<double> transac_exp_distr(1/T_tx);
    double sampled_next_interarrival_time =  transac_exp_distr(gen);
    double next_txn_time = T + sampled_next_interarrival_time;
    Event * e = new Event(ID_FOR_GEN_TRANS, node_id);  
    
    AddEvent(e,next_txn_time);
}

void Node::receiveTransaction(Transaction *Txn, double T, int sender_node_id)
{
    all_transactions.insert(Txn->transac_id);

                                // sender_node_id
    broadcastTransaction(Txn,T, node_id);
}

void Node::broadcastTransaction(Transaction *txn, double T, int sender_node_id)
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

        double c = (this->speed == fast && id_node_mapping[neighbour]->speed == fast)? 1e8 : 5e6 ;

        std::exponential_distribution<double> exp_distr_for_d(c/96000);
        double d = exp_distr_for_d(gen);

        double latency = rho + 8000/c + d;
        //some calculation
                                                    // sender  // receiver
        Event * f = new Event(ID_FOR_RECEIVE_TRANS, node_id, neighbour);
        f->addTransactionInfo(txn);
        AddEvent(f,T+latency);
    }
}



void Node::generateBlock(set<int> transac_pool, double T, int parent_id)
{
    //choose some subset of transactions from transac_pool, say sub
    Transaction* coinbase_tr = new Transaction(node_id);       //coinbase transaction
    transac_pool.insert(coinbase_tr->transac_id);

    set<Transaction*> txns ;
    for(auto id : transac_pool){
        txns.insert(id_txn_mapping[id]);
    }
    Block* b =new Block(txns, parent_id ); // give transactions
    
    std::exponential_distribution<double> mining_exp_distr (1/T_k);
    double t_k = mining_exp_distr(gen);

    double new_time = T + t_k;


    Event * e = new Event(ID_FOR_CHECK_AND_BROADCAST_BLOCK,node_id);
    e->addBlockInfo(b);
    AddEvent(e,new_time);

}



bool Node::validateAndAddTreeNode( double arrival_time, int parent_id, int b_id){
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

void Node::checkAndBroadcastBlock(Block *b, double T){
    int parent_id = id_block_mapping[b->block_id]->previous_id;
    if(longest_chain_head == id_block_mapping[parent_id]){          //should be comparing block ids?
        Event * e = new Event(ID_FOR_RECEIVE_BLOCK, node_id);
        e->addBlockInfo(b);
        AddEvent(e, T);

    }
}
void Node::receiveBlock(Block *b, double T)
{   
    // block has already been received, broadcasted, validate. 
    // Returning here, to unsure infinite loop in broadcasting.
    if(received[b->block_id]){
        return;
    }
    received[b->block_id] = true;
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
    // no need to broadcast genesis block, as it is received by all nodes at T=0
    broadcastBlock(b,T);
    



   

    //if parent not yet arrived at node, then wait till it comes
    if(!received[parent_id])
    {
        pending_blocks.insert(b->block_id);
        return;
    }
    
    ////////////////////// if parent was received but discarded, this block also needs to be discared.//////////
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

            generateBlock(chosen_txns, T, longest_chain_head->block_id);

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
            AddEvent(e,T);
            }
        }
    }

   for(int id : to_remove){
       pending_blocks.erase(id);
   }



}

void Node::broadcastBlock(Block *b, double T)
{   
    if(received[b->block_id]){
        return;
    }
   

      vector<int> neighbours = adj[node_id];

        for(int v: neighbours)
        {
            double c = (this->speed == fast && id_node_mapping[v]->speed == fast)? 1e8 : 5e6 ;

            std::exponential_distribution<double> exp_distr_for_d(c/96000);
            double d = exp_distr_for_d(gen);

            double latency = rho + 8000*(b->transactions).size()/c + d;
            //some calculation

            Event* f = new Event(ID_FOR_RECEIVE_BLOCK, v);
            f->addBlockInfo(b);
            AddEvent(f,T+latency);
        }

    
}