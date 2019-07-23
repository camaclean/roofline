#include <CL/cl2.hpp>
#include "driver.hpp"
#include "kernel.h"
#include <cmath>
#include "cl_test_framework.hpp"
#include <set>

#ifndef DEVICE
#  define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

// Kernel v1 executes all trials within the kernel
// Kernel v2 submits each trial to the kernel queue
#ifndef KERNEL
#define KERNEL 1 // 1 or 2
#endif

using namespace nersc::literals;

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

template<typename T>
struct type_param;

template<typename T>
inline constexpr const char* type_param_v = type_param<T>::value;

template<>
struct type_param<double>
{
    static constexpr const char value[] = "double";
};

template<>
struct type_param<float>
{
    static constexpr const char value[] = "float";
};

template<>
struct type_param<int>
{
    static constexpr const char value[] = "int";
};

template<>
struct type_param<long>
{
    static constexpr const char value[] = "long";
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

struct roofline_results
{
    //GCC is having problems with = default for some strange reason
    constexpr roofline_results()
        : nsize{}
        , trials{}
        , seconds{}
        , total_bytes{}
        , total_flops{}
    {}
    constexpr roofline_results(const roofline_results&) = default;
    constexpr roofline_results(roofline_results&&) = default;
    constexpr roofline_results(
            size_t nsize,
            uint64_t trials,
            const std::chrono::duration<double>& seconds,
            uint64_t total_bytes,
            uint64_t total_flops)
        : nsize{nsize}
        , trials{trials}
        , seconds{seconds}
        , total_bytes{total_bytes}
        , total_flops{total_flops}
    {}

