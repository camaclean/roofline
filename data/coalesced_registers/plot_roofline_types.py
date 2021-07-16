#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from ert_plot import *
import configparser

device='pac_a10'

config = configparser.ConfigParser()
config.read("settings.ini")

resource_roofline = config['DEFAULT'].getboolean('resource_roofline', True)
draw_title = config['DEFAULT'].getboolean('title', True)


figsize=(8,4)
fig = plt.figure(figsize=figsize)
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Roofline By Data Type: Single Element DRAM Transfers")
if draw_title:
    plt.suptitle("Arria 10 Roofline By Data Type", size='xx-large')
    plt.title("Single Element DRAM Transfers")

whatx = ErtLog.ai
whaty = ErtLog.max_gflops

f1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'float', 'ro--', whatx=whatx, whaty=whaty)
d1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'double', 'bo--', whatx=whatx, whaty=whaty)
i1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'int', 'go--', whatx=whatx, whaty=whaty)
l1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'long', 'co--', whatx=whatx, whaty=whaty)
sc1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'signed_char', 'mo--', whatx=whatx, whaty=whaty)

def plot_ert_rooflines(ax):
    unrolls=[1,2,4,8,16,32,64,128,256,512]
    plot_ert_resource_roofline(device, 'swi', 1, 'coalesced', unrolls, 'float', whaty=ErtLog.max_gflops, ax=ax, linetype='r-', linewidth=1, zorder=30)
    plot_ert_resource_roofline(device, 'swi', 1, 'coalesced', unrolls, 'double', whaty=ErtLog.max_gflops, ax=ax, linetype='b-', linewidth=1, zorder=30)
    plot_ert_resource_roofline(device, 'swi', 1, 'coalesced', unrolls, 'int', whaty=ErtLog.max_gflops, ax=ax, linetype='g-', linewidth=1, zorder=30)
    plot_ert_resource_roofline(device, 'swi', 1, 'coalesced', unrolls, 'long', whaty=ErtLog.max_gflops, ax=ax, linetype='c-', linewidth=1, zorder=30)
    plot_ert_resource_roofline(device, 'swi', 1, 'coalesced', unrolls, 'signed_char', whaty=ErtLog.max_gflops, ax=ax, linetype='m-', linewidth=1, zorder=30)

if resource_roofline:
    plot_ert_rooflines(ax)

plt.locator_params(axis='x', numticks=12)
ax.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.set_ylabel('GOp/s')
ax.set_xlabel('AI')
#ax.legend([u1[0],u2[0],u4[0],u8[0],u16[0],u32[0]],['1','2','4','8','16','32'], loc='lower right', title='Unrolls')
ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float','double','int32','int64','int8'], loc='lower right', title='Data types')
if not draw_title:
    plt.tight_layout()
plt.savefig('roofline_types.png', dpi=500)
