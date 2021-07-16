#!/bin/bash

set -eu

source submit.sh

submit 1 "signed char" 1 1
submit 2 "signed char" 1 1
submit 1 "signed char" 1 2
submit 2 "signed char" 1 2
submit 1 "signed char" 1 4
submit 2 "signed char" 1 4
submit 1 "signed char" 1 8
submit 2 "signed char" 1 8
submit 1 "signed char" 1 16
submit 2 "signed char" 1 16
submit 1 "signed char" 1 32
submit 2 "signed char" 1 32
submit 1 "signed char" 1 64
submit 2 "signed char" 1 64
submit 1 "signed char" 1 128
submit 2 "signed char" 1 128
submit 1 "signed char" 1 256
submit 2 "signed char" 1 256
submit 1 "signed char" 1 512
submit 2 "signed char" 1 512
submit 1 "signed char" 1 1024
submit 2 "signed char" 1 1024
submit 1 "signed char" 1 2048
submit 2 "signed char" 1 2048
submit 1 "signed char" 1 $((2048+256))
submit 1 "signed char" 1 $((2048+512))
submit 1 "signed char" 1 $((2048+512+256))
submit 1 "signed char" 1 $((2048+512+256+128))
submit 1 "signed char" 1 $((2048+512+256+128+64))
submit 1 "signed char" 1 $((2048+512+256+128+64+16))
submit 1 "signed char" 1 $((2048+512+256+128+64+16+4))
submit 1 "signed char" 1 $((2048+512+256+128+64+16+8))
submit 1 "signed char" 1 $((2048+512+256+128+64+16+8+4))
submit 1 "signed char" 1 $((2048+512+256+128+64+16+8+4+1))
submit 1 "signed char" 1 $((2048+512+256+128+64+16+8+4+2))
submit 1 "signed char" 1 $((2048+512+256+128+64+16+8+4+2+1))
submit 1 "signed char" 1 $((2048+512+256+128+64+32))
#submit 1 "signed char" 1 $((2048+512+256+128+64+32+16))
# submit 1 "signed char" 1 $((2048+1024)) # too big
submit 1 "signed char" 2 1
submit 2 "signed char" 2 1
submit 1 "signed char" 2 2
submit 2 "signed char" 2 2
submit 1 "signed char" 2 4
submit 2 "signed char" 2 4
submit 1 "signed char" 2 8
submit 2 "signed char" 2 8
submit 1 "signed char" 2 16
submit 2 "signed char" 2 16
submit 1 "signed char" 2 32
submit 2 "signed char" 2 32
submit 1 "signed char" 2 64
submit 2 "signed char" 2 64
submit 1 "signed char" 2 128
submit 2 "signed char" 2 128
submit 1 "signed char" 2 256
submit 2 "signed char" 2 256
submit 1 "signed char" 2 512
submit 2 "signed char" 2 512
submit 1 "signed char" 2 1024
submit 1 "signed char" 4 1
submit 2 "signed char" 4 1
submit 1 "signed char" 4 2
submit 2 "signed char" 4 2
submit 1 "signed char" 4 4
submit 2 "signed char" 4 4
submit 1 "signed char" 4 8
submit 2 "signed char" 4 8
submit 1 "signed char" 4 16
submit 2 "signed char" 4 16
submit 1 "signed char" 4 32
submit 2 "signed char" 4 32
submit 1 "signed char" 4 64
submit 2 "signed char" 4 64
submit 1 "signed char" 4 128
submit 2 "signed char" 4 128
submit 1 "signed char" 4 256
submit 2 "signed char" 4 256
submit 1 "signed char" 4 512
submit 1 "signed char" 8 1
submit 2 "signed char" 8 1
submit 1 "signed char" 8 2
submit 2 "signed char" 8 2
submit 1 "signed char" 8 4
submit 2 "signed char" 8 4
submit 1 "signed char" 8 8
submit 2 "signed char" 8 8
submit 1 "signed char" 8 16
submit 2 "signed char" 8 16
submit 1 "signed char" 8 32
submit 2 "signed char" 8 32
submit 1 "signed char" 8 64
submit 2 "signed char" 8 64
submit 1 "signed char" 8 128
submit 2 "signed char" 8 128
submit 1 "signed char" 8 256
submit 1 "signed char" 16 1
submit 2 "signed char" 16 1
submit 1 "signed char" 16 2
submit 2 "signed char" 16 2
submit 1 "signed char" 16 4
submit 2 "signed char" 16 4
submit 1 "signed char" 16 8
submit 2 "signed char" 16 8
submit 1 "signed char" 16 16
submit 2 "signed char" 16 16
submit 1 "signed char" 16 32
submit 2 "signed char" 16 32
submit 1 "signed char" 16 64
submit 2 "signed char" 16 64
submit 1 "signed char" 16 128
submit 1 "signed char" 32 1
submit 2 "signed char" 32 1
submit 1 "signed char" 32 2
submit 2 "signed char" 32 2
submit 1 "signed char" 32 4
submit 2 "signed char" 32 4
submit 1 "signed char" 32 8
submit 2 "signed char" 32 8
submit 1 "signed char" 32 16
submit 2 "signed char" 32 16
submit 1 "signed char" 32 32
submit 2 "signed char" 32 32
submit 1 "signed char" 32 64
submit 1 "signed char" 64 1
submit 2 "signed char" 64 1
submit 1 "signed char" 64 2
submit 2 "signed char" 64 2
submit 1 "signed char" 64 4
submit 2 "signed char" 64 4
submit 1 "signed char" 64 8
submit 2 "signed char" 64 8
submit 1 "signed char" 64 16
submit 2 "signed char" 64 16
submit 1 "signed char" 64 32
submit 1 "signed char" 128 1
submit 2 "signed char" 128 1
submit 1 "signed char" 128 2
submit 2 "signed char" 128 2
submit 1 "signed char" 128 4
submit 2 "signed char" 128 4
submit 1 "signed char" 128 8
submit 2 "signed char" 128 8
submit 1 "signed char" 128 16
submit 1 "signed char" 256 1
submit 2 "signed char" 256 1
submit 1 "signed char" 256 2
submit 2 "signed char" 256 2
submit 1 "signed char" 256 4
submit 2 "signed char" 256 4
submit 1 "signed char" 256 8
submit 1 "signed char" 512 1
submit 2 "signed char" 512 1
submit 1 "signed char" 512 2
submit 2 "signed char" 512 2
submit 1 "signed char" 512 4
submit 1 "signed char" 1024 1
submit 2 "signed char" 1024 1
submit 1 "signed char" 1024 2
submit 2 "signed char" 1024 2
submit 1 "signed char" 2048 1

