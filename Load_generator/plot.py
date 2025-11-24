import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("mixed.csv")

plt.figure()
plt.plot(df["vus"], df["tps"])
plt.xlabel("VUS")
plt.ylabel("Throughput (TPS)")
plt.title("Throughput vs Load")
plt.grid(True)
plt.savefig("throughput_vs_load_mixed.png", dpi=200)
plt.close()

plt.figure()
plt.plot(df["vus"], df["avg"])
plt.xlabel("VUS")
plt.ylabel("Avg Latency")
plt.title("Latency vs Load")
plt.grid(True)
plt.savefig("latency_vs_load_mixed.png", dpi=200)
plt.close()