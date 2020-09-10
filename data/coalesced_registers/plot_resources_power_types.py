#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import pandas as pd
import os
from math import atan2, degrees, log2
from matplotlib.ticker import MultipleLocator
from ert_plot import *

device='pac_a10'

df = pd.read_csv("resources.csv", names=["kernel_type","buffers","queue_type","unroll","ert_flop","dtype","dsp_used","dsp_max","lut_used","lut_max"])

fig = plt.figure()
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Power Efficiency: Floating Point Unrolling")
plt.suptitle("Arria 10 Power Efficiency By Data Type", size='xx-large')
plt.title("Single Element DRAM Transfers")

f1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'float', 'ro-')
d1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'double', 'bo-')
i1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'int', 'go-')
l1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'long', 'co-')
sc1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'signed_char', 'mo-')

#power_line(1)
#power_line(2)
#power_line(4)
#power_line(8)
#power_line(16)
#power_line(32)

ax.yaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax.set_ylabel('Watts')
ax.set_xlabel('% Hardware Utilization')
labelLines(plt.gca().get_lines(), xvals=(17.5,17.5,17.5,17.5,17.5,17.5), zorder=2.5)
#ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float','double','int32','int64','int8'], loc='upper left', title='Data types')
ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float','double','int32','int64','int8'], loc='lower right', title='Data types')
plt.xlim(0,100)
plt.ylim(0,32)
plt.savefig('roofline_resources_power_types.png', dpi=500)
