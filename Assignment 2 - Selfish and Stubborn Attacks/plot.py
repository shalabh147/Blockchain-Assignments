import matplotlib.pyplot as plt

x = [25, 50, 75]
y1 = [0.7, 0.733, 0.788]
y2 = [0.409, 0.645, 0.769]

plt.plot(x, y1, marker='o', label="Stubborn Mining")
plt.plot(x, y2, marker='o', label="Selfish Mining")

plt.xlabel("zeta")
plt.ylabel("MPU_attacker")
plt.title("Variation of MPU_attacker w.r.t zeta")
plt.legend()
# plt.show()
plt.savefig("plots/mpu attacker zeta.png")
