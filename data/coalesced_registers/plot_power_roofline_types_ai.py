#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from ert_plot import *

import configparser

config = configparser.ConfigParser()
config.read("settings.ini")

resource_roofline = config['DEFAULT'].getboolean('resource_roofline', True)
draw_title = config['DEFAULT'].getboolean('title', True)
device = config['DEFAULT'].get('device','pac_a10')

fig = plt.figure()
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Power Roofline: Floating Point Unrolling")
if draw_title:
    plt.suptitle("Arria 10 Power Roofline", size='xx-large')
    plt.title("Data Type Efficiency Increasing AI")

whatx = ErtLog.max_power
whaty = ErtLog.max_gflops

f1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'float', 'ro--', whatx=whatx, whaty=whaty)
d1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'double', 'bo--', whatx=whatx, whaty=whaty)
i1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'int', 'go--', whatx=whatx, whaty=whaty)
l1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'long', 'co--', whatx=whatx, whaty=whaty)
sc1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'signed_char', 'mo--', whatx=whatx, whaty=whaty)

fstring="%s GOp/Joule"
power_line(1/4, fstring)
power_line(1/2, fstring)
power_line(1, fstring)
power_line(2, fstring)
power_line(4, fstring)
power_line(8, fstring)
power_line(16, fstring)
power_line(32, fstring)
power_line(64, fstring)

#ax.set_xticks([16,20,24,28,32])
#ax.yaxis.set_ticks(np.arange(20,32,2))
#ax.yaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
#ax.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
#ax.set_xscale('linear')
ax.set_xlabel('Watts')
ax.set_ylabel('GOp/s')
labeloffset=2
labelLines(plt.gca().get_lines(), xvals=(labeloffset,labeloffset,labeloffset,labeloffset,labeloffset,labeloffset,labeloffset,labeloffset,labeloffset), zorder=2.5, angle=23)
ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float','double','int32','int64','int8'], loc='lower right', title='Data types')
plt.xlim(1,256)
plt.ylim(1/8.0,2048)
if not draw_title:
    plt.tight_layout()
plt.savefig('roofline_power_types_ai.png', dpi=500)
