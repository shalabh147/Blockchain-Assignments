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



    void generateBlock(int T)
    {
        //choose some subset of transactions from transaction_pool, say sub
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
        set<Transaction> txns = b->transactions;
        bool invalid = false;
        for(auto txn: txns)
        {
            int idx = txn->idx;
            int c = txn->c;
            int idy = txn->idy;

            if(bitcoin_map[idx] < c)
            {
                invalid = true;
                break;
            }

            bitcoin_map[idx] -= c;
            bitcoin_map[idy] += c;   
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
        if(invalid == false)
        {
            if(last_block_created_time + last_wait_interval > T)
            {
                //cancel event scheduled
                int time_to_cancel = last_block_created_time + last_wait_interval;
                Simulator.removeEvent(id_for_broadcasting_block,uniq_id,time_to_cancel);
            }

            int parent_id = b->previous_id;
            tree_blocks[parent_id].push_back(b->id);
            height[b->id] = height[parent_id] + 1;

            if(height[b->id] > max_height)
            {
                max_height = height[b->id];
                generateBlock(T);
                //before generating, first need to remove from transaction pool all transactions of largest chain
            }
        }

        
        
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
    Block b;
    Transaction txn;

    public:
    Event(int e_id, int b_id)
    {
        block_id = b_id;
        event_id = e_id;
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


};


class Transaction{

    
};


class Block{


};

