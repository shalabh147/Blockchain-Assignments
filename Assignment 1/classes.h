#include<bits/stdc++.h>
using namespace std;

class Node{
    int uniq_id;
    vector<vector<int> > tree_blocks;
    vector<int> height;
    set<int> transaction_pool;
    map<int,int> bitcoin_map;
    int last_block_created_time;
    int last_wait_interval;
    int max_height;

    Block b;

    public:
    Node()
    {

    }

    void generateTransaction(int T)
    {
        int receiving_id = random;
        int coins = random;
        Transaction Txn = Transaction(uniq_id, receiving_id, coins);
        broadcastTransaction(Txn,T);

        int next_txn_time = T + random;
        Event e = Event(id_for_gen_transaction, uniq_id);
        Simulator.Add_Event(e,next_txn_time);
    }

    void receiveTransaction(Transaction *Txn, int T)
    {
        transaction_pool.insert(Txn->id);
        broadcastTransaction(Txn,T);
    }

    void broadcastTransaction(Transaction *Txn, int T)
    {
        vector<int> neighbours = adj[uniq_id];

        for(int v: neighbours)
        {
            //ensure loopless forwarding
            int l = latency[uniq_id][v];
            //some calculation

            Event f = Event(id_for_receiving_transaction_event, v);
            f->addTransactionInfo(txn);
            Simulator.Add_Event(f,T+l);
        }
    }



    void generateBlock(set<int> transac_pool,int T)
    {
        //choose some subset of transactions from transac_pool, say sub
        b = Block(sub);
        last_block_created_time = T;

        int t_k = rand(something);
        last_wait_interval = t_k;

        int new_time = T + t_k;
        Event e = Event(id_for_broadcasting_block,block_id);

        Simulator.Add_Event(e,new_time);

    }

    void receiveBlock(Block *b, int T)
    {   
        BroadcastBlock(b,T);
        int parent_id = b->previous_id; 
        //if parent not yet present with node, then wait till it comes

        //if parent present
        

        bool invalid = false;
        //validate transactions of received block using this chain of parents.

        if(invalid == true)
        {
            return;
        }
            
        if(height[parent_id] == max_height)
        {
            if(last_block_created_time + last_wait_interval > T)
            {
                //cancel event scheduled
                int time_to_cancel = last_block_created_time + last_wait_interval;
                Simulator.removeEvent(id_for_broadcasting_block,uniq_id,time_to_cancel);
            }


            tree_blocks[parent_id].push_back(b->id);
            height[b->id] = height[parent_id] + 1;
            max_height = height[b->id];

            //calculate transaction pool by removing all transactions corresponding to this longest chain
            generateBlock(transac_pool,T);
       
        }

/*
        if(invalid == false)
        {
            for(auto txn: txns)
            {
                if(transaction_pool.find(txn->id)!=transaction_pool.end())
                {
                    transaction_pool.erase(txn->id);
                }
            }
        }
*/  
        
        //Event e = Event(id_for_generating_block,block_id);

        //Simulator.Add_Event(e,new_time);

    }

    void BroadcastBlock(Block *b, int T)
    {
        if(max_height == height[b->id])         //if still the largest chain, only then broadcast last formed block
        {
            vector<int> neighbours = adj[uniq_id];

            for(int v: neighbours)
            {
                int l = latency[uniq_id][v];
                //some calculation

                Event f = Event(id_for_receiving_block_event, v);
                f->addBlockInfo(b);
                Simulator.Add_Event(f,T+l);
            }

        }
    }

};



class Event{
    int block_id;
    int event_id; //0 for generating_trans, 1 for receiving_trans, 2 for broadcasting_block, 3 for receiving_block 
    int node_id;

    Block b;
    Transaction txn;

    public:
    Event(int e_id, int b_id, int n_id)
    {
        block_id = b_id;
        event_id = e_id;
        node_id = n_id;
    }

    void addBlockInfo(Block c)
    {
        b = c;
    }

    void addTransactionInfo(Transaction t)
    {
        txn = t;
    }


};


class Simulate{
    map<int,vector<Event*> > event_queue;
    public:
    Simulate()
    {

    }

    void AddEvent(Event *e, int time)
    {
        event_queue[time].push_back(e);
    }

    void removeEvent(int event_id, int node_id, int time)
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

    int transac_id;
    int idx;        //sender
    int idy;        //receiver
    int c;          //coins
    
};


class Block{

    int block_id;
    int previous_id;
    set<Transaction> transactions;

};