    size_t nsize;
    uint64_t trials;
    std::chrono::duration<double> seconds;
    uint64_t total_bytes;
    uint64_t total_flops;
};

constexpr bool operator<(const roofline_results& lhs, const roofline_results& rhs) noexcept
{
    return std::tie(lhs.nsize,lhs.trials,lhs.seconds,lhs.total_bytes,lhs.total_flops) < std::tie(rhs.nsize,rhs.trials,rhs.seconds,rhs.total_bytes,rhs.total_flops);
}

struct roofline_config
{
    constexpr roofline_config() noexcept
        : type{""}
        , device_selector{nersc::device_selector::autoselect}
        , trialconfig{trial_config::coalesced}
        , memory_max{}
        , align{}
    {}
    constexpr roofline_config(const roofline_config&) = default;
    constexpr roofline_config(roofline_config&&) = default;
    constexpr roofline_config(
            const char *type,
            nersc::device_selector device_selector,
            trial_config trialconfig,
            size_t memory_max,
            size_t align) noexcept
        : type{type}
        , device_selector{device_selector}
        , trialconfig{trialconfig}
        , memory_max{memory_max}
        , align{align}
    {}
    const char *type;
    nersc::device_selector device_selector;
    trial_config trialconfig;
    size_t memory_max;
    size_t align;
};

bool operator<(const roofline_config& lhs, const roofline_config& rhs) noexcept
{
    int typeres = std::strcmp(lhs.type,rhs.type);
    if (typeres < 0)
        return true;
    else if (typeres > 0)
        return false;
    else
        return std::tie(lhs.device_selector,lhs.trialconfig,lhs.memory_max,lhs.align) < std::tie(rhs.device_selector,rhs.trialconfig,rhs.memory_max,rhs.align);
}

template<typename T = double, 
         typename DeviceSelector = std::integral_constant<nersc::device_selector,nersc::device_selector::autoselect>,
         typename TrialConfig = std::integral_constant<trial_config,trial_config::coalesced>,
         typename MemoryMax = std::integral_constant<size_t,8_Gi>,
         typename Align= std::integral_constant<size_t,32>,
         typename CodeType = std::integral_constant<nersc::code_type,nersc::default_device_code_type_v<DeviceSelector::value>>
    >
class roofline;

template<typename T, nersc::device_selector DeviceSelector, trial_config TrialConfig, size_t MemoryMax, size_t Align, nersc::code_type CodeType>
class roofline<
    T,
    std::integral_constant<nersc::device_selector, DeviceSelector>,
    std::integral_constant<trial_config, TrialConfig>,
    std::integral_constant<size_t,MemoryMax>,
    std::integral_constant<size_t,Align>,
    std::integral_constant<nersc::code_type,CodeType>
>
{
public:
    using result_type = roofline_results;

    template<nersc::code_type CT = CodeType>
    static constexpr const char* kernel_name_format = 
        (CT == nersc::code_type::source) ? 
            "kernel${trial_config}" : "kernel_${trial_config}_${ert_flop}_${type}";

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
      , res{
          {
              type_param_v<T>,
              DeviceSelector,
              TrialConfig,
              MemoryMax,
              Align
          },
          {}
        }

              
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
              {"type", type_param_v<T>},
              {"trial_config",trial_config_name_v<TrialConfig>},
              {"ert_flop",std::to_string(ert_flops)}
          };
        program = nersc::load_program<DeviceSelector,CodeType>(context,device,kernel_name_format<CodeType>,kernel_params);
        ocl_kernel = new ert_kernel_type<TrialConfig>(program, "ocl_kernel");
        print_header();
    }

    void print_header() noexcept
    {
        printf("%12s %12s %15s %12s %12s %12s %12s\n", 
            "nsize", "trials", "microsecs", "bytes", "flops", "GB/s", "GF/s");
    }

    template<bool Verify>
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

                // nsize; trials; microseconds; bytes; flops; GB/s, GiF/s
                printf("%12lu %12lu %15.3lf %12lu %12lu %12.3f %12.3f\n",
                    working_set_size * bytes_per_elem,
                    t,
                    std::chrono::duration<double,std::micro>(seconds).count(),
                    total_bytes,
                    total_flops,
                    total_bytes/seconds.count()/1e9,
                    total_flops/seconds.count()/1e9);

                std::get<1>(res).emplace_back(working_set_size*bytes_per_elem, t, seconds, total_bytes, total_flops);

                if constexpr (Verify)
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

    std::pair<roofline_config, std::vector<roofline_results>> results() const noexcept { return res; }

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
    std::pair<roofline_config, std::vector<roofline_results>> res;
};

int main(int argc, char *argv[]) {
    constexpr nersc::device_selector DeviceSelector = nersc::device_selector::autoselect;
    constexpr nersc::code_type CodeType = nersc::code_type::source;
    constexpr trial_config TrialConfig = trial_config::coalesced;
    //constexpr trial_config TrialConfig = trial_config::queued;
    constexpr size_t MemoryMax=1024_Mi;
    constexpr bool Verify = true;
    using nersc::wrap_integral;

    uint wg_size;

    if (argc == 2) {
        wg_size = atoi(argv[1]);
    }
    else {
        wg_size=0;  // let the OpenCL runtime decide
    }

    std::set<uint> wg_sizes = {1};
    std::set<uint32_t> flop_trials = {1,256};

    //roofline_type_runner<DeviceSelector,TrialConfig,MemoryMax>::run<float,double>(wg_sizes, flop_trials);
    //using Experiment = roofline<double,wrap_integral<DeviceSelector>,wrap_integral<TrialConfig>,wrap_integral<MemoryMax>>;
    using Experiment = roofline<double,wrap_integral<DeviceSelector>,wrap_integral<TrialConfig>,wrap_integral<MemoryMax>>;
    //nersc::conduct_experiment_runtime_ensemble<Experiment,Verify>(wg_sizes, flop_trials);
    //nersc::conduct_experiment_ensemble<roofline,Verify,std::tuple<float,double>>(wg_sizes, flop_trials);
    nersc::conduct_experiment_ensemble<roofline,Verify>(wg_sizes, flop_trials);
    
    return 0;
}
