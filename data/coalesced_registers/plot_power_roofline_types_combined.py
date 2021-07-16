#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from ert_plot import *

device='pac_a10'

fig, (ax1, ax2) = plt.subplots(2,1, constrained_layout=False, figsize=(6.4,8.8))
#plt.title("Arria 10 Power Roofline By Data Type: Single Element DRAM Transfers")
plt.suptitle("Arria 10 Power Roofline By Data Type", size='xx-large')
ax1.set_title("a) Single Element DRAM Transfers")
plt.locator_params(axis='x', numticks=12)

whatx = ErtLog.ert_flops
whaty = ErtLog.max_power
f1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'float', 'ro--', whatx=whatx, whaty=whaty, ax=ax1)
d1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'double', 'bo--', whatx=whatx, whaty=whaty, ax=ax1)
i1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'int', 'go--', whatx=whatx, whaty=whaty, ax=ax1)
l1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'long', 'co--', whatx=whatx, whaty=whaty, ax=ax1)
sc1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'signed_char', 'mo--', whatx=whatx, whaty=whaty, ax=ax1)

ax1.yaxis.set_ticks(np.arange(20,32,2))
ax1.yaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax1.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax1.set_ylabel('Watts')
ax1.set_xlabel('ERT Ops')
ax1.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float','double','int32','int64','int8'], loc='upper left', title='Data types')


ax2.set_title("b) Optimal DRAM Transfer Width")

f16 = plot_ert(device, 'swi', 1, 'coalesced', 16, 'float', 'ro--', whatx=whatx, whaty=whaty)
d8 = plot_ert(device, 'swi', 1, 'coalesced', 8, 'double', 'bo--', whatx=whatx, whaty=whaty)
i16 = plot_ert(device, 'swi', 1, 'coalesced', 16, 'int', 'go--', whatx=whatx, whaty=whaty)
l8 = plot_ert(device, 'swi', 1, 'coalesced', 8, 'long', 'co--', whatx=whatx, whaty=whaty)
sc64 = plot_ert(device, 'swi', 1, 'coalesced', 64, 'signed_char', 'mo--', whatx=whatx, whaty=whaty)

ax2.yaxis.set_ticks(np.arange(20,32,2))
ax2.yaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax2.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax2.set_ylabel('Watts')
ax2.set_xlabel('ERT Ops')
ax2.legend([f16[0],d8[0],i16[0],l8[0],sc64[0]],['float (unroll=16)','double (unroll=8)','int32 (unroll=16)','int64 (unroll=8)','int8 (unroll=64)'], loc='upper left', title='Data Types')

plt.subplots_adjust(hspace=0.24,left=0.1,right=0.95,top=0.9,bottom=0.06, wspace=0)

plt.savefig('roofline_power_types_combined.png', dpi=500)
