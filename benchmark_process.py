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
        x.append(int(row['nProcesses']))
        y1.append(float(row['clock_realtime']))
        y2.append(float(row['clock_cputime']))

    plt.title("Process benchmark [ms]", fontdict={'fontsize':"medium"} )
    plt.xticks(x,x)
    plt.plot(x, y1, marker="o", label="real_time")
    plt.plot(x, y2, marker="o", label="cpu_time")
    plt.legend(loc="upper left")
    plt.savefig(os.path.splitext(file_name)[0]+'.png')
    