submit 1 float 1   1    # y
submit 1 float 1   2    # y
submit 1 float 1   4    # y
submit 1 float 1   8    # y
submit 1 float 1   16    # y
submit 1 float 1   32    # y
submit 1 float 1   64    # y
submit 1 float 1   128    # y
submit 1 float 1   256    # y
submit 1 float 1   512    # y
submit 1 float 1   1024    # y
submit 1 float 1   2048    # y
submit 2 float 1   1    # y
submit 2 float 1   2    # y
submit 2 float 1   4    # y
submit 2 float 1   8    # y
submit 2 float 1   16   # y
submit 2 float 1   32   # y
submit 2 float 1   64   # y
submit 2 float 1   128  # y
submit 2 float 1   256  # y
submit 2 float 1   512  # y
submit 2 float 1   1024 # y
submit 2 float 1   1515 # y
#submit 2 float 1   1516 # too big
submit 1 float 1   2048 # y
submit 1 float 1   3030 # y
submit 1 float 1   3031 # y
submit 1 float 1   3032 # y
submit 1 float 1   3033 # y
#submit 1 float 1 3034  # too big
submit 1 float 2   1    # y
submit 1 float 2   2    # y
submit 1 float 2   4    # y
submit 1 float 2   8    # y
submit 1 float 2   16    # y
submit 1 float 2   32    # y
submit 1 float 2   64    # y
submit 1 float 2   128    # y
submit 1 float 2   256    # y
submit 1 float 2   512    # y
submit 1 float 2   1024    # y
submit 2 float 2   1    # y
submit 2 float 2   2    # y
submit 2 float 2   4    # y
submit 2 float 2   8    # y
submit 2 float 2   16   # y
submit 2 float 2   32   # y
submit 2 float 2   64   # y
submit 2 float 2   128  # y
submit 2 float 2   256  # y
submit 2 float 2   512  # y
submit 2 float 2   757  # y
#submit 2 float 2   758 # too big
submit 1 float 2   1024 # y
submit 1 float 2   1515 # y
#submit 1 float 2   1516 # too big
submit 1 float 3   1    # y
submit 1 float 3   505  # y
submit 1 float 3   1009 # y
#submit 1 float 3   1010 # too big
submit 2 float 3   1    # y
submit 2 float 3   503  # y
#submit 2 float 3   504  # too big
submit 1 float 4   1    # y
submit 1 float 4   2    # y
submit 1 float 4   4    # y
submit 1 float 4   8    # y
submit 1 float 4   16    # y
submit 1 float 4   32    # y
submit 1 float 4   64    # y
submit 1 float 4   128    # y
submit 1 float 4   256    # y
submit 1 float 4   512    # y
submit 2 float 4   1    # y
submit 2 float 4   2    # y
submit 2 float 4   4    # y
submit 2 float 4   8    # y
submit 2 float 4   16   # y
submit 2 float 4   32   # y
submit 2 float 4   64   # y
submit 2 float 4   128  # y
submit 2 float 4   256  # y
submit 2 float 4   377  # y
#submit 2 float 4   378  # too big
#submit 2 float 4   379  # too big
submit 1 float 4   512  # y
submit 1 float 4   757  # y
#submit 1 float 4   758  # too big
submit 1 float 5   1    # y
submit 1 float 5   603  # y
submit 1 float 5   604  # y
submit 1 float 5   605  # y
#submit 1 float 5   606  # too big
submit 2 float 5   1    # y
submit 2 float 5   300  # y
submit 2 float 5   301  # y
#submit 2 float 5   302  # too big
submit 1 float 6   1    # y
submit 1 float 6   503  # y
#submit 1 float 6   504  # too big
submit 2 float 6   1    # y
submit 2 float 6   250  # y
submit 2 float 6   251  # y
#submit 2 float 6   252  # too big
submit 1 float 7   1    # y
submit 1 float 7   431  # y
#submit 1 float 7   432  # too big
submit 2 float 7   1    # y
submit 2 float 7   215  # y
#submit 2 float 7   216  # too big
submit 1 float 8   1    # y
submit 1 float 8   2    # y
submit 1 float 8   4    # y
submit 1 float 8   8    # y
submit 1 float 8   16    # y
submit 1 float 8   32    # y
submit 1 float 8   64    # y
submit 1 float 8   128    # y
submit 2 float 8   1    # y
submit 2 float 8   2    # y
submit 2 float 8   4    # y
submit 2 float 8   8    # y
submit 2 float 8   16   # y
submit 2 float 8   32   # y
submit 2 float 8   64   # y
submit 2 float 8   128  # y
submit 2 float 8   186  # y
submit 2 float 8   187  # y
#submit 2 float 8   188 # too big
submit 1 float 8   256  # y
submit 1 float 8   377  # y
#submit 1 float 8   378 # too big
submit 1 float 12  1    # y
submit 1 float 16  1    # y
submit 1 float 16  2    # y
submit 1 float 16  4    # y
submit 1 float 16  8    # y
submit 1 float 16  16    # y
submit 1 float 16  32    # y
submit 1 float 16  64    # y
submit 2 float 16  1    # y
submit 2 float 16  2    # y
submit 2 float 16  4    # y
submit 2 float 16  8    # y
submit 2 float 16  16   # y
submit 2 float 16  32   # y
submit 2 float 16  64   # y
submit 2 float 16  92   # y
submit 2 float 16  93   # y
#submit 2 float 16  94   # too big
submit 1 float 16  128  # y
submit 1 float 16  186  # y
submit 1 float 16  187  # y
#submit 1 float 16  188  # too big
submit 1 float 24  1    # y
submit 1 float 32  2    # y
submit 1 float 32  4    # y
submit 1 float 32  8    # y
submit 1 float 32  16    # y
submit 1 float 32  32    # y
submit 2 float 32  1    # y
submit 2 float 32  2    # y
submit 2 float 32  4    # y
submit 2 float 32  8    # y
submit 2 float 32  16   # y
submit 2 float 32  32   # y
submit 2 float 32  45   # y
#submit 2 float 32  46  # too big
submit 1 float 32  64   # y
submit 1 float 32  92   # y
submit 1 float 32  93   # y
#submit 1 float 32  94   # too big
submit 1 float 48  1    # y
submit 1 float 64  1    # y
submit 2 float 64  1    # y
submit 2 float 64  2    # y
submit 2 float 64  4    # y
submit 2 float 64  8    # y
submit 2 float 64  16   # y
submit 2 float 64  21   # y
#submit 2 float 64  22   # too big
submit 1 float 64  32   # y
submit 1 float 64  45   # y
#submit 1 float 64  46  # too big
submit 1 float 96  1    # y
submit 1 float 128 1    # y
submit 2 float 128 1    # y
submit 2 float 128 2    # y
submit 2 float 128 4    # y
submit 2 float 128 8    # y
submit 2 float 128 9    # y
#submit 2 float 128 10  #too big
submit 1 float 128 16   # y
submit 1 float 128 21   # y
#submit 1 float 128 22  # too big
submit 2 float 192 1    # y
submit 2 float 256 1    # y
submit 2 float 256 2    # y
submit 2 float 256 3    # y
#submit 2 float 256 4   # too big
submit 1 float 256 1    # y
submit 1 float 256 2    # y
submit 1 float 256 4    # y
submit 1 float 256 5    # y
submit 1 float 256 6    # y
#submit 1 float 256 7    # too big
submit 2 float 512 1    # y
#submit 2 float 512 2    # too big
submit 1 float 512 1    # y
submit 1 float 512 2    # y
submit 1 float 512 3    # y
#submit 1 float 512 4   # too big
#submit 2 float 1024 1   # too big
submit 1 float 1024 1   # y
#submit 1 float 1024 2   # too big

