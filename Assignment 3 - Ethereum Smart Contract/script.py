import networkx as nx
import time
from web3 import *
from solc import compile_source
import os
import random
import numpy as np
import snap

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
    return Web3(IPCProvider(os.environ['HOME']+'/HW3/test-eth1/geth.ipc', timeout=100000))
    

#deploying contract on ethereum node
def deployEmptyContract(contract_source_path, w3, account):
    compiled_sol = compile_source_file(contract_source_path)
    contract_id, contract_interface3 = compiled_sol.popitem()
    curBlock = w3.eth.getBlock('latest')
    tx_hash = w3.eth.contract(
            abi=contract_interface3['abi'],
            bytecode=contract_interface3['bin']).constructor().transact({'txType':"0x0", 'from':account, 'gas':1000000})
    return tx_hash


def deployContracts(w3, account):
    source_path = os.environ['HOME']+'/HW3/mainFunc3.sol'
    tx_hash3 = deployEmptyContract(source_path, w3, account)

    
    receipt3 = w3.eth.getTransactionReceipt(tx_hash3)
    w3.miner.start(1)
    while ((receipt3 is None)) :
        time.sleep(1)
        receipt3 = w3.eth.getTransactionReceipt(tx_hash3)

    w3.miner.stop()

    
    if receipt3 is not None:
        print("empty:{0}".format(receipt3['contractAddress']))
        return receipt3['contractAddress']


#helper function for register user. calls registerUser in the contract
def register_user(con, user):
    contract_source_path = os.environ['HOME']+'/HW3/mainFunc3.sol'
    compiled_sol = compile_source_file(contract_source_path)

    contract_id, contract_interface = compiled_sol.popitem()

    sort_contract = w3.eth.contract(
    address=con,
    abi=contract_interface['abi'])
    tx_hash = sort_contract.functions.registerUser(user, str(user)).transact({'txType':"0x3", 'from':w3.eth.accounts[0], 'gas':2409638})

    return tx_hash
 


#helper function for make account. calls createAccount in the contract
def make_account(con, id1, id2,v):
    contract_source_path = os.environ['HOME']+'/HW3/mainFunc3.sol'
    compiled_sol = compile_source_file(contract_source_path)

    contract_id, contract_interface = compiled_sol.popitem()
    
    coins = int( np.random.exponential(10) * 0.5)
    if v%50==0:
        print(",,,,,",v, id1, id2, "have coins ", coins)
    
    sort_contract = w3.eth.contract(
    address=con,
    abi=contract_interface['abi'])
    tx_hash = sort_contract.functions.createAcc(id1, id2, coins, coins).transact({'txType':"0x3", 'from':w3.eth.accounts[0], 'gas':2409638})

    return tx_hash



    

#helper function for send amount. calls sendAmount in the contract
def send_amount(con, id1, id2):
    contract_source_path = os.environ['HOME']+'/HW3/mainFunc3.sol'
    compiled_sol = compile_source_file(contract_source_path)

    contract_id, contract_interface = compiled_sol.popitem()

    sort_contract = w3.eth.contract(
    address=con,
    abi=contract_interface['abi'])
    tx_hash = sort_contract.functions.sendAmount(id1, id2, 1).transact({'txType':"0x3", 'from':w3.eth.accounts[0], 'gas':240963800})
    w3.miner.start(1)
    receipt3 = w3.eth.getTransactionReceipt(tx_hash)

    while ((receipt3 is None)) : 
        time.sleep(0.1)

        receipt3 = w3.eth.getTransactionReceipt(tx_hash)
    

    receipt3 = w3.eth.getTransactionReceipt(tx_hash)

    w3.miner.stop()
    receipts = sort_contract.events.send().processReceipt(receipt3)
    receipts=receipts[0]['args']['count']
  
    return receipts


#helper function for remove account. calls removeAccount in the contract
def remove_account(con, id1, id2):
    contract_source_path = os.environ['HOME']+'/HW3/mainFunc3.sol'
    compiled_sol = compile_source_file(contract_source_path)

    contract_id, contract_interface = compiled_sol.popitem()

    sort_contract = w3.eth.contract(
    address=con,
    abi=contract_interface['abi'])
    tx_hash = sort_contract.functions.closeAccount(id1, id2).transact({'txType':"0x3", 'from':w3.eth.accounts[0], 'gas':2409638})

    return tx_hash
  


w3 = connectWeb3()

time.sleep(4)
con = deployContracts(w3, w3.eth.accounts[0])

print("*****deploy contracts done***")
print(con)
#make 
# graph with 100 nodes
n = 100
g = nx.barabasi_albert_graph(n, 2)


#create node network in ethereum by registering users
for i in range(n):
    if i%10==0:
        print("registered: ", i)
    tx_hash = register_user(con, i)
    w3.miner.start(1)
    receipt3 = w3.eth.getTransactionReceipt(tx_hash)

    while ((receipt3 is None)) : 
        time.sleep(0.1)

        receipt3 = w3.eth.getTransactionReceipt(tx_hash)
    


    receipt3 = w3.eth.getTransactionReceipt(tx_hash)
    w3.miner.stop()
   
print("*** registered users*******")


#fill node network by adding edges(joint accounts) in ethereum
v=0
for edge in g.edges():
    v+=1
    txn_hash = make_account(con, edge[0],edge[1],v)
    w3.miner.start(1)
    receipt3 = w3.eth.getTransactionReceipt(tx_hash)

    while ((receipt3 is None)) : 
        time.sleep(0.1)

        receipt3 = w3.eth.getTransactionReceipt(tx_hash)
    


    receipt3 = w3.eth.getTransactionReceipt(tx_hash)
    w3.miner.stop()
print("*** accounts created*******")

#send amount to accounts, main 1000 transactions and analysis
per_hundred=[]
prev_cnt4=0
prev_cnt=0
curr_cnt=0
for x in range(1000):
    if x%4 == 0:
        prev_cnt4=curr_cnt
    if x%100==0:
        per_hundred.append(curr_cnt-prev_cnt)
        prev_cnt=curr_cnt
    
    lst = random.sample(range(0, n), 2)
    tx_hash = send_amount(con, lst[0], lst[1])
    if tx_hash==1:
        
        curr_cnt+=1
    



print("hundred: ", per_hundred)