#include "classes.h"
using namespace std;

int main()
{

    /////////////////////// simulator class?????????????????????????????????????????
    Simulate Simulator = Simulate();
    
    int n; 

    cout<<"Enter the number of nodes in the network: "<<endl;
    cin>>n;

    for(int i=0;i<n;i++){
        Node node = Node();

    }

    double z;
    cout<<"Enter fraction of slow nodes in the network: "<<endl;
    cin>>z;

    int frac_slow_nodes = (int) z * n;

   set<int> id_slow_nodes;
   while(id_slow_nodes.size()<frac_slow_nodes){
       id_slow_nodes.insert(rand() % n);

   }
   for(auto id : id_slow_nodes){
       id_node_mapping[id]->set_speed_to_slow();
   }

    double T_tx;
   cout<<"Enter mean interarrival time for transactions: ";
   cin>>T_tx;

   double T_k;
   cout<<"Enter mean block mining time: ";
   cin>>T_k;

   //decide a random node to be given credit for genesis block having a transaction giving it 50 coins
    int idx = rand() % n;
    Transaction * t = new Transaction(idx);       //coinbase transaction

    Block * genesis = new Block(t);               //genesis block

    for(int i=0; i<n;i++){
    Event *e = new Event(ID_FOR_RECEIVE_BLOCK, i);
    e->addBlockInfo(genesis);
    Simulator.AddEvent(e,0);
    }
    
    // trigger transaction generation for each node
    for(int i=0;i<n;i++){
        Event *e = new Event(ID_FOR_GEN_TRANS, i);
        Simulator.AddEvent(e,0);
    }
    

    //create an adjacency matrix out of these nodes (should be a global variable in classes.h)

    //create latency matrix (again global variable)

    //broadcast genesis block to all nodes
    

    

    Simulator.runSimulation();    


}