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

#define KERNEL1(a,b,c)   ((a) = (b) + (c))
#define KERNEL2(a,b,c)   ((a) = (a)*(b) + (c))

#define ERT_FLOP 1
//#define ERT_FLOP 2
//#define ERT_FLOP 256

__kernel void ocl_kernel(
            __global double* A,
            __global int* params)
{
  const double epsilon=1e-6;
  const double factor=(1.0 - epsilon);
  double alpha = -epsilon;

    size_t i = get_global_id(0);
    double beta = A[i]*factor;

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

    A[i] = beta;

  params[0] = sizeof(*A);	/* bytes_per_elem */
  params[1] = 2;		/* mem_accesses_per_elem */
  params[2] = ERT_FLOP;		/* total flops per trial */
}
