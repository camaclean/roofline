#ifndef KERNEL_OCL_H
#define KERNEL_OCL_H

void ocl_kernel(uint64_t nsize,
               uint64_t ntrials,
               double* __restrict__ array,
               int* bytes_per_elem,
               int* mem_accesses_per_elem);
#endif