submit 1 double 1   1    # y
submit 1 double 1   2    # y
submit 1 double 1   4    # y
submit 1 double 1   8    # y
submit 1 double 1   16    # y
submit 1 double 1   32    # y
submit 1 double 1   64    # y
submit 1 double 1   128    # y
submit 1 double 1   256    # y
submit 1 double 1   314  # y
submit 1 double 1   315  # y
#submit 1 double 1   316  # y assert fails
submit 1 double 1   317  # y
#submit 1 double 1   318  # too big
submit 2 double 1   1    # y
submit 2 double 1   2    # y
submit 2 double 1   4    # y
submit 2 double 1   8    # y
submit 2 double 1   16   # y
submit 2 double 1   32   # y
submit 2 double 1   64   # y
submit 2 double 1   79   # y
submit 2 double 1   128  # y
submit 2 double 1   150  # y
submit 2 double 1   155  # y
submit 2 double 1   157  # y
submit 2 double 1   158  # y
submit 2 double 1   159  # y
#submit 2 double 1   160  # too big
submit 2 double 2   1    # y
submit 2 double 2   2    # y
submit 2 double 2   4    # y
submit 2 double 2   8    # y
submit 2 double 2   16   # y
submit 2 double 2   32   # y
submit 2 double 2   39   # y
submit 2 double 2   64   # y
submit 2 double 2   75   # y
submit 2 double 2   82   # y
submit 2 double 2   83   # y
#submit 2 double 2   84   # too big
submit 1 double 2   159  # y
submit 1 double 2   160  # y
submit 1 double 2   161  # y
submit 1 double 2   162  # y
#submit 1 double 2   163  # too big
submit 2 double 4   1    # y
submit 2 double 4   2    # y
submit 2 double 4   4    # y
submit 2 double 4   8    # y
submit 2 double 4   16   # y
submit 2 double 4   19   # y
submit 2 double 4   32   # y
submit 2 double 4   36   # y
submit 2 double 4   38   # y
submit 2 double 4   39   # y
#submit 2 double 4   40   # too big
submit 1 double 4   83   # y
#submit 1 double 4   84   # too big
submit 2 double 8   1    # y
submit 2 double 8   2    # y
submit 2 double 8   4    # y
submit 2 double 8   8    # y
submit 2 double 8   10   # y
submit 2 double 8   12   # y
submit 2 double 8   14   # y
submit 2 double 8   15   # y
#submit 2 double 8   16   # too big
submit 1 double 8   1    # y
submit 1 double 8   2
submit 1 double 8   4
submit 1 double 8   8
submit 1 double 8   16
submit 1 double 8   32
submit 1 double 8   38   # y
#submit 1 double 8   39   # too big
submit 2 double 16  1    # y
submit 2 double 16  2    # y
submit 2 double 16  4    # y
submit 2 double 16  7    # y
submit 2 double 16  8    # y
submit 2 double 16  9    # y
#submit 2 double 16  10   # too big
submit 1 double 16  15   # y
submit 1 double 16  16   # y
#submit 1 double 16  17   # too big
submit 2 double 32  1    # y
submit 2 double 32  2    # y
submit 2 double 32  3    # y
submit 2 double 32  4    # y
submit 2 double 32  5    # y
#submit 2 double 32  6    # too big
submit 1 double 32  9    # y
#submit 1 double 32  10   # to big
submit 2 double 64  1    # y
#submit 2 double 64  2    # too big
submit 1 double 64  3    # y
#submit 1 double 64  4    # too big
#submit 1 double 64  5    # too big
submit 2 double 128 1    # y
#submit 2 double 128 2    # too big
submit 1 double 128 1    # y
#submit 1 double 128 2    # too big
submit 1 double 256 1    # y
#submit 1 double 256 2    # too big

