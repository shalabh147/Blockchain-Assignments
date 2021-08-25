#include<bits/stdc++.h>
using namespace std;



int COINBASE_REWARD = 50;
int ID_FOR_GEN_TRANS = 0;
int ID_FOR_RECEIVE_TRANS = 1;
int ID_FOR_BROADCASTING_BLOCK = 2;
int ID_FOR_RECEIVE_BLOCK = 3;

class Event;

class Simulate{
    static map<int,vector<Event*> > event_queue;
    public:
    Simulate()
    {

    }

    void static AddEvent(Event *e, int time)
    {
        event_queue[time].push_back(e);
    }

    void static removeEvent(int event_id, int node_id, int time)
    {
        for(int i=0;i<event_queue[time].size();i++)
        {
            if(event_queue[time][i]->event_id == event_id && event_queue[time][i]->node_id == node_id)
            {
                auto iterator = event_queue[time].begin() + i;
                event_queue[time].erase(iterator);
                break;
            }
        }
    }



};
class Transaction{
    static int num_transactions;
    public:
    int transac_id;
    

    int idx;        //sender
    int idy;        //receiver
    int c;          //coins

    

    Transaction()
    {
        transac_id = num_transactions;
        num_transactions++;
    }

    Transaction(int s_id, int r_id, int co)
    {
        transac_id = num_transactions;
        num_transactions++;
        idx = s_id;
        idy = r_id;
        c = co;
    }


    // this constructor is only used for creating coinbase transactions
    Transaction(int s_id){
        transac_id = num_transactions;
        num_transactions++;
        idx = s_id;
        idy = -1;
        c = COINBASE_REWARD;

    }
    
};

class Block;
// 0 is the genesis block
map<int, Block*> id_block_mapping;  //to traverse the tree

class Block{
    static int total_blocks_created;
public:
    block_id_type block_id;
    int previous_id;
   


     set<Transaction> transactions;
    // this constructor to be only used of genesis block creation.
    Block()
    {
        block_id = total_blocks_created;
        total_blocks_created++;
        id_block_mapping[block_id] = this;
    }



    Block(set<Transaction> s, int prev_id)
    {
        block_id = total_blocks_created;
        total_blocks_created++;
        id_block_mapping[block_id] = this;
        previous_id = prev_id;
        transactions = s;
    }

};

class Node;
enum node_speed { fast, slow};
map<int, Node*> id_node_mapping;

class Node{
    
    int node_id;
    vector<vector<int> > tree_blocks;
    
    // parent[i] gives the id of the parent block for block i, in the tree of blocks
    map<int,int> parent;

    // map[i] gives the balance of node i in longest chain of the node. correctness ensured only after findbalances called.
    map<int,int> bitcoin_map;

    // heights[i] gives the height of i th block in the tree of blocks
    map<int,int> heights;

    set<int> all_transactions;

    // all transactions - transactions in longest chain
    set<int> current_transaction_pool;

    
    int last_block_created_time;
    int last_wait_interval;
    int max_height;

    node_speed speed;

    Block b;

    public:

     
  
  static exponential_distribution<int> transac_exp_distr (1/T_tx);
  static mt19937 gen(1);
  
    static int num_nodes;

    int num_node = 0;
    Node()
    {
        node_id = num_nodes;
        num_nodes++;
        id_node_mapping[node_id] = this;
        
        parent[0] = -1; // genesis block has no parent
        heights[0] =1;
        max_height = 1;
        speed = fast;
       
    }

   void set_speed_to_slow(){
       this->speed = slow;  
   }


    // vrinda does not like this :( its okay, we'll find a better way for you :)
    bool findBalances(int block_id)         //traverse the tree up from block_id to find balances and validate transactions
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
       

    

    void generateTransaction(int T)
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

    void receiveTransaction(Transaction *Txn, int T, int sender_node_id)
    {
        all_transactions.insert(Txn->transac_id);

                                    // sender_node_id
        broadcastTransaction(Txn,T, node_id);
    }

    void broadcastTransaction(Transaction *txn, int T, int sender_node_id)
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



    void generateBlock(set<int> transac_pool,int T)
    {
        //choose some subset of transactions from transac_pool, say sub

       Block* b =new  Block(); // give transactions
        last_block_created_time = T;

        int t_k = rand(something);
        last_wait_interval = t_k;

        int new_time = T + t_k;
        Event * e = new Event(ID_FOR_BROADCASTING_BLOCK,b->block_id);
        e->addBlockInfo(b);
        Simulate::AddEvent(e,new_time);

    }

    void receiveBlock(Block *b, int T)
    {   
        BroadcastBlock(b,T);
        int parent_id = b->previous_id; 

       

        }
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
                Simulator.removeEvent(id_for_broadcasting_block,uniq_id,time_to_cancel);
            }


            tree_blocks[parent_id].push_back(b->id);
            
            parent[b->block_id] = b->previous_id;
            heights[b->block_id] = heights[parent_id] + 1;
            max_height = heights[b->block_id];
        

            //calculate transaction pool by removing all transactions corresponding to this longest chain
            generateBlock(transac_pool,T);
       
        }


    }

    void BroadcastBlock(Block *b, int T)
    {
        if(max_height == heights[b->id])         //if still the largest chain, only then broadcast last formed block
        {
            vector<int> neighbours = adj[uniq_id];

            for(int v: neighbours)
            {
                int l = latency[uniq_id][v];
                //some calculation

                Event* f = new Event(ID_FOR_RECEIVE_BLOCK, v);
                f->addBlockInfo(b);
                Simulate::AddEvent(f,T+l);
            }

        }
    }

};



class Event{
    int sender_id;
    int event_id; //0 for generating_trans, 1 for receiving_trans, 2 for broadcasting_block, 3 for receiving_block 
    int node_id; // id at which event occurs
    int block_id

    Block * b;
    Transaction * txn;

    public:
    Event(int e_id, int s_id, int n_id)
    {
        sender_id = s_id;
        event_id = e_id;
        node_id = n_id;
    }

    Event(int e_id, int n_id){
        event_id = e_id;
        node_id = n_id;
    }
   

    

    void addBlockInfo(Block * c)
    {
        b = c;
    }

    void addTransactionInfo(Transaction * t)
    {
        txn = t;
    }

    void gen_trans_event(int curr_time){
        id_node_mapping.find(node_id)->second->generateTransaction(curr_time);
    }

    void receive_trans_event(int curr_time){
        id_node_mapping.find(node_id)->second->receiveTransaction(this->txn, curr_time, sender_id);
    }

    void receive_block_event(int curr_time){
        id_node_mapping.find(node_id)->second->receiveBlock(this->b, curr_time);
    }



};







