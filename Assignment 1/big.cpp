#include<bits/stdc++.h>
#include<random>
using namespace std;


const int COINBASE_REWARD = 50;
const int ID_FOR_GEN_TRANS = 0;
const int ID_FOR_RECEIVE_TRANS = 1;
const int ID_FOR_BROADCASTING_BLOCK = 2;
const int ID_FOR_RECEIVE_BLOCK = 3;
const int ID_FOR_CHECK_AND_BROADCAST_BLOCK = 4;
const int MAX_TXNS = 900;
double T_tx;        //Transaction interarrival time mean
double T_k;         //Block interarrival time mean
double threshold;   //max time to run simulation

vector<vector<int> > adj;

std::mt19937 gen(1);

std::random_device                  rand_dev;
std::mt19937                        generator(rand_dev());

std::uniform_real_distribution<double>  distr(0.01, 0.5);
// extern std::uniform_real_distribution<double>  distr();

const double rho = distr(generator);
// const double rho = distr(generator);

class Event;
class Block;
class Node;
class Transaction;

set<pair<double, Event*> > event_queue;

 map<int, Transaction*> id_txn_mapping;

enum node_speed { fast, slow};
 map<int, Node*> id_node_mapping;
   
void AddEvent(Event *e, double time)
{
    event_queue.insert(make_pair(time,e));
}


struct BlockTreeNode {
    double arrival_time;
    int block_id;
    int level;
    // node id, balance
    map<int, int> bitcoin_balances;
    BlockTreeNode * parent;

};

int num_nodes=0;

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

    // Block b;

    
  
    //static exponential_distribution<double> transac_exp_distr(1/T_tx);
    //static mt19937 gen(1);
  
    // static int num_nodes;

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

    void gen_trans_event(double curr_time){
        cout<<" generate txn event at node: "<<node_id<<endl;
        id_node_mapping[node_id]->generateTransaction(curr_time);
    }

    void receive_trans_event(double curr_time){
        cout<<" receive txn event at node: "<<node_id<<endl;
        id_node_mapping[node_id]->receiveTransaction(this->txn, curr_time, sender_id);
    }

    void receive_block_event(double curr_time){
        cout<<" receive block event at node: "<<node_id<<endl;
        id_node_mapping[node_id]->receiveBlock(this->b, curr_time);
    }

    void broadcast_block_event(double curr_time){
        cout<<" broadcast block event at node: "<<node_id<<endl;
        id_node_mapping[node_id]->broadcastBlock(this->b, curr_time);
    }

    void check_and_broadcast_block_event(double curr_time){
        cout<<" check and broadcast block event at node: "<<node_id<<endl;
        id_node_mapping[node_id]->checkAndBroadcastBlock(this->b, curr_time);
    }
};




int num_transactions=0;
class Transaction{
    // static int num_transactions;
    public:
    int transac_id;
    
    int idx;        //sender
    int idy;        //receiver
    int c;          //coins


    Transaction()
    {
        transac_id = num_transactions;
        num_transactions++;
        // id_txn_mapping[transac_id] = this;
    }

        Transaction(int s_id, int r_id, int co)
    {
        transac_id = num_transactions;
        num_transactions++;
        idx = s_id;
        idy = r_id;
        c = co;
        // id_txn_mapping[transac_id] = this;
    }

    // this constructor is only used for creating coinbase transactions (r_id?)
    Transaction(int s_id){
        transac_id = num_transactions;
        num_transactions++;
        idx = -1;
        idy = s_id;
        c = COINBASE_REWARD;
        // id_txn_mapping[transac_id] = this;

    }
    
};


// 0 is the genesis block
map<int, Block*> id_block_mapping;  //to traverse the tree

int total_blocks_created=0;

class Block{
    // static int total_blocks_created;
public:
    int block_id;
    int previous_id;

    set<Transaction* >  transactions;

    //std::random_device rd; 
    //std::mt19937 gen(rd());
    //std::exponential_distribution<> mining_exp_distr (1/1);
    
    
    
    Block()
    {
        
    }
    Block(Transaction * t) // constructor for genesis block creation

    {
        block_id = total_blocks_created;
        total_blocks_created++;
        // id_block_mapping[block_id] = this;
        set<Transaction *> s;
        s.insert(t);
        transactions = s;
        previous_id = -1;
    }



    Block(set<Transaction *> s, int prev_id)
    {
        block_id = total_blocks_created;
        total_blocks_created++;
        // id_block_mapping[block_id] = this;
        previous_id = prev_id;
        transactions = s;
    }

};

