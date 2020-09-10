#!/usr/bin/env python3

import os
import glob, re
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import pandas as pd
import size_format
from math import atan2, degrees, log2

dtype_sizes={"signed_char":1, "float": 4, "int": 4, "double": 8, "long": 8}

#Label line with line2D label data
def labelLine(line,x,label=None,angle=None,align=True,**kwargs):

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
        dy = dy**2
        ang = log2(degrees(atan2(dy,dx)))

        #Transform to screen co-ordinates
        pt = np.array([x,x]).reshape((1,2))
        trans_angle = ax.transData.transform_angles(np.array((ang,)),pt)[0]

    else:
        trans_angle = 0

    if angle is not None:
        trans_angle = angle

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

def labelLines(lines,align=True,xvals=None,angle=None,**kwargs):

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
        labelLine(line,x,label,angle,align,**kwargs)

class ErtTrial:
    def __init__(self, dtype, ert_flops, nsize, iterations, ms, bytes_read, ops, bandwidth, gflops):
        self.m_nsize = int(nsize)
        self.m_iterations = int(iterations)
        self.m_microseconds = float(ms)
        self.m_bytes = self.m_nsize*self.m_iterations*2
        self.m_ops = self.m_nsize/dtype_sizes[dtype]*self.m_iterations*ert_flops
        self.m_bandwidth = self.m_bytes/self.m_microseconds/1e3
        self.m_gflops = self.m_ops/self.m_microseconds/1e3
        self.m_power = []
    def nsize(self):
        return self.m_nsize
    def iterations(self):
        return self.m_iterations
    def microseconds(self):
        return self.m_microseconds
    def bytes(self):
        return self.m_bytes
    def ops(self):
        return self.m_ops
    def bandwidth(self):
        return self.m_bandwidth
    def gflops(self):
        return self.m_gflops
    def power(self):
        return self.m_power
    def __str__(self):
        return "<ErtTrial nsize: %d, iterations: %d, microseconds: %f, bytes: %d, ops: %d, GB/s: %f, GF/s: %f>" % (self.m_nsize, self.m_iterations, self.m_microseconds, self.m_bytes, self.m_ops, self.m_bandwidth, self.m_gflops)
    def add_power_datapoint(self, p):
        self.m_power.append(p)
    def max_power(self):
        m = -1.0
        for p in self.m_power:
            if p > m:
                m = p
        return m

class ErtLog:
    def __init__(self, filepath, powerpath=None):
        self.m_trials = []
        m = re.search(r'(?P<device>[A-Za-z0-9_]+)_(?P<kernel_type>\w+)_(?P<buffers>[A-Za-z0-9]+)_(?P<queue_type>\w+)_(?P<unroll>[0-9]+)_(?P<ert_flops>[0-9]+)_(?P<dtype>\w+)\.log', filepath)
        self.m_device = m.group('device')
        self.m_kernel_type = m.group('kernel_type')
        if m.group('buffers').isdigit():
            self.m_buffers = int(m.group('buffers'))
        else:
            self.m_buffers = m.group('buffers')
        self.m_queue_type = m.group('queue_type')
        self.m_unroll = int(m.group('unroll'))
        self.m_ert_flops = int(m.group('ert_flops'))
        self.m_dtype = m.group('dtype')
        with open(filepath) as f:
            f.readline()
            for line in f:
                s = line.split()
                t = ErtTrial(self.m_dtype,self.m_ert_flops,s[5],s[6],s[7],s[8],s[9],s[10],s[11])
                self.m_trials.append(t)
        if powerpath is not None:
            with open(powerpath) as f:
                for line in f:
                    s = line.split()
                    trialnum = int(s[0])
                    power = float(s[3])
                    self.m_trials[trialnum].add_power_datapoint(power)
    def trials(self):
        return self.m_trials
    def device(self):
        return self.m_device
    def kernel_type(self):
        return self.m_kernel_type
    def buffers(self):
        return self.m_buffers
    def queue_type(self):
        return self.m_queue_type
    def unroll(self):
        return self.unroll
    def ert_flops(self):
        return self.m_ert_flops
    def ai(self):
        dtype_bytes = dtype_sizes[self.m_dtype]
        return self.m_ert_flops/float(dtype_bytes)
    def dtype(self):
        return self.m_dtype
    def max_bandwidth(self):
        m=-1.0
        for t in self.m_trials:
            if t.bandwidth() > m:
                m = t.bandwidth()
        return m
    def max_gflops(self):
        m=-1.0
        for t in self.m_trials:
            if t.gflops() > m:
                m = t.gflops()
        return m
    def add_power_trial(self, trialnum, power):
        self.m_trials[trialnum].add_power_datapoint(power)
    def max_power(self):
        m=-1.0
        for t in self.m_trials:
            mp = t.max_power()
            if mp > m:
                m = mp
        return m

