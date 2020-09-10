#!/usr/bin/env python3

import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import pandas as pd
import os
from math import atan2, degrees, log2
from matplotlib.ticker import MultipleLocator

device='pac_a10'

dtype_sizes={"signed_char":1, "float": 4, "int": 4, "double": 8, "long": 8}

#Label line with line2D label data
def labelLine(line,x,label=None,align=True,**kwargs):

    ax = line.axes
    xdata = line.get_xdata()
    ydata = line.get_ydata()

    if (x < xdata[0]) or (x > xdata[-1]):
        print('x label location is outside data range! %s %s %s' % (x, xdata[0], xdata[-1]))
        return

    #Find corresponding y co-ordinate and angle of the line
    ip = 1
    for i in range(len(xdata)):
        if x < xdata[i]:
            ip = i
            break

    y = ydata[ip-1] + (ydata[ip]-ydata[ip-1])*(x-xdata[ip-1])/(xdata[ip]-xdata[ip-1])

    if not label:
        label = line.get_label()

    if align:
        #Compute the slope
        dx = xdata[ip] - xdata[ip-1]
        dy = ydata[ip] - ydata[ip-1]
        print("dx, dy: %d %d" % (dx, dy))
        dy = dy**2
        print("dx, dy: %d %d" % (dx, dy))
        ang = log2(degrees(atan2(dy,dx)))
        print(ang)

        #Transform to screen co-ordinates
        pt = np.array([x,x]).reshape((1,2))
        print(pt)
        trans_angle = ax.transData.transform_angles(np.array((ang,)),pt)[0]
        print(trans_angle)

    else:
        trans_angle = 0

    #Set a bunch of keyword arguments
    if 'color' not in kwargs:
        kwargs['color'] = line.get_color()

    if ('horizontalalignment' not in kwargs) and ('ha' not in kwargs):
        kwargs['ha'] = 'center'

    if ('verticalalignment' not in kwargs) and ('va' not in kwargs):
        kwargs['va'] = 'center'

    if 'backgroundcolor' not in kwargs:
        kwargs['backgroundcolor'] = ax.get_facecolor()

    if 'clip_on' not in kwargs:
        kwargs['clip_on'] = True

    if 'zorder' not in kwargs:
        kwargs['zorder'] = 2.5

    ax.text(x,y,label,rotation=trans_angle,**kwargs)

def labelLines(lines,align=True,xvals=None,**kwargs):

    ax = lines[0].axes
    labLines = []
    labels = []

    #Take only the lines which have labels other than the default ones
    for line in lines:
        label = line.get_label()
        if "_line" not in label:
            labLines.append(line)
            labels.append(label)

    if xvals is None:
        xmin,xmax = ax.get_xlim()
        xvals = np.linspace(xmin,xmax,len(labLines)+2)[1:-1]

    for line,x,label in zip(labLines,xvals,labels):
        labelLine(line,x,label,align,**kwargs)

class ErtTrial:
    def __init__(self, dtype, ert_flops, nsize, iterations, ms, bytes_read, ops, bandwidth, gflops):
        self.nsize = int(nsize)
        self.iterations = int(iterations)
        self.microseconds = float(ms)
        self.bytes = self.nsize*self.iterations*2
        self.ops = self.nsize/dtype_sizes[dtype]*self.iterations*ert_flops
        self.bandwidth = self.bytes/self.microseconds/1e3
        self.gflops = self.ops/self.microseconds/1e3
        self.power = []
        #print(self.bandwidth)
    def __str__(self):
        return "<ErtTrial nsize: %d, iterations: %d, microseconds: %f, bytes: %d, ops: %d, GB/s: %f, GF/s: %f>" % (self.nsize, self.iterations, self.microseconds, self.bytes, self.ops, self.bandwidth, self.gflops)
    def add_power_datapoint(self, p):
        self.power.append(p)
    def max_power(self):
        m = -1.0
        for p in self.power:
            if p > m:
                m = p
        return m

