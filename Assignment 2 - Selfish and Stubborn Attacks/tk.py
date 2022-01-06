import matplotlib.pyplot as plt
x=[0.5,1,5,6,8,10,20]
att_stubborn=[]
att_selfish=[]
over_stubborn=[]
over_selfish=[]
br=[1/i for i in r]
print(br)
plt.plot(x,br)
plt.xscale("log")
plt.title("Branching (1/r) with number of nodes")
plt.ylabel("Branching")
plt.xlabel("Number of nodes")
# plt.show()
plt.savefig("outputs/Branching_nodes.png")