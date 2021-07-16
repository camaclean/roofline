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

figsize=(8,5)
fig = plt.figure(figsize=figsize)
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Roofline: Floating Point Unrolling")
if draw_title:
    plt.suptitle("Arria 10 Roofline", size='xx-large')
    plt.title("Floating Point Unrolling")

fmax=386*1000*1000
dsps=1518
bandwidth=34.133
gflops_max=fmax*dsps*2/(1000.0*1000.0*1000.0)
intercept=(gflops_max-bandwidth)/bandwidth
x = np.linspace(intercept+2,dsps/4.0,dsps,dtype=float)
y = 0*x+gflops_max
#y = float(power)*(x-1)+float(power)
#plt.annotate('%s GFLOP/Joule' % power, xy=(17,power*16), xycoords='data')
#powerl = fractions(power,0)
x2 = np.linspace(1/8.0,intercept,100,dtype=float)
y2 = float(bandwidth)*(x2-1)+bandwidth #+34

max1 = plt.plot(x,y,'--', color='0.5', linewidth=1) #label=fstring % powerl)
max2 = plt.plot(x2,y2,'--', color='0.5', linewidth=1) #label=fstring % powerl)

def roofline(x):
    if x < intercept:
        return float(bandwidth)*(x-1)+bandwidth
    else:
        return gflops_max

x3 = np.linspace(1/8.0,dsps/4.0,30000,dtype=float)
np_roofline = np.vectorize(roofline)
y3 = np_roofline(x3)
#max3 = plt.plot(x3,y3,'-', color='0.0', linewidth=4) #label=fstring % powerl)

if resource_roofline:
    unrolls=[1,2,4,8,16,32,64,128,256,512]
    plot_ert_resource_roofline(device, 'swi', 1, 'coalesced', unrolls, 'float', whaty=ErtLog.max_gflops, color='0.0', linewidth=3, zorder=30)

whatx = ErtLog.ai
whaty = ErtLog.max_gflops
u1 = plot_ert(device, 'swi', 1, 'coalesced', 1, 'float', 'ro--', whatx=whatx, whaty=whaty)
u2 = plot_ert(device, 'swi', 1, 'coalesced', 2, 'float', 'bo--', whatx=whatx, whaty=whaty)
u4 = plot_ert(device, 'swi', 1, 'coalesced', 4, 'float', 'go--', whatx=whatx, whaty=whaty)
u8 = plot_ert(device, 'swi', 1, 'coalesced', 8, 'float', 'co--', whatx=whatx, whaty=whaty)
u16 = plot_ert(device, 'swi', 1, 'coalesced', 16, 'float', 'mo--', whatx=whatx, whaty=whaty)
u32 = plot_ert(device, 'swi', 1, 'coalesced', 32, 'float', 'yo--', whatx=whatx, whaty=whaty)
u64 = plot_ert(device, 'swi', 1, 'coalesced', 64, 'float', 'o--', whatx=whatx, whaty=whaty)
u128 = plot_ert(device, 'swi', 1, 'coalesced', 128, 'float', 'o--', whatx=whatx, whaty=whaty)
u256 = plot_ert(device, 'swi', 1, 'coalesced', 256, 'float', 'o--', whatx=whatx, whaty=whaty)
u512 = plot_ert(device, 'swi', 1, 'coalesced', 512, 'float', 'o--', whatx=whatx, whaty=whaty)

plt.locator_params(axis='x', numticks=12)
ax.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(fractions))
ax.set_ylabel('GF/s')
ax.set_xlabel('AI')
ax.legend([u1[0],u2[0],u4[0],u8[0],u16[0],u32[0],u64[0],u128[0],u256[0],u512[0]],['1','2','4','8','16','32','64','128','256','512'], loc='lower right', title='Unrolls')
if not draw_title:
    plt.tight_layout()
plt.savefig('roofline_1_theoretical_real_max.png', dpi=500)
