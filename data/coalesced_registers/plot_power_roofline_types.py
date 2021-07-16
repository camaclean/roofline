#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from ert_plot import *

device='pac_a10'

fig = plt.figure()
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Power Roofline By Data Type: Single Element DRAM Transfers")
plt.suptitle("Arria 10 Power Roofline By Data Type", size='xx-large')
plt.title("Single Element DRAM Transfers")

whatx = ErtLog.ert_flops
whaty = ErtLog.max_power
f1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'float', 'ro--', whatx=whatx, whaty=whaty)
d1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'double', 'bo--', whatx=whatx, whaty=whaty)
i1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'int', 'go--', whatx=whatx, whaty=whaty)
l1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'long', 'co--', whatx=whatx, whaty=whaty)
sc1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'signed_char', 'mo--', whatx=whatx, whaty=whaty)

ax.yaxis.set_ticks(np.arange(20,32,2))
ax.yaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax.set_ylabel('Watts')
ax.set_xlabel('ERT Ops')
ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float','double','int32','int64','int8'], loc='upper left', title='Data types')
plt.savefig('roofline_power_types.png', dpi=500)
