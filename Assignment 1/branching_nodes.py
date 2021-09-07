import matplotlib.pyplot as plt
x=[3,5,10,30,50,100]
r=[0.342,0.199,0.106,0.038,0.0214,0.0127]
br=[1/i for i in r]
print(br)
plt.plot(x,br)
plt.xscale("log")
plt.title("Branching (1/r) with number of nodes")
plt.ylabel("Branching")
plt.xlabel("Number of nodes")
# plt.show()
plt.savefig("outputs/Branching_nodes.png")