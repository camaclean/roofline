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

fig, (ax1, ax2) = plt.subplots(2,1, constrained_layout=False, figsize=(6.4,8.8))
#plt.title("Arria 10 Power Efficiency: Floating Point Unrolling")
plt.suptitle("Arria 10 Power Efficiency By Data Type", size='xx-large')
ax1.set_title("a) Single Element DRAM Transfers")

f1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'float', 'ro-', ax=ax1)
d1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'double', 'bo-', ax=ax1)
i1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'int', 'go-', ax=ax1)
l1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'long', 'co-', ax=ax1)
sc1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'signed_char', 'mo-', ax=ax1)

#power_line(1)
#power_line(2)
#power_line(4)
#power_line(8)
#power_line(16)
#power_line(32)

ax1.yaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax1.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax1.set_ylabel('Watts')
ax1.set_xlabel('% Hardware Utilization')
#ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float','double','int32','int64','int8'], loc='upper left', title='Data types')
ax1.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float','double','int32','int64','int8'], loc='lower right', title='Data types')


ax1.set_xlim(0,100)
ax1.set_ylim(0,32)


ax2.set_title("b) Optimal DRAM Transfer Width")

f16 = plot_res_power(df, device, 'swi', 1, 'coalesced', 16, 'float', 'ro-', ax=ax2)
d8 = plot_res_power(df, device, 'swi', 1, 'coalesced', 8, 'double', 'bo-', ax=ax2)
i16 = plot_res_power(df, device, 'swi', 1, 'coalesced', 16, 'int', 'go-', ax=ax2)
l8 = plot_res_power(df, device, 'swi', 1, 'coalesced', 8, 'long', 'co-', ax=ax2)
sc64 = plot_res_power(df, device, 'swi', 1, 'coalesced', 64, 'signed_char', 'mo-', ax=ax2)

ax2.yaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax2.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax2.set_ylabel('Watts')
ax2.set_xlabel('% Hardware Utilization')
#ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float (unroll=16)','double (unroll=8)','int32 (unroll=16)','int64 (unroll=8)','int8 (unroll=64)'], loc='upper left', title='Data Types')
ax2.legend([f16[0],d8[0],i16[0],l8[0],sc64[0]],['float (unroll=16)','double (unroll=8)','int32 (unroll=16)','int64 (unroll=8)','int8 (unroll=64)'], loc='lower right', title='Data Types')

ax2.set_xlim(0,100)
ax2.set_ylim(0,32)
#plt.xlim(0,100)
#plt.ylim(0,32)

plt.subplots_adjust(hspace=0.24,left=0.1,right=0.95,top=0.9,bottom=0.06, wspace=0)
plt.savefig('roofline_resources_power_types_combined.png', dpi=500)
