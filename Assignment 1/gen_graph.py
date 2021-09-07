from os import write
import networkx as nx

import sys
n = int(sys.argv[1])

g = nx.barabasi_albert_graph(n, 2)

with open("edge_list.txt",'w+') as f:
    nodes = str(n) + "\n"
    f.write(nodes)
    edg = str(len(g.edges())) + "\n"
    f.write(edg)
    for edge in g.edges():
        line = str(edge[0]) + " " + str(edge[1]) + "\n"
        f.write(line)

    n = input("Enter the fraction of slow nodes: ")
    n += "\n"
    f.write(n)
    
    T_tx = input("Enter mean interarrival time for transactions: ")
    T_tx += "\n"
    f.write(T_tx)

    T_k = input("Enter mean block mining time: ")
    T_k += "\n"
    f.write(T_k)

    

    invalid_block_nodes = input("How many nodes should produce invalid blocks?")
    invalid_block_nodes += "\n"
    f.write(invalid_block_nodes)

    for x in range(0,int(invalid_block_nodes)):
        n = input("Enter node to produce invalid block ")
        n += "\n"
        f.write(n)
    




#print(len(g.edges()))