class ErtLog:
    def __init__(self, filepath):
        self.trials = []
        m = re.search(r'(?P<device>[A-Za-z0-9_]+)_(?P<kernel_type>\w+)_(?P<buffers>[A-Za-z0-9]+)_(?P<queue_type>\w+)_(?P<unroll>[0-9]+)_(?P<ert_flops>[0-9]+)_(?P<dtype>\w+)\.log', filepath)
        self.device = m.group('device')
        self.kernel_type = m.group('kernel_type')
        if m.group('buffers').isdigit():
            self.buffers = int(m.group('buffers'))
        else:
            self.buffers = m.group('buffers')
        self.queue_type = m.group('queue_type')
        self.unroll = int(m.group('unroll'))
        self.ert_flops = int(m.group('ert_flops'))
        self.dtype = m.group('dtype')
        with open(filepath) as f:
            f.readline()
            for line in f:
                s = line.split()
                t = ErtTrial(self.dtype,self.ert_flops,s[5],s[6],s[7],s[8],s[9],s[10],s[11])
                self.trials.append(t)
    def max_bandwidth(self):
        m=-1.0
        for t in self.trials:
            if t.bandwidth > m:
                m = t.bandwidth
        return m
    def max_gflops(self):
        m=-1.0
        for t in self.trials:
            if t.gflops > m:
                m = t.gflops
        return m
    def add_power_trial(self, trialnum, power):
        self.trials[trialnum].add_power_datapoint(power)
    def max_power(self):
        m=-1.0
        for t in self.trials:
            mp = t.max_power()
            if mp > m:
                m = mp
        return m


# swi_2_coalesced_8_1_float.log
def plot(df, device, kernel_type, buffers, queue_type, unroll, dtype, linetype):
    data_x=[]
    data_y=[]
    ff = df[df.kernel_type.eq(kernel_type) & df.buffers.eq(buffers) & df.queue_type.eq(queue_type) & df.unroll.eq(unroll) & df.dtype.eq(dtype)].sort_values(by=['ert_flop'])
    for row in ff.itertuples(index=True):
        print(row)
        ert_flops = row.ert_flop
        powerpath = 'logs/%s_%s_%s_%s_%d_%s_%s.power.log' % (device, kernel_type, buffers, queue_type, unroll, ert_flops, dtype)
        filepath = 'logs/%s_%s_%s_%s_%d_%s_%s.log' % (device, kernel_type, buffers, queue_type, unroll, ert_flops, dtype)
        if not os.path.exists(powerpath) or not os.path.exists(filepath):
            continue
        el = ErtLog(filepath)
        with open(powerpath) as f:
            for line in f:
                s = line.split()
                trialnum = int(s[0])
                power = float(s[3])
                el.add_power_trial(trialnum, power)

        print(powerpath)
        res = 100*float(row.dsp_used)/float(row.dsp_max)
        if res == 0:
            res = 100*float(row.lut_used)/float(row.lut_max)
        data_x.append(res)
        data_y.append(el.max_power())
    return plt.plot(data_x, data_y, linetype)

def fractions(x,pos):
    print(x, np.log2(x))
    if np.log2(x) >= 0:
        return '{:.0f}'.format(x)
    else:
        return '1/{:.0f}'.format(1/x)

def power_line(power):
    x = np.linspace(1,512,100)
    y = power*x+power
    #plt.annotate('%s GFLOP/Joule' % power, xy=(17,power*16), xycoords='data')
    ret = plt.loglog(x,y,'--', basex=2, basey=2, color='0.5', label="%s GFLOP/Joule" % power)
    return ret

df = pd.read_csv("resources.csv", names=["kernel_type","buffers","queue_type","unroll","ert_flop","dtype","dsp_used","dsp_max","lut_used","lut_max"])
print(df)

fig = plt.figure()
ax = fig.add_subplot(1,1,1)
#plt.title("Arria 10 Power Efficiency: Floating Point Unrolling")
plt.suptitle("Arria 10 Power Efficiency By Data Type", size='xx-large')
plt.title("Single Element DRAM Transfers")

f1 = plot(df, device, 'swi', 1, 'coalesced', 1, 'float', 'ro-')
d1 = plot(df, device, 'swi', 1, 'coalesced', 1, 'double', 'bo-')
i1 = plot(df, device, 'swi', 1, 'coalesced', 1, 'int', 'go-')
l1 = plot(df, device, 'swi', 1, 'coalesced', 1, 'long', 'co-')
sc1 = plot(df, device, 'swi', 1, 'coalesced', 1, 'signed_char', 'mo-')

#power_line(1)
#power_line(2)
#power_line(4)
#power_line(8)
#power_line(16)
#power_line(32)

ax.yaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax.set_ylabel('Watts')
ax.set_xlabel('% Hardware Utilization')
labelLines(plt.gca().get_lines(), xvals=(17.5,17.5,17.5,17.5,17.5,17.5), zorder=2.5)
ax.legend([f1[0],d1[0],i1[0],l1[0],sc1[0]],['float','double','int32','int64','int8'], loc='lower right', title='Data types')
plt.xlim(0,100)
#plt.ylim(0.25,2048)
plt.savefig('roofline_resources_power_types.png', dpi=500)
