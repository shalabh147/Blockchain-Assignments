import matplotlib.pyplot as plt
x=[0.1,0.3,0.5,0.7,2,3,4,8,15,30,50]
y=[173,75,26,25,17,10,8,5,3,2,2]
plt.plot(x,y)
plt.xscale("log")
plt.title("Height of Tree with Mean block mining time")
plt.ylabel("Height of Tree")
plt.xlabel("T_k")
# plt.show()
plt.savefig("outputs/Height_tk.png")