submit 1 int 1 1    # y
submit 1 int 1 2    # y
submit 1 int 1 4    # y
submit 1 int 1 8    # y
submit 1 int 1 16   # y
submit 1 int 1 32   # y
submit 1 int 1 64   # y
submit 1 int 1 128  # y
submit 1 int 1 256  # y
submit 1 int 1 512  # y
#submit 1 int 1 1024 # too big
#submit 1 int 1 1515 # too big
submit 2 int 1 1    # y
submit 2 int 1 2    # y
submit 2 int 1 4    # y
submit 2 int 1 8    # y
submit 2 int 1 16   # y
submit 2 int 1 32   # y
submit 2 int 1 64   # y
submit 2 int 1 128  # y
submit 2 int 1 256  # y
submit 2 int 1 500  # y
submit 2 int 1 505  # y
#submit 2 int 1 506  # too big
submit 1 int 1 1010 # y
#submit 1 int 1 1011 # assert failure
#submit 1 int 1 1012 # too big
submit 1 int 2 1    # y
submit 2 int 2 1    # y
submit 2 int 2 2    # y
submit 2 int 2 4    # y
submit 2 int 2 8    # y
submit 2 int 2 16   # y
submit 2 int 2 32   # y
submit 2 int 2 64   # y
submit 2 int 2 128  # y
submit 2 int 2 252  # y
submit 2 int 2 253  # y
#submit 2 int 2 254  # too big
submit 1 int 2 505  # y
#submit 1 int 2 506  # too big
submit 1 int 4 1    # y
submit 2 int 4 1    # y
submit 2 int 4 2    # y
submit 2 int 4 4    # y
submit 2 int 4 8    # y
submit 2 int 4 16   # y
submit 2 int 4 32   # y
submit 2 int 4 64   # y
submit 2 int 4 125  # y
#submit 2 int 4 126  # too big
submit 1 int 4 253  # y
#submit 1 int 4 254  # too big
submit 1 int 8 1    # y
submit 2 int 8 1    # y
submit 2 int 8 2    # y
submit 2 int 8 4    # y
submit 2 int 8 8    # y
submit 2 int 8 16   # y
submit 2 int 8 32   # y
submit 2 int 8 62   # y
submit 2 int 8 63   # y
#submit 2 int 8 64   # too big
submit 1 int 8 125  # y
#submit 1 int 8 126  # too big
submit 1 int 16 1   # y
submit 2 int 16 1   # y
submit 2 int 16 2   # y
submit 2 int 16 4   # y
submit 2 int 16 8   # y
submit 2 int 16 16  # y
submit 2 int 16 30  # y
submit 2 int 16 31  # y
#submit 2 int 16 32  # too big
submit 1 int 16 2
submit 1 int 16 4
submit 1 int 16 8
submit 1 int 16 16
submit 1 int 16 32
submit 1 int 16 63  # y
#submit 1 int 16 64 # too big
submit 1 int 32 1   # y
submit 2 int 32 1   # y
submit 2 int 32 2   # y
submit 2 int 32 4   # y
submit 2 int 32 8   # y
submit 2 int 32 15  # y
#submit 2 int 32 16  # too big
submit 1 int 32 31  # y
#submit 1 int 32 32  # too big
submit 1 int 64 1   # y
submit 2 int 64 1   # y
submit 2 int 64 2   # y
submit 2 int 64 4   # y
submit 2 int 64 7   # y
#submit 2 int 64 8   # too big
submit 1 int 64 15  # y
#submit 1 int 64 26  # too big
submit 1 int 128 1  # y
submit 1 int 128 7  # y
#submit 1 int 128 8  # too big
submit 2 int 128 1  # y
submit 2 int 128 2  # y
submit 2 int 128 3  # y
#submit 2 int 128 4  # too big
submit 1 int 256 1  # y
#submit 1 int 256 2  # too big
submit 2 int 256 1  # y
#submit 2 int 256 2  # too big
submit 1 int 512 1  # y

