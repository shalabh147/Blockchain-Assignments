import matplotlib.pyplot as plt
x=[]
y=[]
channel_values = open("hash_power.txt").read().split()
fl=0;
for i in channel_values:
	if(fl==1):
		x.append(float(i))
	else :
		y.append(float(i))
	fl=fl+1
	fl=fl%2
# x= [round(i,3) for i in x]
print(x)
print(y)
plt.plot(x,y)
plt.xlabel("Percent hashing power")
plt.ylabel("Ratio")
plt.title("Effect of hashing Power")
# plt.show()
plt.savefig("outputs/Hashing_power.png")