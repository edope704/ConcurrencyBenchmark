import numpy as np
import matplotlib.pyplot as plt
import csv
import os, sys


file_name = sys.argv[1]
if file_name is None:
    print("usage:", sys.argv[0], " filename.csv")
    exit(1)

with open(file_name, newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    x = []
    y1 = []
    y2 = []
    for row in reader:
        x.append(int(row['nThreads']))
        y1.append(float(row['clock_realtime']))
        y2.append(float(row['clock_cputime']))

    fig, (real_time, cpu_time) = plt.subplots(2, sharex=True)
    fig.suptitle("Threads benchmark [us]", fontsize="medium")

    real_time.set_title("real_time")
    real_time.plot(x, y1, marker="o")
    real_time.set_xticks(x)

    cpu_time.set_title("cpu_time")
    cpu_time.plot(x, y2, marker="o")
    cpu_time.set_xticks(x)

    fig.tight_layout()
    fig.savefig(os.path.splitext(file_name)[0]+'.png')
    