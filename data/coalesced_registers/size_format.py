
iec=[(1024**4, ' Ti'),
     (1024**3, ' Gi'),
     (1024**2, ' Mi'),
     (1024**1, ' Ki'),
     (1024**0, ' ')]

iec_bytes=[(1024**4, 'TiB'),
     (1024**3, ' GiB'),
     (1024**2, ' MiB'),
     (1024**1, ' KiB'),
     (1024**0, ' B')]

iec_bandwidth=[(1024**4, 'TiB/s'),
     (1024**3, ' GiB/s'),
     (1024**2, ' MiB/s'),
     (1024**1, ' KiB/s'),
     (1024**0, ' B/s')]

si= [(1000**4, ' T'),
     (1000**3, ' G'),
     (1000**2, ' M'),
     (1000**1, ' k'),
     (1000**0, '')]

si_bytes= [(1000**4, 'TB'),
     (1000**3, ' GB'),
     (1000**2, ' MB'),
     (1000**1, ' kB'),
     (1000**0, ' TB')]

si_bandwidth=[(1000**4, 'TB/s'),
     (1000**3, ' GB/s'),
     (1000**2, ' MB/s'),
     (1000**1, ' kB/s'),
     (1000**0, ' B/s')]

def size_format(x, units=iec):
    for factor, suffix in units:
        if x >= factor:
            break
    v = x/factor
    return '{:.0f}{:s}'.format(v,suffix)