submit 1 long 1 1   # y
submit 1 long 1 2   # y
submit 1 long 1 4   # y
submit 1 long 1 8   # y
submit 1 long 1 16   # y
submit 1 long 1 32   # y
submit 1 long 1 64   # y
submit 1 long 1 128   # y
submit 1 long 1 216 # y
submit 1 long 1 220 # y
submit 1 long 1 256 # y
submit 1 long 1 236 # y
submit 1 long 1 420 # y
submit 1 long 1 425 # y
submit 1 long 1 426 # y
submit 1 long 1 428 # y
#submit 1 long 1 429 # too big
submit 2 long 1 1   # y
submit 2 long 1 2   # y
submit 2 long 1 4   # y
submit 2 long 1 8   # y
submit 2 long 1 16  # y
submit 2 long 1 32  # y
submit 2 long 1 64  # y
submit 2 long 1 92  # y
submit 2 long 1 108 # y
submit 2 long 1 110 # y
submit 2 long 1 120 # y
submit 2 long 1 128 # y
submit 2 long 1 140 # y
submit 2 long 1 150 # y
submit 2 long 1 170 # y
submit 2 long 1 180 # y
submit 2 long 1 190 # y
submit 2 long 1 200 # y
submit 2 long 1 210 # y
submit 2 long 1 212 # y
submit 2 long 1 213 # y
submit 2 long 1 214 # y
submit 2 long 1 215 # y
#submit 2 long 1 216 # too big
submit 1 long 2 1   # y
submit 2 long 2 1   # y
submit 2 long 2 2   # y
submit 2 long 2 4   # y
submit 2 long 2 8   # y
submit 2 long 2 16  # y
submit 2 long 2 32  # y
submit 2 long 2 54  # y
submit 2 long 2 55  # y
submit 2 long 2 64  # y
submit 2 long 2 106 # y
submit 2 long 2 107 # y
#submit 2 long 2 108 # too big
#submit 2 long 2 110 # too big
submit 1 long 2 128 # y
submit 1 long 2 214 # y
submit 1 long 2 215 # y
#submit 1 long 2 216 # too big
#submit 1 long 2 218 # too big
#submit 1 long 2 220 # too big
submit 1 long 4 1   # y
submit 2 long 4 1   # y
submit 2 long 4 2   # y
submit 2 long 4 4   # y
submit 2 long 4 8   # y
submit 2 long 4 16  # y
submit 2 long 4 27  # y
submit 2 long 4 28  # y
submit 2 long 4 32  # y
submit 2 long 4 53  # y
#submit 2 long 4 54  # too big
#submit 2 long 4 55  # too big
submit 1 long 4 64  # y
submit 1 long 4 106 # y
submit 1 long 4 107 # y
#submit 1 long 4 108 # too big
submit 1 long 8 1   # y
submit 1 long 8 2   # y
submit 1 long 8 4   # y
submit 1 long 8 8   # y
submit 1 long 8 16  # y
submit 2 long 8 1   # y
submit 2 long 8 2   # y
submit 2 long 8 4   # y
submit 2 long 8 8   # y
submit 2 long 8 13  # y
submit 2 long 8 14  # y
submit 2 long 8 16  # y
submit 2 long 8 26  # y
submit 2 long 8 27  # y
#submit 2 long 8 28  # too big
submit 1 long 8 32  # y
submit 1 long 8 53  # y
#submit 1 long 8 54  # too big
#submit 1 long 8 55  # too big
submit 1 long 16 1  # y
submit 2 long 16 1  # y
submit 2 long 16 2  # y
submit 2 long 16 4  # y
submit 2 long 16 6  # y
submit 2 long 16 7  # y
submit 2 long 16 8  # y
submit 2 long 16 10 # y
submit 2 long 16 12 # y
submit 2 long 16 13 # y
#submit 2 long 16 14 # too big
submit 1 long 16 16 # y
submit 1 long 16 27 # y
submit 1 long 32 1  # y
submit 2 long 32 1  # y
submit 2 long 32 2  # y
submit 2 long 32 3  # y
submit 2 long 32 4  # y
submit 2 long 32 7  # y
#submit 2 long 32 8  # too big
submit 1 long 32 8  # y
submit 1 long 32 13 # y
#submit 1 long 32 14 # too big
submit 1 long 64 1  # y
submit 1 long 64 4  # y
submit 1 long 64 7  # y
#submit 1 long 64 8  # too big
submit 2 long 64 1  # y
submit 2 long 64 2  # y
submit 2 long 64 3  # y
#submit 2 long 64 4  # too big
submit 1 long 128 1 # y
submit 1 long 128 2 # y
#submit 1 long 128 3 # too big
submit 2 long 128 1 # y
#submit 2 long 128 2 # too big
#submit 1 long 256 1 # too big

