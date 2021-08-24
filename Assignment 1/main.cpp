#include "classes.h"
using namespace std;

int main()
{
    //creating the genesis block
    Block gen_block = Block();
    
    int n; 

    cout<<"Enter the number of nodes in the network: "<<endl;
    cin>>n;

    for(int i=0;i<n;i++){
        Node node = Node();

    }

    int z;
    cout<<"Enter fraction of slow nodes in the network: "<<endl;
    cin>>z;

    int frac_slow_nodes = (int) z * n;

   set<int> id_slow_nodes;
   while(id_slow_nodes.size()<frac_slow_nodes){
       id_slow_nodes.insert(rand() % n);

   }
   for(auto id : id_slow_nodes){
       id_node_mapping.find(id)->second->set_speed_to_slow();
   }

    double T_tx;
   cout<<"Enter mean interarrival time for transactions: ";
   cin>>T_tx;

   
    

    //create an adjacency matrix out of these nodes (should be a global variable in classes.h)

    //create latency matrix (again global variable)


    

    Simulate Simulator = Simulate();
    


}