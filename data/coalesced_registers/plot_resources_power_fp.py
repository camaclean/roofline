#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import pandas as pd
from math import atan2, degrees, log2
from matplotlib.ticker import MultipleLocator
from ert_plot import *

device='pac_a10'

dtype_sizes={"signed_char":1, "float": 4, "int": 4, "double": 8, "long": 8}

df = pd.read_csv("resources.csv", names=["kernel_type","buffers","queue_type","unroll","ert_flop","dtype","dsp_used","dsp_max","lut_used","lut_max"])

fig = plt.figure()
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Power Efficiency: Floating Point Unrolling")
plt.suptitle("Arria 10 Power Efficiency", size='xx-large')
plt.title("Float32 Unrolling")
u1 = plot_res_power(df, device, 'swi', 1, 'coalesced', 1, 'float', 'ro-')
u2 = plot_res_power(df, device, 'swi', 1, 'coalesced', 2, 'float', 'bo-')
u4 = plot_res_power(df, device, 'swi', 1, 'coalesced', 4, 'float', 'go-')
u8 = plot_res_power(df, device, 'swi', 1, 'coalesced', 8, 'float', 'co-')
u16 = plot_res_power(df, device, 'swi', 1, 'coalesced', 16, 'float', 'mo-')
u32 = plot_res_power(df, device, 'swi', 1, 'coalesced', 32, 'float', 'yo-')

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
#ax.legend([u1[0],u2[0],u4[0],u8[0],u16[0],u32[0]],['1','2','4','8','16','32'], loc='lower right', title='Unrolls')
#ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float (unroll=16)','double (unroll=8)','int32 (unroll=16)','int64 (unroll=8)','int8 (unroll=64)'], loc='lower right', title='Data Types')
ax.legend([u1[0],u2[0],u4[0],u8[0],u16[0],u32[0]],['1','2','4','8','16','32'], loc='lower right', title='Unrolls')
plt.xlim(0,100)
plt.ylim(0,32)
plt.savefig('roofline_resources_power_fp.png', dpi=500)
