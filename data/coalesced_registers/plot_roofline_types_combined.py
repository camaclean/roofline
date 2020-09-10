#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from ert_plot import *

device='pac_a10'

fig, (ax1, ax2) = plt.subplots(2,1, constrained_layout=False, figsize=(6.4,8.8))
#plt.title("Arria 10 Roofline By Data Type: Single Element DRAM Transfers")
plt.suptitle("Arria 10 Roofline By Data Type", size='xx-large')
ax1.set_title("a) Single Element DRAM Transfers")

whatx = ErtLog.ai
whaty = ErtLog.max_gflops

f1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'float', 'ro--', whatx=whatx, whaty=whaty, ax=ax1)
d1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'double', 'bo--', whatx=whatx, whaty=whaty, ax=ax1)
i1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'int', 'go--', whatx=whatx, whaty=whaty, ax=ax1)
l1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'long', 'co--', whatx=whatx, whaty=whaty, ax=ax1)
sc1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'signed_char', 'mo--', whatx=whatx, whaty=whaty, ax=ax1)

plt.locator_params(axis='x', numticks=12)
ax1.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax1.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax1.set_ylabel('GOp/s')
ax1.set_xlabel('AI')
#ax.legend([u1[0],u2[0],u4[0],u8[0],u16[0],u32[0]],['1','2','4','8','16','32'], loc='lower right', title='Unrolls')
ax1.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float','double','int32','int64','int8'], loc='lower right', title='Data types')
#ax1.set_aspect('equal')



ax2.set_title("b) Optimal DRAM Transfer Width")

f16 = plot_ert(device, 'swi', 1, 'coalesced', 16, 'float', 'ro--', whatx=whatx, whaty=whaty, ax=ax2)
d8 = plot_ert(device, 'swi', 1, 'coalesced', 8, 'double', 'bo--', whatx=whatx, whaty=whaty, ax=ax2)
i16 = plot_ert(device, 'swi', 1, 'coalesced', 16, 'int', 'go--', whatx=whatx, whaty=whaty, ax=ax2)
l8 = plot_ert(device, 'swi', 1, 'coalesced', 8, 'long', 'co--', whatx=whatx, whaty=whaty, ax=ax2)
sc64 = plot_ert(device, 'swi', 1, 'coalesced', 64, 'signed_char', 'mo--', whatx=whatx, whaty=whaty, ax=ax2)

ax2.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax2.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax2.set_ylabel('GOp/s')
ax2.set_xlabel('AI')
ax2.legend([f16[0],d8[0],i16[0],l8[0],sc64[0]],['float (unroll=16)','double (unroll=8)','int32 (unroll=16)','int64 (unroll=8)','int8 (unroll=64)'], loc='lower right', title='Data Types')

plt.subplots_adjust(hspace=0.24,left=0.1,right=0.95,top=0.9,bottom=0.06, wspace=0)
#fig.set_constrained_layout_pads(hspace=0.30)

plt.savefig('roofline_types_combined.png', dpi=500)
