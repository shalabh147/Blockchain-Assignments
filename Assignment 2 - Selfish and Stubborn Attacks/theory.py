import matplotlib.pyplot as plt
hash_pow=[1,10,20,30,40,50]
ratio_1=[]
ratio_0=[]
for i in hash_pow:
	a=i/100
	ratio_1.append(((a*((1-a)**2)*(2*a+1))-(a**3))/(1-a*(1+(2-a)*a)))
for i in hash_pow:
	a=i/100
	ratio_0.append(((a*((1-a)**2)*(4*a))-(a**3))/(1-a*(1+(2-a)*a)))
empirical_stub=[0,0.133,0.353,0.357,0.5,0.438]
empirical_selfish=[0,0.167,0.333,0.444,0.6,0.6]
print(ratio_1)
plt.plot(hash_pow,ratio_1,label='Theoretical with gamma=1')
plt.plot(hash_pow,ratio_0,label='Theoretical with gamma=0')
plt.plot(hash_pow,empirical_stub,label='Stubborn empirical')
plt.plot(hash_pow,empirical_selfish,label='Selfish empirical')
plt.title("Fraction of attacker nodes in main chain")
plt.ylabel("Fraction")
plt.xlabel("Hashing Power (%)")
plt.legend()
# plt.show()
plt.savefig("fraction_comparison.png")