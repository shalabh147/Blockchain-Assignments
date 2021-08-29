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

print(len(g.edges()))