#include<bits/stdc++.h>
using namespace std;


int COINBASE_REWARD = 50;
int ID_FOR_GEN_TRANS = 0;
int ID_FOR_RECEIVE_TRANS = 1;
int ID_FOR_BROADCASTING_BLOCK = 2;
int ID_FOR_RECEIVE_BLOCK = 3;
int MAX_TXNS = 900;


struct BlockTreeNode {
    int arrival_time;
    int block_id;
    int level;
    // node id, balance
    map<int, int> bitcoin_balances;
    BlockTreeNode * parent;

};

class Event;

class Simulate{
    
    public:
    
    static map<int,vector<Event*> > event_queue;
    Simulate();
    void static AddEvent(Event *e, int time);
    void static removeEvent(int event_id, int node_id, int time);
    void runSimulation();

};
class Transaction;

map<int, Transaction*> id_txn_mapping;
class Transaction{
    static int num_transactions;
    public:
    int transac_id;
    
    int idx;        //sender
    int idy;        //receiver
    int c;          //coins


    Transaction();

    Transaction(int s_id, int r_id, int co);

    // this constructor is only used for creating coinbase transactions (r_id?)
    Transaction(int s_id);
    
};

class Block;
// 0 is the genesis block
map<int, Block*> id_block_mapping;  //to traverse the tree

class Block{
    static int total_blocks_created;
public:
    int block_id;
    int previous_id;

    set<Transaction* >  transactions;

     static exponential_distribution<double> mining_exp_distr (1/T_k);
    static mt19937 gen(1);
    Block();

    // this constructor to be only used of genesis block creation.
    Block(Transaction * t);

    Block(set<Transaction *> s, int prev_id);

};

enum node_speed { fast, slow};
map<int, Node*> id_node_mapping;

class Node{
    public:
    int node_id;
    

    set<int> block_chain_leaves;
    BlockTreeNode* longest_chain_head;
    map<int, BlockTreeNode*> id_blockTreeNode_mapping;
    
    // parent[i] gives the id of the parent block for block i, in the tree of blocks
    map<int,int> parent;

    // map[i] gives the balance of node i in longest chain of the node. correctness ensured only after findbalances called.
    map<int,int> bitcoin_map;

    // heights[i] gives the height of i th block in the tree of blocks
    map<int,int> heights;

    set<int> all_transactions;

    // transactions in current longest chain
    set<int> longest_chain_txns;

    //those blocks recieved whose parent not received yet


    set<int> pending_blocks;

    //true if block i has been received already
    map<int, bool> present;

    
    int last_block_created_time;
    int last_wait_interval;
    int max_height;

    node_speed speed;

    Block b;

    
  
    static exponential_distribution<double> transac_exp_distr(1/T_tx);
    static mt19937 gen(1);
  
    static int num_nodes;

    int num_node = 0;
    Node();

    void set_speed_to_slow();
    bool findBalances(int block_id);

    void generateTransaction(int T);

    void receiveTransaction(Transaction *Txn, int T, int sender_node_id);

    void broadcastTransaction(Transaction *txn, int T, int sender_node_id);

    bool validateAndAddTreeNode(int arrival_time, int parent_id, int b_id);
    
    void generateBlock(set<int> transac_pool,int T, int parent_id);

    void receiveBlock(Block *b, int T);
    void broadcastBlock(Block *b, int T);

};

class Event{
    
    public:

    int sender_id;
    int event_id; //0 for generating_trans, 1 for receiving_trans, 2 for broadcasting_block, 3 for receiving_block 
    int node_id; // id at which event occurs
    int block_id;

    Block * b;
    Transaction * txn;

    Event(int e_id, int s_id, int n_id);

    Event(int e_id, int n_id);
   
    void addBlockInfo(Block * c);

    void addTransactionInfo(Transaction * t);

    void gen_trans_event(int curr_time);

    void receive_trans_event(int curr_time);

    void receive_block_event(int curr_time);

    void broadcast_block_event(int curr_time);
};







