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
plt.suptitle("Arria 10 ERT Spatial Locality", size='xx-large')
plt.title("Float32")
whatx = ErtTrial.nsize
whaty = ErtTrial.bandwidth
ert_flop=1
bufs=2
u1 = plot_trials(device, 'swi', bufs, 'coalesced', 1, ert_flop, 'float', 'ro--', whatx=whatx, whaty=whaty)
u2 = plot_trials(device, 'swi', bufs, 'coalesced', 2, ert_flop, 'float', 'bo--', whatx=whatx, whaty=whaty)
u4 = plot_trials(device, 'swi', bufs, 'coalesced', 4, ert_flop, 'float', 'go--', whatx=whatx, whaty=whaty)
u8 = plot_trials(device, 'swi', bufs, 'coalesced', 8, ert_flop, 'float', 'co--', whatx=whatx, whaty=whaty)
u16 = plot_trials(device, 'swi', bufs, 'coalesced', 16, ert_flop, 'float', 'mo--', whatx=whatx, whaty=whaty)
u32 = plot_trials(device, 'swi', bufs, 'coalesced', 32, ert_flop, 'float', 'yo--', whatx=whatx, whaty=whaty)
u64 = plot_trials(device, 'swi', bufs, 'coalesced', 64, ert_flop, 'float', 'o--', whatx=whatx, whaty=whaty)
u128 = plot_trials(device, 'swi', bufs, 'coalesced', 128, ert_flop, 'float', 'o--', whatx=whatx, whaty=whaty)
ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
plt.locator_params(axis='x', numticks=12)
ax.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(size_format_scale_iec_bytes))
#ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.set_ylabel('GB/s')
ax.set_xlabel('Spatial Locality')
plt.xticks(rotation=-45,ha='left')
ax.legend([u1[0],u2[0],u4[0],u8[0],u16[0],u32[0],u64[0],u128[0]],['1','2','4','8','16','32','64','128'], loc='upper left', title='Unrolls')
fig.subplots_adjust(bottom=0.18)
plt.savefig('fp_bandwidth.png', dpi=500)