#submit local float 1 1 1
#submit local float 1 8 1
#submit local float 1 64 1
#submit local float 1 128 1
#submit local float 1 256 1
#submit local float 1 512 1
#submit local float 1 757 1
#submit local float 1 1024 1
#submit local float 1 1515 1
#submit local float 1 1516 1
#submit local float 1 2048 1
#submit local float 1 3032 1
#submit local float 2 1 1
#submit local float 2 1 2
#submit local float 2 2 1
#submit local float 2 2 2
#submit local float 2 4 1
#submit local float 2 4 2
#submit local float 2 8 1
#submit local float 2 8 2
#submit local float 2 16 1
#submit local float 2 16 2
#submit local float 2 32 1
#submit local float 2 32 2
#submit local float 2 64 1
#submit local float 2 64 2
#submit local float 2 128 1
#submit local float 2 128 2
#submit local float 2 256 1
#submit local float 2 256 2
#submit local float 2 512 1
#submit local float 2 512 2
#submit local float 2 757 1
#submit local float 2 757 2
#submit local float 2 1515 1
#submit local float 2 1515 2
#submit local float 2 1516 1
#submit local float 2 1516 2
#submit local float 4 1 1
#submit local float 4 1 2
#submit local float 4 1 4
#submit local float 4 2 1
#submit local float 4 2 2
#submit local float 4 2 4
#submit local float 4 4 1
#submit local float 4 4 2
#submit local float 4 4 4
#submit local float 4 8 1
#submit local float 4 8 2
#submit local float 4 8 4
#submit local float 4 16 1
#submit local float 4 16 2
#submit local float 4 16 4
#submit local float 4 32 1
#submit local float 4 32 2
#submit local float 4 32 4
#submit local float 4 64 1
#submit local float 4 64 2
#submit local float 4 64 4
#submit local float 4 128 1
#submit local float 4 128 2
#submit local float 4 128 4
#submit local float 4 256 1
#submit local float 4 256 2
#submit local float 4 256 4
#submit local float 4 377 1
#submit local float 4 377 2
#submit local float 4 377 4
#submit local float 4 754 4
#submit local float 4 757 4
#submit local float 4 758 4
#submit local float 8 1 1
#submit local float 8 1 2
#submit local float 8 1 4
#submit local float 8 1 8
#submit local float 8 2 1
#submit local float 8 2 2
#submit local float 8 2 4
#submit local float 8 2 8
#submit local float 8 4 1
#submit local float 8 4 2
#submit local float 8 4 4
#submit local float 8 4 8
#submit local float 8 8 1
#submit local float 8 8 2
#submit local float 8 8 4
#submit local float 8 8 8
#submit local float 8 16 1
#submit local float 8 16 2
#submit local float 8 16 4
#submit local float 8 16 8
#submit local float 8 32 1
#submit local float 8 32 2
#submit local float 8 32 4
#submit local float 8 32 8
#submit local float 8 64 1
#submit local float 8 64 2
#submit local float 8 64 4
#submit local float 8 64 8
#submit local float 8 128 1
#submit local float 8 128 2
#submit local float 8 128 4
#submit local float 8 128 8
#submit local float 8 186 1
#submit local float 8 186 2
#submit local float 8 186 4
#submit local float 8 186 8
#submit local float 8 377 8
#submit local float 8 379 8
#submit local float 16 1 1
#submit local float 16 1 2
#submit local float 16 1 4
#submit local float 16 1 8
#submit local float 16 1 16
#submit local float 16 2 1
#submit local float 16 2 2
#submit local float 16 2 4
#submit local float 16 2 8
#submit local float 16 2 16
#submit local float 16 4 1
#submit local float 16 4 2
#submit local float 16 4 4
#submit local float 16 4 8
#submit local float 16 4 16
#submit local float 16 8 1
#submit local float 16 8 2
#submit local float 16 8 4
#submit local float 16 8 8
#submit local float 16 8 16
#submit local float 16 16 1
#submit local float 16 16 2
#submit local float 16 16 4
#submit local float 16 16 8
#submit local float 16 16 16
#submit local float 16 32 1
#submit local float 16 32 2
#submit local float 16 32 4
#submit local float 16 32 8
#submit local float 16 32 16
#submit local float 16 64 1
#submit local float 16 64 2
#submit local float 16 64 4
#submit local float 16 64 8
#submit local float 16 64 16
#submit local float 16 94 16
#submit local float 16 186 16
#submit local float 16 189 16
#submit local float 16 195 16
#submit local float 32 1 1
#submit local float 32 1 2
#submit local float 32 1 4
#submit local float 32 1 8
#submit local float 32 1 16
#submit local float 32 1 32
#submit local float 32 45 1
#submit local float 32 45 2
#submit local float 32 45 4
#submit local float 32 45 8
#submit local float 32 45 16
#submit local float 32 45 32
#submit local float 32 94 32
#submit local float 64 1 1
#submit local float 64 1 2
#submit local float 64 1 4
#submit local float 64 1 8
#submit local float 64 1 16
#submit local float 64 1 32
#submit local float 64 1 64
#submit local float 64 21 1
#submit local float 64 21 2
#submit local float 64 21 4
#submit local float 64 21 8
#submit local float 64 21 16
#submit local float 64 21 32
#submit local float 64 21 64
#submit local float 64 47 64
#submit local float 128 1 1
#submit local float 128 1 2
#submit local float 128 1 4
#submit local float 128 1 8
#submit local float 128 1 16
#submit local float 128 1 32
#submit local float 128 1 64
#submit local float 128 1 128
#submit local float 128 9 1
#submit local float 128 9 2
#submit local float 128 9 4
#submit local float 128 9 8
#submit local float 128 9 16
#submit local float 128 9 32
#submit local float 128 9 64
#submit local float 128 9 128
#submit local float 128 20 1
#submit local float 128 20 128
#submit local float 128 23 128
#submit local float 256 1 1
#submit local float 256 1 2
#submit local float 256 1 4
#submit local float 256 1 8
#submit local float 256 1 16
#submit local float 256 1 32
#submit local float 256 1 64
#submit local float 256 1 128
#submit local float 256 1 256
#submit local float 256 4 1
#submit local float 256 4 256
#submit local float 256 11 256
#submit local float 512 1 1
#submit local float 512 1 2
#submit local float 512 1 4
#submit local float 512 1 8
#submit local float 512 1 16
#submit local float 512 1 32
#submit local float 512 1 64
#submit local float 512 1 128
#submit local float 512 1 256
#submit local float 512 1 512
#submit local float 512 5 512