void runSimulation()
{
    double sim_time = 0;
    while(sim_time < threshold)

    {
        sim_time = event_queue.begin() -> first;

        if(sim_time > threshold)
            break;
        
        Event* e = event_queue.begin() -> second;
        cout<<"Event dequeued at time: "<<sim_time;
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
            cout<<sim_time<<" Node "<<e->node_id<<"has to generate transaction "<<endl;
            e->gen_trans_event(sim_time);
        }
        else if(e->event_id == ID_FOR_RECEIVE_TRANS)
        {   //cout<<sim_time<<" Receiving transaction"<<endl;
            e->receive_trans_event(sim_time);
        }
        else if(e->event_id == ID_FOR_CHECK_AND_BROADCAST_BLOCK){
            e->check_and_broadcast_block_event(sim_time);
        }
        

        event_queue.erase(event_queue.begin());
    }
}


Node::Node()
{
    node_id = num_nodes;
    num_nodes++;
    // id_node_mapping[node_id] = this;
    
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
            //cout<<"About to generate txn, just checking balance\n";
        if(longest_chain_head->bitcoin_balances[node_id] != 0){
            int coins = rand() % longest_chain_head->bitcoin_balances[node_id];
            
            Transaction * Txn = new Transaction(node_id, receiving_id, coins);
            cout<<"txn: "<<Txn->transac_id<<" generated at node: "<<node_id<<endl;
            id_txn_mapping[Txn->transac_id] = Txn;
            broadcastTransaction(Txn,T,node_id);
        }
    }
    std::exponential_distribution<double> transac_exp_distr(1/T_tx);
    double sampled_next_interarrival_time =  transac_exp_distr(gen);
    double next_txn_time = T + sampled_next_interarrival_time;
    //cout<<"Generate next trans at "<<next_txn_time<<endl;
    Event * e = new Event(ID_FOR_GEN_TRANS, node_id);  
    
    AddEvent(e,next_txn_time);
}

