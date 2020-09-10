#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from math import atan2, degrees, log2
from matplotlib.ticker import MultipleLocator
from ert_plot import *

device='pac_a10'

fig = plt.figure()
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Power Roofline: Floating Point Unrolling")
plt.suptitle("Arria 10 Power Efficiency", size='xx-large')
plt.title("Floating Point Unrolling")
whatx = ErtLog.max_power
whaty = ErtLog.max_bandwidth
u1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'float', 'ro-', whatx=whatx, whaty=whaty)
u2 = plot_ert(device, 'swi', 1, 'coalesced', 2, 'float', 'bo-', whatx=whatx, whaty=whaty)
u4 = plot_ert(device, 'swi', 1, 'coalesced', 4, 'float', 'go-', whatx=whatx, whaty=whaty)
u8 = plot_ert(device, 'swi', 1, 'coalesced', 8, 'float', 'co-', whatx=whatx, whaty=whaty)
u16 = plot_ert(device, 'swi', 1, 'coalesced', 16, 'float', 'mo-', whatx=whatx, whaty=whaty)
u32 = plot_ert(device, 'swi', 1, 'coalesced', 32, 'float', 'yo-', whatx=whatx, whaty=whaty)

power_line(1.0)
power_line(1.0/2)
power_line(1.0/4)
power_line(1.0/8)

#plt.locator_params(axis='x', numticks=12)
#ax.xaxis.set_ticks(np.arange(20,32,2))
#ax.set_xscale('log')
ax.set_xticks([16,20,24,28,32])
#ax.yaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
#ax.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
#ax.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
locmin = matplotlib.ticker.LogLocator(base=2.0,numticks=12)
ax.yaxis.set_minor_locator(locmin)
ax.yaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
ax.set_ylabel('GB/s')
ax.set_xlabel('Watts')
labelLines(plt.gca().get_lines(), xvals=(17.5,17.5,17.5,17.5,17.5,17.5), zorder=2.5, angle=8)
#ax.legend([u1[0],u2[0],u4[0],u8[0],u16[0],u32[0]],['1','2','4','8','16','32'], loc='lower right', title='Unrolls')
#ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float (unroll=16)','double (unroll=8)','int32 (unroll=16)','int64 (unroll=8)','int8 (unroll=64)'], loc='lower right', title='Data Types')
ax.legend([u1[0],u2[0],u4[0],u8[0],u16[0],u32[0]],['1','2','4','8','16','32'], loc='lower right', title='Unrolls')
plt.xlim(16,32)
plt.ylim(1,34)
plt.savefig('roofline_1_power_gb.png', dpi=500)
