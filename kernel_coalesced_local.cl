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

#define KERNEL1(a,b,c)   ((a) = (b) + (c))
#define KERNEL2(a,b,c)   ((a) = (a)*(b) + (c))

#ifndef TYPE
#define TYPE double
#endif

//#ifndef ERT_FLOP
//#  define ERT_FLOP 2
//#  define ERT_FLOP 256
//#endif

#ifndef SIZE
#define SIZE (2*1024*1024)
#endif

kernel void ocl_kernel(
            const ulong trials,
            const ulong elements,
            global int* restrict params)
{
  local TYPE A[SIZE/UNROLL/sizeof(TYPE)][UNROLL] __attribute__((numbanks(NUMBANKS),bankwidth(BANKWIDTH)));
  ulong el = elements/UNROLL;
  for (ulong i=0; i<el; ++i) {
    #pragma unroll UNROLL
    for (ulong k=0; k<UNROLL; ++k) {
      A[i][k] = 1.0f;
    }
  }
  for (ulong j=0; j<trials; ++j) {
    for (ulong i=0; i<el; ++i)
    {
      #pragma unroll UNROLL
      for (ulong k=0; k<UNROLL; ++k) {

#if TYPE == float
        const float epsilon=1e-6f;
        const float factor=(1.0f - epsilon);
        const float alpha=-1e-6f;
#elif TYPE == int
        const int factor=2;
        const int alpha=-1;
#else
        const TYPE epsilon=1e-6;
        const TYPE factor=(1.0 - epsilon);
        const TYPE alpha = -epsilon;
#endif
        TYPE tmp = A[i][k];

        TYPE beta = tmp*factor;

#if (ERT_FLOP & 1) == 1       /* add 1 flop */
        KERNEL1(beta,tmp,alpha);
#endif
#if (ERT_FLOP & 2) == 2       /* add 2 flops */
        KERNEL2(beta,tmp,alpha);
#endif
#if (ERT_FLOP & 4) == 4       /* add 4 flops */
        REP2(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 8) == 8       /* add 8 flops */
        REP4(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 16) == 16     /* add 16 flops */
        REP8(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 32) == 32     /* add 32 flops */
        REP16(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 64) == 64     /* add 64 flops */
        REP32(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 128) == 128   /* add 128 flops */
        REP64(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 256) == 256   /* add 256 flops */
        REP128(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 512) == 512   /* add 512 flops */
        REP256(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 1024) == 1024 /* add 1024 flops */
        REP512(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 2048) == 2048 /* add 1024 flops */
        REP1024(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 4096) == 4096 /* add 1024 flops */
        REP2048(KERNEL2(beta,tmp,alpha));
#endif
#if (ERT_FLOP & 8192) == 8192 /* add 1024 flops */
        REP4096(KERNEL2(beta,tmp,alpha));
#endif

        A[i][k] = beta;
      }
    }
  }

  params[0] = sizeof(TYPE);	/* bytes_per_elem */
  params[1] = 2;		/* mem_accesses_per_elem */
  params[2] = ERT_FLOP;		/* total flops per trial */
}


// vim: set filetype=c :
