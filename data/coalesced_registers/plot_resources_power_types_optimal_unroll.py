#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import pandas as pd
import os
from ert_plot import *
from math import atan2, degrees, log2
from matplotlib.ticker import MultipleLocator

device='pac_a10'

df = pd.read_csv("resources.csv", names=["kernel_type","buffers","queue_type","unroll","ert_flop","dtype","dsp_used","dsp_max","lut_used","lut_max"])

fig = plt.figure()
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Power Efficiency By Data Type: Optimal DRAM Transfer Width")
plt.suptitle("Arria 10 Power Efficiency By Data Type", size='xx-large')
plt.title("Optimal DRAM Transfer Width")

f1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 16, 'float', 'ro-')
d1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 8, 'double', 'bo-')
i1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 16, 'int', 'go-')
l1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 8, 'long', 'co-')
sc1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 64, 'signed_char', 'mo-')

ax.yaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax.set_ylabel('Watts')
ax.set_xlabel('% Hardware Utilization')
labelLines(plt.gca().get_lines(), xvals=(17.5,17.5,17.5,17.5,17.5,17.5), zorder=2.5)
#ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float (unroll=16)','double (unroll=8)','int32 (unroll=16)','int64 (unroll=8)','int8 (unroll=64)'], loc='upper left', title='Data Types')
ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float (unroll=16)','double (unroll=8)','int32 (unroll=16)','int64 (unroll=8)','int8 (unroll=64)'], loc='lower right', title='Data Types')
plt.xlim(0,100)
plt.ylim(0,32)
plt.savefig('roofline_resources_power_types_optimal_unroll.png', dpi=500)
