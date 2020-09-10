/* OpenCL ERT kernel */

#define REP2(S)        S ;        S
#define REP4(S)   REP2(S);   REP2(S)
#define REP8(S)   REP4(S);   REP4(S)
#define REP16(S)  REP8(S);   REP8(S)
#define REP32(S)  REP16(S);  REP16(S)
#define REP64(S)  REP32(S);  REP32(S)
#define REP128(S) REP64(S);  REP64(S)
#define REP256(S) REP128(S); REP128(S)
#define REP512(S) REP256(S); REP256(S)
#define REP1024(S) REP512(S); REP512(S)
#define REP2048(S) REP1024(S); REP1024(S)
#define REP4096(S) REP2048(S); REP2048(S)

#define KERNEL1(a,b,c)   ((a) = (b) + (c)); b = __fpga_reg(b); c = __fpga_reg(c)
#define KERNEL2(a,b,c)   ((a) = (a)*(b) + (c)); b = __fpga_reg(b); c = __fpga_reg(c)

#ifndef TYPE
#define TYPE double
#endif

//#ifndef ERT_FLOP
//#  define ERT_FLOP 2
//#  define ERT_FLOP 256
//#endif

#ifndef UNROLL
#define UNROLL 8
#endif


void kernel1(__global TYPE* restrict A, const size_t i) {

  TYPE alpha = 2;
  TYPE beta = 1;


#if (ERT_FLOP & 1) == 1       /* add 1 flop */
      KERNEL1(beta,A[i],alpha);
#endif
#if (ERT_FLOP & 2) == 2       /* add 2 flops */
      KERNEL2(beta,A[i],alpha);
#endif
#if (ERT_FLOP & 4) == 4       /* add 4 flops */
      REP2(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 8) == 8       /* add 8 flops */
      REP4(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 16) == 16     /* add 16 flops */
      REP8(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 32) == 32     /* add 32 flops */
      REP16(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 64) == 64     /* add 64 flops */
      REP32(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 128) == 128   /* add 128 flops */
      REP64(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 256) == 256   /* add 256 flops */
      REP128(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 512) == 512   /* add 512 flops */
      REP256(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 1024) == 1024 /* add 1024 flops */
      REP512(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 2048) == 2048 /* add 1024 flops */
      REP1024(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 4096) == 4096 /* add 1024 flops */
      REP2048(KERNEL2(beta,A[i],alpha));
#endif
#if (ERT_FLOP & 8192) == 8192 /* add 1024 flops */
      REP4096(KERNEL2(beta,A[i],alpha));
#endif

      A[i] = beta;
}

__kernel void ocl_kernel(
            const ulong trials,
            const ulong elements,
            __global TYPE* restrict A,
            __global int* restrict params)
{
  ulong el = elements/UNROLL;
  for (ulong j=0; j<trials; ++j) {
    for (ulong i=0; i<el; ++i)
    {
      #pragma unroll UNROLL
      for (ulong k=0; k<UNROLL; ++k) {
        kernel1(A,i*UNROLL+k);
      }
    }
  }

  params[0] = sizeof(*A);	/* bytes_per_elem */
  params[1] = 2;		/* mem_accesses_per_elem */
  params[2] = ERT_FLOP;		/* total flops per trial */
}

// vim: set filetype=c :
