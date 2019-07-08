#include <CL/cl2.hpp>
#include "driver.hpp"
#include "kernel.h"
#include <cmath>
#include "cl_test_framework.hpp"

#ifndef DEVICE
#  define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

// Kernel v1 executes all trials within the kernel
// Kernel v2 submits each trial to the kernel queue
#ifndef KERNEL
#define KERNEL 1 // 1 or 2
#endif

using nersc::operator""_Ki, nersc::operator""_Mi, nersc::operator""_Gi;

enum class trial_config
{
    coalesced,
    queued
};

template<trial_config Config>
struct trial_config_name;

template<trial_config Config>
inline constexpr const char* trial_config_name_v = trial_config_name<Config>::value;

template<>
struct trial_config_name<trial_config::coalesced>
{
    //static constexpr const char value[] = "coalesced";
    static constexpr const char value[] = "1";
};

template<>
struct trial_config_name<trial_config::queued>
{
    //static constexpr const char value[] = "queued";
    static constexpr const char value[] = "2";
};

template<trial_config Config>
struct ert_kernel_type_selector;

template<trial_config Config>
using  ert_kernel_type = typename ert_kernel_type_selector<Config>::type;

template<>
struct ert_kernel_type_selector<trial_config::coalesced>
{
    using type = cl::KernelFunctor<ulong, cl::Buffer, cl::Buffer>;
};

template<>
struct ert_kernel_type_selector<trial_config::queued>
{
    using type = cl::KernelFunctor<cl::Buffer, cl::Buffer>;
};

//#define ERT_MEMORY_MAX ((uint64_t)8*1024*1024*1024)
//#define ERT_ALIGN 32
//#define ERT_WORKING_SET_MIN 8192
//#define ERT_WORKING_SET_MIN 4194304
//#define ERT_WORKING_SET_MIN 134217728
#define ERT_TRIALS_MIN 1

#undef DEBUG
#define HEADER
#undef VERIFY

template<typename T, trial_config TrialConfig, size_t MemoryMax = 8_Gi, size_t Align=32>
class roofline
{
public:
    using kernel_type = ert_kernel_type<TrialConfig>;

private:
    cl::EnqueueArgs make_enqueue_args(uint64_t n, uint wg_size)
    {
        if (wg_size > 0)
            return cl::EnqueueArgs(queue, cl::NDRange(n), cl::NDRange(wg_size));
        else
            return cl::EnqueueArgs(queue, cl::NDRange(n));
    }

    template<trial_config Config = TrialConfig>
    std::enable_if_t<Config == trial_config::queued> run_kernel(uint64_t n, uint t)
    {
        for (uint i = 0; i < t; ++ i)
            (*ocl_kernel)(make_enqueue_args(n,wg_size),d_buf,d_params);
    };

    template<trial_config Config = TrialConfig>
    std::enable_if_t<Config == trial_config::coalesced> run_kernel(uint64_t n, uint t)
    {
        (*ocl_kernel)(make_enqueue_args(n,wg_size),t,d_buf,d_params);
    };

public:
    roofline(uint wg_size, uint32_t ert_flops=1, uint64_t trials_min=1) 
      : context{}
      , device {}
      , program{}
      , queue{}
      , d_buf{}
      , d_params{}
      , desired_wg_size{wg_size}
      , wg_size{}
      , ert_flops{ert_flops}
      , trials_min{trials_min}
      , ocl_kernel{}
    {};

    template<nersc::device_selector DeviceSelector, nersc::code_type CodeType = nersc::default_device_code_type_v<DeviceSelector>>
    void init()
    {
        std::tie(context,device) = nersc::init_opencl<DeviceSelector>();
        uint max_wg_size = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
        if (desired_wg_size > max_wg_size)
            wg_size = max_wg_size;
        else
            wg_size = desired_wg_size;
        queue = cl::CommandQueue(context);

        std::map<std::string, std::string> kernel_params = 
          {
              {"trial_config",trial_config_name_v<TrialConfig>},
              {"ert_flop",std::to_string(ert_flops)}
          };
        program = nersc::load_program<DeviceSelector,CodeType>(context,device,"kernel${trial_config}",kernel_params);
        ocl_kernel = new ert_kernel_type<TrialConfig>(program, "ocl_kernel");
    }

