// SPDX-License-Identifier: GPL-3.0
pragma solidity ^0.4.24;


contract Coin {

    struct Adjacency{
    uint user_id;
    uint[] neighbours;
    }

    mapping (uint => mapping(uint => uint)) public joint_balances;      //joint_balances[id1][id2] will give balance of id1 on id1-id2 edge
    mapping (uint => bool) public visited;      //the visited array for bfs, set all visited to false after bfs complete
    mapping (uint => uint) public parent;       //parent array to get backtrack path, set parent to -1 after bfs complete
    uint[] queue;                               //array for bfs queue
    uint[] path;                                //array for path
    mapping (uint => string) register;
   
    Adjacency[] adj;
    uint public count;
    
    
    constructor() {
    
    }
    
    event send(address indexed from, uint count);       //event to return count in logs
    

    function AddAdj(uint user_id) public {              //adding new user to adjacency list

        Adjacency memory a = Adjacency(user_id, new uint[](0));
        adj.push(a);
    }
    function registerUser(uint user_id, string memory username) public {        
        register[user_id] = username;
        AddAdj(user_id);
        visited[user_id] = false;
        
    }

    function createAcc(uint user_id_1, uint user_id_2, uint bal1, uint bal2) public {
        
        adj[user_id_1].neighbours.push(user_id_2);              //adding u1 to neighbours of u2 and vice versa, also updating balance
        adj[user_id_2].neighbours.push(user_id_1);
        joint_balances[user_id_1][user_id_2] = bal1;
        joint_balances[user_id_2][user_id_1] = bal2;
        
    }

    function sendAmount(uint user_id_1, uint user_id_2, uint amount) public returns(bool){

        
        uint i = 0;
        uint j = 0;
        queue.push(user_id_1);
        j = 1;
        
        //running bfs
        while(i!=j)
        {
            uint ele = queue[i];
            i++;
            if(visited[ele])
                continue;

            visited[ele] = true;
            if(ele == user_id_2)
                break;

            for(uint k=0; k<adj[ele].neighbours.length; k++)
            {
                uint child = adj[ele].neighbours[k];
                if(visited[child] || joint_balances[ele][child] < amount)
                    continue;
                
                queue.push(child);
                parent[child] = ele;
                j++;
            }
        }

        //reconstructing path
        bool emitted=false;
        if(visited[user_id_2])
        {   uint id = user_id_2;
                while(id!=user_id_1)
                {
                    path.push(id);
                    id = parent[id];
                }
                
                path.push(user_id_1);

            for(uint kl = path.length-1;kl>0;kl--)
            {

                joint_balances[path[kl]][path[kl-1]] -= amount;
                joint_balances[path[kl-1]][path[kl]] += amount;
            
            }
        }
        else
        {
            
          emit send(msg.sender, 0);
          emitted=true;
        //   return false;         //transaction not successful
          
          
        }


        //empty queue and path arrays
        while(queue.length > 0)
        {
            delete queue[queue.length-1];
            queue.length--;
        }

        while(path.length > 0)
        {
            delete path[path.length-1];
            path.length--;
        }

        for(uint num2=0;num2<adj.length;num2++)
        {
            visited[adj[num2].user_id] = false;
        }

      count++;              //transaction was successful so count updated
      if(emitted){
          
      }
      else{
           emit send(msg.sender, 1);
      }
     

    //   return true;
      

        
    }


    

    function getCount() public {
        emit send(msg.sender, count);
    }

    function closeAccount(uint user_id_1, uint user_id_2) public {

        joint_balances[user_id_1][user_id_2] = 0;
        joint_balances[user_id_2][user_id_1] = 0;
        
    }
}