#include<bits/stdc++.h>
#include<random>
using namespace std;


int COINBASE_REWARD = 50;
int ID_FOR_GEN_TRANS = 0;
int ID_FOR_RECEIVE_TRANS = 1;
int ID_FOR_BROADCASTING_BLOCK = 2;
int ID_FOR_RECEIVE_BLOCK = 3;
int ID_FOR_CHECK_AND_BROADCAST_BLOCK = 4;
int MAX_TXNS = 900;
double T_tx;        //Transaction interarrival time mean
double T_k;         //Block interarrival time mean
double threshold;   //max time to run simulation

vector<vector<int> > adj;

class Event;
set<pair<double, Event*> > event_queue;
   
void AddEvent(Event *e, int time)
{
    event_queue.insert(make_pair(time,e));
}

void runSimulation()
{
    double sim_time = 0;
    while(sim_time < threshold)
    {
        sim_time = event_queue.begin() -> first;
        Event* e = event_queue.begin() -> second;
        //vector<Event*> v = event_queue.begin() -> second;
        
        if(e->event_id == ID_FOR_RECEIVE_BLOCK)
        {
            e->receive_block_event(sim_time);
        }
        else if(e->event_id == ID_FOR_BROADCASTING_BLOCK)
        {
            e->broadcast_block_event(sim_time);
        }
        else if(e->event_id == ID_FOR_GEN_TRANS)
        {
            e->gen_trans_event(sim_time);
        }
        else if(e->event_id == ID_FOR_RECEIVE_TRANS)
        {
            e->receive_trans_event(sim_time);
        }
        else if(e->event_id == ID_FOR_CHECK_AND_BROADCAST_BLOCK){
            e->check_and_broadcast_block_event(sim_time);
        }
        

        event_queue.erase(event_queue.begin());
    }
}



std::mt19937 gen(1);

std::random_device                  rand_dev;
std::mt19937                        generator(rand_dev());
std::uniform_real_distribution<double>  distr(0.01, 0.5);

double rho = distr(generator);


struct BlockTreeNode {
    double arrival_time;
    int block_id;
    int level;
    // node id, balance
    map<int, int> bitcoin_balances;
    BlockTreeNode * parent;

};


/*
class Simulate{
    
    public:
    
     map<int,vector<Event*> > event_queue;
   
    void  AddEvent(Event *e, int time);

    void runSimulation();

};
*/
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

    //std::random_device rd; 
    //std::mt19937 gen(rd());
    //std::exponential_distribution<> mining_exp_distr (1/1);
    
    
    
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

    //true if block i is in the tree.
    map<int, bool> present;

    // true if block i was received;
    map<int, bool> received;

    Block* block_at_t;
    

    
    int max_height;

    node_speed speed;

    Block b;

    
  
    //static exponential_distribution<double> transac_exp_distr(1/T_tx);
    //static mt19937 gen(1);
  
    static int num_nodes;

    int num_node = 0;
    Node();

    void set_speed_to_slow();
    bool findBalances(int block_id);

    void generateTransaction(double T);

    void receiveTransaction(Transaction *Txn, double T, int sender_node_id);

    void broadcastTransaction(Transaction *txn, double T, int sender_node_id);

    bool validateAndAddTreeNode(double arrival_time, int parent_id, int b_id);
    
    void generateBlock(set<int> transac_pool,double T, int parent_id);

    void checkAndBroadcastBlock(Block *b, double T);

    void receiveBlock(Block *b, double T);
    void broadcastBlock(Block *b, double T);

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

    void gen_trans_event(double curr_time);

    void receive_trans_event(double curr_time);

    void receive_block_event(double curr_time);

    void broadcast_block_event(double curr_time);

    void check_and_broadcast_block_event(double curr_time);
};