    void run(uint64_t working_set_min = 1)
    {
        uint64_t n;
        uint64_t t;

        uint64_t TSIZE = MemoryMax;
        uint64_t PSIZE = TSIZE;

        uint64_t nsize = (PSIZE & (~(Align-1))) / sizeof(T);

        std::vector<T> buf(PSIZE/sizeof(T));
        std::vector<int> params(3);


        // initialize the data buffer
        std::fill(buf.begin(),buf.end(),1.0);

        n = working_set_min;
        while (n <= nsize) { // working set - nsize

            // number of trials derived from global memory size and working set size
            uint64_t ntrials = nsize / n;
            if (ntrials < 1)
                ntrials = 1;

            // define parameters on the device
            d_params = cl::Buffer(context, begin(params), end(params), false);

            // loop through increasing numbers of trials
            for (t = trials_min; t <= ntrials; t = t * 2) { // working set - ntrials

                // move fresh buffer to device for each trial
                d_buf = cl::Buffer(context, std::begin(buf), std::begin(buf)+n, false, false);

                // queue up trials
                // note: currently OCL kernel does not account for nid offset
                auto start_time = std::chrono::system_clock::now();
                run_kernel<TrialConfig>(n, t);
                queue.finish();  // wait for all trials to finish
                auto end_time = std::chrono::system_clock::now();

                // print out the resuls
                int bytes_per_elem;
                int mem_accesses_per_elem;

                cl::copy(queue, d_params, begin(params), end(params));
                bytes_per_elem = params[0];
                mem_accesses_per_elem = params[1];
                ert_flops = params[2];
                std::chrono::duration<double> seconds = end_time-start_time;
                uint64_t working_set_size = n;
                uint64_t total_bytes = t * working_set_size * bytes_per_elem * mem_accesses_per_elem;
                uint64_t total_flops = t * working_set_size * ert_flops;

                // nsize; trials; microseconds; bytes; flops; GiB/s, GiF/s
                printf("%12lu %12lu %15.3lf %12lu %12lu %12.3f %12.3f\n",
                    working_set_size * bytes_per_elem,
                    t,
                    std::chrono::duration<double,std::micro>(seconds).count(),
                    total_bytes,
                    total_flops,
                    total_bytes/seconds.count()/1_Gi,
                    total_flops/seconds.count()/1e9);

                verify<T>(PSIZE, t, n);
            } // working set - ntrials
        

            n *= 2; //1.1;

        } // working set - nsize

        printf("\n");
        printf("META_DATA\n");
        printf("FLOPS          %d\n", ert_flops);
        if (wg_size > 0)
            printf("WGSIZE         %d\n", wg_size);
        else
            printf("WGSIZE         NULL\n");

    }
    template<typename U=T>
    std::enable_if_t<std::is_floating_point_v<U>,bool> verify(size_t psize, size_t t, size_t n)
    {
        std::vector<T> bufv(psize/sizeof(T));
        const T epsilon=1e-6;
        const T factor=(1.0 - epsilon);
        T alpha = -epsilon;
        T beta;
        T result = 1.0;
  
        // as a runtime optimization, there's no need to copy data back unless debuging
        // as this is outside the timer loop
        cl::copy(queue, d_buf, begin(bufv), begin(bufv)+n);
        for (size_t i=0; i<t; ++i) {
            beta = result*factor;
            if (ert_flops == 1)
            {
                beta = result+alpha;
            } else {
                for (size_t j=0; j<ert_flops/2; ++j) {
                    beta=beta*result+alpha;
                }
            }
            result = beta;
        }
        for (size_t i=0; i<n; ++i) {
            if (fabs(bufv[i] - result) > 1e-12) {
                printf("ERROR: bufv[%zu] == %e != %e\n", i, bufv[i], result);
                return false;
            }
        }
        return true;
    }

    ~roofline() { delete ocl_kernel; }
private:
    cl::Context context;
    cl::Device device;
    cl::Program program;
    cl::CommandQueue queue;
    cl::Buffer d_buf;
    cl::Buffer d_params;
    uint desired_wg_size;
    uint wg_size;
    uint32_t ert_flops;
    uint64_t trials_min;
    kernel_type* ocl_kernel;
};

int main(int argc, char *argv[]) {
  constexpr nersc::device_selector DeviceSelector = nersc::device_selector::autoselect;
  constexpr nersc::code_type CodeType = nersc::code_type::source;
  constexpr trial_config TrialConfig = trial_config::coalesced;
  //constexpr trial_config TrialConfig = trial_config::queued;
  constexpr size_t MemoryMax=1024_Mi;

  uint wg_size;

  if (argc == 2) {
    wg_size = atoi(argv[1]);
  }
  else {
    wg_size=0;  // let the OpenCL runtime decide
  }

#ifdef HEADER
  {

    printf("%12s %12s %15s %12s %12s %12s %12s\n", 
      "nsize", "trials", "microsecs", "bytes", "flops", "GiB/s", "GF/s");
  }
#endif

  std::cout << "Flops: 1" << std::endl;
  {
      roofline<double,TrialConfig,MemoryMax> roofline(wg_size,1);
      roofline.init<DeviceSelector>();
      roofline.run();
  }
  std::cout << "================================" << std::endl;
  std::cout << "Flops: 2" << std::endl;
  {
      roofline<double,TrialConfig,MemoryMax> roofline(wg_size,2);
      roofline.init<DeviceSelector>();
      roofline.run();
  }
  std::cout << "================================" << std::endl;
  std::cout << "Flops: 256" << std::endl;
  {
      roofline<double,TrialConfig,MemoryMax> roofline(wg_size,256);
      roofline.init<DeviceSelector>();
      roofline.run();
  }
  return 0;
}
