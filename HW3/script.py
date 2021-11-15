import networkx as nx
import time
from web3 import *
from solc import compile_source
import os
import random

nodes = 100

def compile_source_file(file_path):
   with open(file_path, 'r') as f:
      source = f.read()
   return compile_source(source)

def read_address_file(file_path):
    file = open(file_path, 'r')
    addresses = file.read().splitlines() 
    return addresses


def connectWeb3():
    # print(os.environ['HOME']+'/HW3/test-eth1/geth.ipc')
    return Web3(IPCProvider(os.environ['HOME']+'/HW3/test-eth1/geth.ipc', timeout=100000))
    


def deployEmptyContract(contract_source_path, w3, account):
    compiled_sol = compile_source_file(contract_source_path)
    contract_id, contract_interface3 = compiled_sol.popitem()
    curBlock = w3.eth.getBlock('latest')
    tx_hash = w3.eth.contract(
            abi=contract_interface3['abi'],
            bytecode=contract_interface3['bin']).constructor(4).transact({'txType':"0x0", 'from':account, 'gas':1000000})
    return tx_hash


def deployContracts(w3, account):
    source_path = os.environ['HOME']+'/HW3/mainFunc.sol'
    tx_hash3 = deployEmptyContract(source_path, w3, account)

    
    receipt3 = w3.eth.getTransactionReceipt(tx_hash3)

    while ((receipt3 is None)) :
        time.sleep(1)
        receipt3 = w3.eth.getTransactionReceipt(tx_hash3)

    w3.miner.stop()

    
    if receipt3 is not None:
        print("empty:{0}".format(receipt3['contractAddress']))
        return receipt3['contractAddress']



def make_account(con, id1, id2):
    contract_source_path = os.environ['HOME']+'/HW3/mainFunc.sol'
    compiled_sol = compile_source_file(contract_source_path)

    contract_id, contract_interface = compiled_sol.popitem()

    sort_contract = w3.eth.contract(
    address=con,
    abi=contract_interface['abi'])
    tx_hash = sort_contract.functions.createAcc(id1, id2).transact({'txType':"0x3", 'from':w3.eth.accounts[0], 'gas':2409638})

    w3.eth.waitForTransactionReceipt(tx_hash)


    


def send_amount(con, id1, id2):
    contract_source_path = os.environ['HOME']+'/HW3/mainFunc.sol'
    compiled_sol = compile_source_file(contract_source_path)

    contract_id, contract_interface = compiled_sol.popitem()

    sort_contract = w3.eth.contract(
    address=con,
    abi=contract_interface['abi'])
    tx_hash = sort_contract.functions.sendAmount(id1, id2).transact({'txType':"0x3", 'from':w3.eth.accounts[0], 'gas':2409638})

    w3.eth.waitForTransactionReceipt(tx_hash)



def remove_account(con, id1, id2):
    contract_source_path = os.environ['HOME']+'/HW3/mainFunc.sol'
    compiled_sol = compile_source_file(contract_source_path)

    contract_id, contract_interface = compiled_sol.popitem()

    sort_contract = w3.eth.contract(
    address=con,
    abi=contract_interface['abi'])
    tx_hash = sort_contract.functions.closeAccount(id1, id2).transact({'txType':"0x3", 'from':w3.eth.accounts[0], 'gas':2409638})

    w3.eth.waitForTransactionReceipt(tx_hash)



w3 = connectWeb3()
w3.miner.start(1)
time.sleep(4)
con = deployContracts(w3, w3.eth.accounts[0])

#make graph with 100 nodes
n = 100
g = nx.barabasi_albert_graph(n, 3)

for edge in g.edges():
    make_account(con, edge[0], edge[1])

for x in range(1000):
    if x%100 == 0:
        #note ratio etc
        pass
    
    lst = random.sample(range(0, n), 2)
    send_amount(con, lst[0], lst[1])

# with open("edge_list.txt",'w+') as f:
#     nodes = str(n) + "\n"
#     f.write(nodes)
#     edg = str(len(g.edges())) + "\n"
#     f.write(edg)
#     for edge in g.edges():
#         line = str(edge[0]) + " " + str(edge[1]) + "\n"
#         make_account(con, edge[0], edge[1])


        