def plot_res_power(df, device, kernel_type, buffers, queue_type, unroll, dtype, linetype):
    data_x=[]
    data_y=[]
    ff = df[df.kernel_type.eq(kernel_type) & df.buffers.eq(buffers) & df.queue_type.eq(queue_type) & df.unroll.eq(unroll) & df.dtype.eq(dtype)].sort_values(by=['ert_flop'])
    for row in ff.itertuples(index=True):
        ert_flops = row.ert_flop
        powerpath = 'logs/%s_%s_%s_%s_%d_%s_%s.power.log' % (device, kernel_type, buffers, queue_type, unroll, ert_flops, dtype)
        filepath = 'logs/%s_%s_%s_%s_%d_%s_%s.log' % (device, kernel_type, buffers, queue_type, unroll, ert_flops, dtype)
        if not os.path.exists(powerpath) or not os.path.exists(filepath):
            continue
        el = ErtLog(filepath, powerpath)

        res = 100*float(row.dsp_used)/float(row.dsp_max)
        res2 = 100*float(row.lut_used)/float(row.lut_max)
        if res2 > res:
            res = res2
        data_x.append(res)
        data_y.append(el.max_power())
    return plt.plot(data_x, data_y, linetype)

def plot_ert(device, kernel_type, buffers, queue_type, unroll, dtype, linetype, whatx, whaty, ax=None):
    files={}
    for filepath in glob.iglob(r'logs/%s_%s_%s_%s_%d_*_%s.log' % (device, kernel_type, buffers, queue_type, unroll, dtype)):
        m = re.search(r'%s_%s_%s_%s_%d_(?P<ert_flops>[0-9]+)_(?P<dtype>\w+)\.log' % (device, kernel_type, buffers, queue_type, unroll), filepath)
        ert_flops = int(m.group('ert_flops'))
        powerpath = 'logs/%s_%s_%s_%s_%d_%s_%s.power.log' % (device, kernel_type, buffers, queue_type, unroll, ert_flops, dtype)
        files[ert_flops] = (filepath, powerpath)
    data_x=[]
    data_y=[]
    for ert_flops, (filepath, powerpath) in sorted(files.items()):
        el = ErtLog(filepath,powerpath)
        data_x.append(whatx(el))
        data_y.append(whaty(el))
    if ax is not None:
        return ax.loglog(data_x, data_y, linetype, basex=2, basey=2)
    else:
        return plt.loglog(data_x, data_y, linetype, basex=2, basey=2)

def plot_trials(device, kernel_type, buffers, queue_type, unroll, ert_flop, dtype, linetype, whatx, whaty, graphtype=plt.plot, **kwargs):
    filepath = 'logs/%s_%s_%s_%s_%d_%s_%s.log' % (device, kernel_type, buffers, queue_type, unroll, ert_flop, dtype)
    powerpath = 'logs/%s_%s_%s_%s_%d_%s_%s.power.log' % (device, kernel_type, buffers, queue_type, unroll, ert_flop, dtype)
    el = ErtLog(filepath,powerpath)
    data_x=[]
    data_y=[]
    for t in el.trials():
        data_x.append(whatx(t))
        data_y.append(whaty(t))
    return graphtype(data_x, data_y, linetype, **kwargs)

def fractions(x,pos):
    if np.log2(x) >= 0:
        return '{:.0f}'.format(x)
    else:
        return '1/{:.0f}'.format(1/x)

def size_format_scale_iec_bytes(x,pos):
    return size_format.size_format(x, units=size_format.iec_bytes)


def power_line(power):
    x = np.linspace(1,512,100)
    y = power*x+power
    #plt.annotate('%s GFLOP/Joule' % power, xy=(17,power*16), xycoords='data')
    powerl = fractions(power,0)
    ret = plt.loglog(x,y,'--', basex=2, basey=2, color='0.5', label="%s GFLOP/Joule" % powerl)
    return ret
