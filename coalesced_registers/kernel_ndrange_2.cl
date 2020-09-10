/* OpenCL ERT kernel */

#define REP2(S)     S;            S
#define REP4(S)     REP2(S);      REP2(S)
#define REP8(S)     REP4(S);      REP4(S)
#define REP16(S)    REP8(S);      REP8(S)
#define REP32(S)    REP16(S);     REP16(S)
#define REP64(S)    REP32(S);     REP32(S)
#define REP128(S)   REP64(S);     REP64(S)
#define REP256(S)   REP128(S);    REP128(S)
#define REP512(S)   REP256(S);    REP256(S)
#define REP1024(S)  REP512(S);    REP512(S)

#define KERNEL1(a, b, c) ((a) = (b) + (c))
#define KERNEL2(a, b, c) ((a) = (a) * (b) + (c))

void kernel1(const ulong elements, __global TYPE *A)
{

  // A needs to be initilized to -1 coming in
  // And with alpha=2 and beta=1, A=-1 is preserved upon return
  TYPE alpha = 2.0;
  TYPE beta  = 1.0;

  size_t i = get_local_id(0);
  const size_t stride = get_local_size(0);

  for (; i < elements; i+= stride)
  {

#if (ERT_FLOP & 1) == 1 /* add 1 flop */
    KERNEL1(beta, A[i], alpha);
#endif
#if (ERT_FLOP & 2) == 2 /* add 2 flops */
    KERNEL2(beta, A[i], alpha);
#endif
#if (ERT_FLOP & 4) == 4 /* add 4 flops */
    REP2(KERNEL2(beta, A[i], alpha));
#endif
#if (ERT_FLOP & 8) == 8 /* add 8 flops */
    REP4(KERNEL2(beta, A[i], alpha));
#endif
#if (ERT_FLOP & 16) == 16 /* add 16 flops */
    REP8(KERNEL2(beta, A[i], alpha));
#endif
#if (ERT_FLOP & 32) == 32 /* add 32 flops */
    REP16(KERNEL2(beta, A[i], alpha));
#endif
#if (ERT_FLOP & 64) == 64 /* add 64 flops */
    REP32(KERNEL2(beta, A[i], alpha));
#endif
#if (ERT_FLOP & 128) == 128 /* add 128 flops */
    REP64(KERNEL2(beta, A[i], alpha));
#endif
#if (ERT_FLOP & 256) == 256 /* add 256 flops */
    REP128(KERNEL2(beta, A[i], alpha));
#endif
#if (ERT_FLOP & 512) == 512 /* add 512 flops */
    REP256(KERNEL2(beta, A[i], alpha));
#endif
#if (ERT_FLOP & 1024) == 1024 /* add 1024 flops */
    REP512(KERNEL2(beta, A[i], alpha));
#endif
#if (ERT_FLOP & 2048) == 2048 /* add 2048 flops */
    REP1024(KERNEL2(beta, A[i], alpha));
#endif

    A[i] = -beta;
  }
}

__attribute__((num_simd_work_items(SIMD)))
__attribute__((num_compute_units(1)))
__attribute__((reqd_work_group_size(SIMD,1,1)))
__kernel void ocl_kernel(const ulong trials,
			 const ulong elements,
			 __global TYPE *A,
			 __global TYPE *B,
			 __global int *params)
{
  const ulong split = elements/2;
  for (size_t i = 0; i < trials; ++i) {
    kernel1(split,A);
    kernel1(split,B);
  }

  params[0] = sizeof(*A); /* bytes_per_elem */
  params[1] = 2;          /* mem_accesses_per_elem */
  params[2] = ERT_FLOP;   /* total flops per trial */
}

// vim: set filetype=c :