void Node::receiveTransaction(Transaction *Txn, double T, int sender_node_id)
{
    cout<<"Node "<<node_id<<" received transaction "<<Txn->transac_id<<endl;
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
    
    Transaction* coinbase_tr = new Transaction(node_id);       //coinbase transaction
    id_txn_mapping[coinbase_tr->transac_id] = coinbase_tr;
    transac_pool.insert(coinbase_tr->transac_id);

    set<Transaction*> txns ;
    for(auto id : transac_pool){
        txns.insert(id_txn_mapping[id]);
    }
    Block* b =new Block(txns, parent_id ); // give transactions
    id_block_mapping[b->block_id] = b;
    cout<<"block id: "<<b->block_id<<" created(at t_k) at node: "<<node_id<<endl;
    
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
    cout<<"block: "<<b->block_id<<"received for checkAndBroadcastBlock at node: "<<node_id;
    int parent_id = id_block_mapping[b->block_id]->previous_id;
    if(longest_chain_head->block_id == parent_id){      
        cout<<" block was valid ";
        Event * e = new Event(ID_FOR_RECEIVE_BLOCK, node_id);
        e->addBlockInfo(b);
        AddEvent(e, T);

    }
    cout<<endl;
}
void Node::receiveBlock(Block *b, double T)
{   
    cout<<"receive block id: "<<b->block_id<<" at node: "<<node_id<<endl;
    // block has already been received, broadcasted, validate. 
    // Returning here, to unsure infinite loop in broadcasting.
    if(received[b->block_id]){
        cout<<" block already received; return receive block"<<endl;
        return;
    }
    received[b->block_id] = true;
    int parent_id = b->previous_id; 
    cout<<" parent id: "<<parent_id<<endl;

     // received genesis block
    if(parent_id == -1){
        present[0] = true;
        cout<<" genesis block received"<<endl;
        BlockTreeNode* genesis_tree_node = new BlockTreeNode();
        id_blockTreeNode_mapping[b->block_id] = genesis_tree_node;
        genesis_tree_node->arrival_time = T;
        genesis_tree_node->block_id = b->block_id;
        genesis_tree_node->level = 1;
        id_blockTreeNode_mapping[b->block_id] = genesis_tree_node;
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


        /////// create new txn set //////////////////////
        set<int> chosen_txns;
 
        generateBlock(chosen_txns, T, longest_chain_head->block_id);
    //cout<<longest_chain_head->bitcoin_balances[2]<<endl;
        return;
        
    }

    cout<<"Non genesis block received by node "<<node_id<<" having block id "<<b->block_id<<endl;
    // no need to broadcast genesis block, as it is received by all nodes at T=0
    broadcastBlock(b,T);
    



   
    //if parent not yet arrived at node, then wait till it comes
    if(!received[parent_id])
    {
        cout<<"parent block: "<<parent_id<<" not received. put in pending blocks "<<endl;
        pending_blocks.insert(b->block_id);
        return;
    }
   
    ////////////////////// if parent was received but discarded, this block also needs to be discared.//////////
    if(!present[parent_id]){
        cout<<"parent received but discarded"<<endl;
        return;

    }
    // cout<<"333333333333"<<endl;

    //if parent present
    bool valid = validateAndAddTreeNode(T, parent_id, b->block_id);

    
    // new block is valid and added to tree.
    if(valid){ 
        cout<<"received block valid"<<endl;
        present[b->block_id] = true;

        if(id_blockTreeNode_mapping[b->block_id]->level > longest_chain_head->level){ // block on which mining is done is changed


            if(longest_chain_head->block_id == parent_id){
                // can use longest_chain_txns
                cout<<"Adding transactions to already existing longest chain ones"<<endl;
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
            cout<<"Longest chain for node "<<node_id<<" now becomes "<<b->block_id<<endl;
            longest_chain_head = id_blockTreeNode_mapping[b->block_id];



            /////// create new txn set //////////////////////
            set<int> utxos;
            set_difference(all_transactions.begin(), all_transactions.end(), longest_chain_txns.begin(), longest_chain_txns.end(), inserter(utxos, utxos.begin()));
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
    
    if(!valid){
        cout<<" received block was not valid"<<endl;
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
   // if(received[b->block_id]){
    //    return;
    //}
   

      vector<int> neighbours = adj[node_id];

        for(int v: neighbours)
        {
            double c = (this->speed == fast && id_node_mapping[v]->speed == fast)? 1e8 : 5e6 ;

            std::exponential_distribution<double> exp_distr_for_d(c/96000);
            double d = exp_distr_for_d(gen);

            double latency = rho + 8000*(b->transactions).size()/c + d;
            //some calculation
            //cout<<"Node "<<v<" will receive this block "<<b->block_id<<" after "<<latency<<" seconds."<<endl;
            Event* f = new Event(ID_FOR_RECEIVE_BLOCK, v);
            f->addBlockInfo(b);
            AddEvent(f,T+latency);
        }

    
}






int main()
{
    // creating edge list from input file

    int n; // number of nodes
    cin>>n; // reading from file

    int num_edges;
    cin>>num_edges; // reading from file

    for(int i=0;i<n;i++){
        vector<int> x;
        adj.push_back(x);
    }

    //construction edge list
    for(int i=0;i<num_edges;i++){
        int x; int y;
        cin>>x>>y; // reading from file
        adj[x].push_back(y);
        adj[y].push_back(x);
    }

    for(int i=0;i<n;i++){
        Node* node =new  Node();
        id_node_mapping[i]=node;

    }

    double z;
    cout<<"Enter fraction of slow nodes in the network: "<<endl;
    cin>>z;
  
    int frac_slow_nodes = z * n;
    cout<<"frac of slow nodes: "<<frac_slow_nodes<<endl;

   set<int> id_slow_nodes;
   while(id_slow_nodes.size()<frac_slow_nodes){
       id_slow_nodes.insert(rand() % n);

   }
   for(auto id : id_slow_nodes){
       cout<<id<<" is a slow node"<<endl;
       id_node_mapping[id]->set_speed_to_slow();
   }

    
   cout<<"Enter mean interarrival time for transactions: ";
   cin>>T_tx;

   
   cout<<"Enter mean block mining time: ";
   cin>>T_k;

   //decide a random node to be given credit for genesis block having a transaction giving it 50 coins
    int idx = rand() % n;
    cout<<"idx is "<<idx<<endl;
    Transaction * t = new Transaction(idx);       //coinbase transaction
    id_txn_mapping[t->transac_id] = t;

    Block * genesis = new Block(t);               //genesis block
    id_block_mapping[genesis->block_id] = genesis;
    for(int i=0; i<n;i++){
    Event *e = new Event(ID_FOR_RECEIVE_BLOCK, i);
    e->addBlockInfo(genesis);
    AddEvent(e,0);
    }

    
    
    // trigger transaction generation for each node
    for(int i=0;i<n;i++){
        Event *e = new Event(ID_FOR_GEN_TRANS, i);
        AddEvent(e,0);
    }
    
    

    // simple 1-neighbour connected graph
    for(int i=0;i<n-1;i++)
    {
        adj[i].push_back(i+1);
        adj[i+1].push_back(i);
    }
    //create an adjacency matrix out of these nodes (should be a global variable in classes.h)

    //create latency matrix (again global variable) or leave the code as it is

    

    threshold = 10.0;

    runSimulation();    


}





// #endif