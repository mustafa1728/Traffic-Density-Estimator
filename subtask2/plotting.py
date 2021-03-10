import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv("out.csv")

df.head()

df['Time'] = 382 * df['Frame Number']/ df['Frame Number'].max()
df.head()

plt.ylim(-0.05, 0.85)
plt.plot(df['Time'], df.iloc[:,1], label='Queue Density')
plt.plot(df['Time'], df.iloc[:,2], label='Moving Density')
plt.legend()

plt.xlabel("Time (in seconds)")
plt.title("COP290 Task 1 Traffic Density")
plt.ylabel('Density')

plt.axvline(x=99, color='g', linestyle='--')
plt.axvline(x=183, color='r', linestyle='--')
plt.axvline(x=287, color='g', linestyle='--')
plt.axvline(x=359, color='r', linestyle='--')

figure = plt.gcf()

figure.set_size_inches(12, 6)
plt.savefig("graph.png", dpi=300)
plt.close()

df2 = pd.read_csv("out_frame_wise.csv", header=None)
df2.head()

plt.ylim(-0.05, 0.85)
plt.plot(df['Frame Number'], df.iloc[:,1], label='Queue Density')
plt.plot(df['Frame Number'], df.iloc[:,2], label='Moving Density')
plt.legend()

plt.xlabel("Time (in seconds)")
plt.title("COP290 Task 1 Traffic Density")
plt.ylabel('Density')


figure = plt.gcf()

figure.set_size_inches(12, 6)
plt.savefig("graph_framewise.png", dpi=300)

