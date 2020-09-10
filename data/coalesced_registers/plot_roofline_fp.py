#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from ert_plot import *

device='pac_a10'

fig = plt.figure()
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Roofline: Floating Point Unrolling")
plt.suptitle("Arria 10 Roofline", size='xx-large')
plt.title("Floating Point Unrolling")
whatx = ErtLog.ai
whaty = ErtLog.max_gflops
u1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'float', 'ro--', whatx=whatx, whaty=whaty)
u2 = plot_ert(device, 'swi', 1, 'coalesced', 2, 'float', 'bo--', whatx=whatx, whaty=whaty)
u4 = plot_ert(device, 'swi', 1, 'coalesced', 4, 'float', 'go--', whatx=whatx, whaty=whaty)
u8 = plot_ert(device, 'swi', 1, 'coalesced', 8, 'float', 'co--', whatx=whatx, whaty=whaty)
u16 = plot_ert(device, 'swi', 1, 'coalesced', 16, 'float', 'mo--', whatx=whatx, whaty=whaty)
u32 = plot_ert(device, 'swi', 1, 'coalesced', 32, 'float', 'yo--', whatx=whatx, whaty=whaty)
plt.locator_params(axis='x', numticks=12)
ax.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.set_ylabel('GF/s')
ax.set_xlabel('AI')
ax.legend([u1[0],u2[0],u4[0],u8[0],u16[0],u32[0]],['1','2','4','8','16','32'], loc='lower right', title='Unrolls')
plt.savefig('roofline_1.png', dpi=500)
