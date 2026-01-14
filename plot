#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import sys
import os

colors = ["blue", "green", "red", 'gray', 'cyan', 'orange', 'purple']


files = os.listdir('.data')
fig,ax = plt.subplots()
i = 0
for f in files:
	print('.data/' + f)

	cols=()

	sheet = pd.read_csv(".data/" + f, delimiter='\t')
	cols = sheet.columns
	data = []
	for col in cols:
		data.append(sheet[col])

	#fig.tight_layout()
	ax.set_xlabel("time", loc='center')
	ax.set_ylabel("Data")
	#ax.set_title("")
	if len(data[0]) == 0:
		continue

	plt.plot(data[0], data[1], label=f, color=colors[i])
	i = i + 1

plt.legend()
plt.savefig('plot.png', format='png')


