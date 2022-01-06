Steps for running code:

1) Install the python dependency networkx using $pip3 install networkx
2) Run $python3 gen_graph.py #num_nodes
3) This will prompt to enter the parameters for simulation, enter those.
4) An output file edge_list.txt is generated
5) Run $mkdir outputs
6) Run $g++ big.cpp -std=c++11
7) Run $./a.out < edge_list.txt > out.txt
8) Output files out.txt, frac_nodes_in_longest_chain.txt are generated which can be ignored (can use in demo). All tree files are generated in "outputs" folder by the name tree_<node id>.txt. there are other .dot and .png files as well.