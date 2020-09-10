#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from ert_plot import *

device='pac_a10'

fig = plt.figure()
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Roofline By Data Type: Optimal DRAM Transfer Width")
plt.suptitle("Arria 10 Roofline By Data Type", size='xx-large')
plt.title("Optimal DRAM Transfer Width")

whatx = ErtLog.ai
whaty = ErtLog.max_gflops

f1 = plot_ert(device, 'swi', 1, 'coalesced', 16, 'float', 'ro--', whatx=whatx, whaty=whaty)
d1 = plot_ert(device, 'swi', 1, 'coalesced', 8, 'double', 'bo--', whatx=whatx, whaty=whaty)
i1 = plot_ert(device, 'swi', 1, 'coalesced', 16, 'int', 'go--', whatx=whatx, whaty=whaty)
l1 = plot_ert(device, 'swi', 1, 'coalesced', 8, 'long', 'co--', whatx=whatx, whaty=whaty)
sc1 = plot_ert(device, 'swi', 1, 'coalesced', 64, 'signed_char', 'mo--', whatx=whatx, whaty=whaty)

plt.locator_params(axis='x', numticks=12)
ax.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.set_ylabel('GOp/s')
ax.set_xlabel('AI')
ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float (unroll=16)','double (unroll=8)','int32 (unroll=16)','int64 (unroll=8)','int8 (unroll=64)'], loc='lower right', title='Data Types')
plt.savefig('roofline_types_unroll.png', dpi=500)
