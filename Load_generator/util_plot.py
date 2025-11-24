import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("Get.csv")

# CPU Utilization Plot
plt.figure()
plt.plot(df["vus"], df["core1_cpu_max"])
plt.xlabel("VUS (Load)")
plt.ylabel("Core-1 CPU Utilization (%)")
plt.title("CPU Utilization vs Load")
plt.grid(True)
plt.savefig("cpu_util_vs_load_get.png", dpi=200)
plt.close()

# Disk Utilization Plot
plt.figure()
plt.plot(df["vus"], df["disk_util_max"])
plt.xlabel("VUS (Load)")
plt.ylabel("Disk Utilization (%)")
plt.title("Disk Utilization vs Load")
plt.grid(True)
plt.savefig("disk_util_vs_load_get.png", dpi=200)
plt.close()