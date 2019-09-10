#include <CL/cl2.hpp>
#include "driver.hpp"
#include "kernel.h"
#include <cmath>
#include "cl_test_framework.hpp"
#include "fpgaoclutils.hpp"
#include <set>
#include <filesystem>

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
    static constexpr const char value[] = "coalesced";
    //static constexpr const char value[] = "1";
};

template<>
struct trial_config_name<trial_config::queued>
{
    static constexpr const char value[] = "queued";
    //static constexpr const char value[] = "2";
};

enum class opencl_kernel_type
{
    ndrange,
    swi
};

template<opencl_kernel_type Type>
struct opencl_kernel_type_name;

template<opencl_kernel_type Type>
constexpr const char* opencl_kernel_type_name_v = opencl_kernel_type_name<Type>::value;

template<>
struct opencl_kernel_type_name<opencl_kernel_type::ndrange>
{
    static constexpr const char value[] = "ndrange";
};

template<>
struct opencl_kernel_type_name<opencl_kernel_type::swi>
{
    static constexpr const char value[] = "swi";
};

template<trial_config Config, opencl_kernel_type KernelType, int Buffers>
struct ert_kernel_type_selector;

template<trial_config Config, opencl_kernel_type KernelType, int Buffers>
using  ert_kernel_type = typename ert_kernel_type_selector<Config, KernelType, Buffers>::type;

template<int Buffers>
struct ert_kernel_type_selector<trial_config::coalesced,opencl_kernel_type::ndrange,Buffers>
{
    using type = nersc::KernelDuplicateBufferFunctor<ulong, cl::Buffer[Buffers], cl::Buffer>;
};

template<int Buffers>
struct ert_kernel_type_selector<trial_config::queued,opencl_kernel_type::ndrange,Buffers>
{
    using type = nersc::KernelDuplicateBufferFunctor<cl::Buffer[Buffers], cl::Buffer>;
};

template<int Buffers>
struct ert_kernel_type_selector<trial_config::coalesced,opencl_kernel_type::swi,Buffers>
{
    using type = nersc::KernelDuplicateBufferFunctor<ulong, ulong, cl::Buffer[Buffers], cl::Buffer>;
};

template<int Buffers>
struct ert_kernel_type_selector<trial_config::queued,opencl_kernel_type::swi,Buffers>
{
    using type = nersc::KernelDuplicateBufferFunctor<ulong, cl::Buffer[Buffers], cl::Buffer>;
};

template<>
struct ert_kernel_type_selector<trial_config::coalesced,opencl_kernel_type::ndrange,0>
{
    using type = cl::KernelFunctor<ulong, cl::Buffer>;
};

template<>
struct ert_kernel_type_selector<trial_config::queued,opencl_kernel_type::ndrange,0>
{
    using type = cl::KernelFunctor<cl::Buffer>;
};

template<>
struct ert_kernel_type_selector<trial_config::coalesced,opencl_kernel_type::swi,0>
{
    using type = cl::KernelFunctor<ulong, ulong, cl::Buffer>;
};

template<>
struct ert_kernel_type_selector<trial_config::queued,opencl_kernel_type::swi,0>
{
    using type = cl::KernelFunctor<ulong, cl::Buffer>;
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
         typename KernelType = std::integral_constant<opencl_kernel_type, opencl_kernel_type::ndrange>,
         typename Buffers = std::integral_constant<int,1>,
         typename MemoryMax = std::integral_constant<size_t,8_Gi>,
         typename Align= std::integral_constant<size_t,32>,
         typename CodeType = std::integral_constant<nersc::code_type,nersc::default_device_code_type_v<DeviceSelector::value>>
    >
class roofline;

template<typename T, nersc::device_selector DeviceSelector, trial_config TrialConfig, opencl_kernel_type KernelType, int Buffers, size_t MemoryMax, size_t Align, nersc::code_type CodeType>
class roofline<
    T,
    std::integral_constant<nersc::device_selector, DeviceSelector>,
    std::integral_constant<trial_config, TrialConfig>,
    std::integral_constant<opencl_kernel_type, KernelType>,
    std::integral_constant<int, Buffers>,
    std::integral_constant<size_t, MemoryMax>,
    std::integral_constant<size_t, Align>,
    std::integral_constant<nersc::code_type, CodeType>
>
{
public:
    using result_type = roofline_results;

    template<size_t B = Buffers>
    static constexpr const char* all_kernel_name_format = (B > 0) ?
                    "${kernel_type}_${buffers}_${trial_config}_${unroll}_${ert_flop}_${type}" :
                    "${kernel_type}_${buffers}_${trial_config}_${unroll}_${els_per_bank}_${ert_flop}_${type}";

    template<nersc::code_type CT = CodeType>
    static constexpr const char* kernel_name_format =
        (CT == nersc::code_type::source) ?
            "${kernel_type}_${trial_config}" : all_kernel_name_format<>;

    using kernel_type = ert_kernel_type<TrialConfig, KernelType, Buffers>;

private:
    cl::EnqueueArgs make_enqueue_args(uint64_t n, uint wg_size)
    {
        if (wg_size > 0)
            return cl::EnqueueArgs(queue, cl::NDRange(n), cl::NDRange(wg_size));
        else
            return cl::EnqueueArgs(queue, cl::NDRange(n));
    }

    template<trial_config Config = TrialConfig, opencl_kernel_type KT = KernelType, size_t DBufs = Buffers>
    std::enable_if_t<Config == trial_config::queued && KT == opencl_kernel_type::ndrange && (DBufs > 0)> run_kernel(uint64_t n, uint t, cl::Buffer (&d_buf)[DBufs], cl::Buffer& d_params)
    {
        for (uint i = 0; i < t; ++ i)
            (*ocl_kernel)(make_enqueue_args(n,wg_size),d_buf,d_params);
    };

    template<trial_config Config = TrialConfig, opencl_kernel_type KT = KernelType, size_t DBufs = Buffers>
    std::enable_if_t<Config == trial_config::queued && KT == opencl_kernel_type::ndrange && DBufs == 0> run_kernel(uint64_t n, uint t, cl::Buffer& d_params)
    {
        for (uint i = 0; i < t; ++ i)
            (*ocl_kernel)(make_enqueue_args(n,wg_size),d_params);
    }

    template<trial_config Config = TrialConfig, opencl_kernel_type KT = KernelType, size_t DBufs = Buffers>
    std::enable_if_t<Config == trial_config::coalesced && KT == opencl_kernel_type::ndrange && (DBufs > 0)> run_kernel(uint64_t n, uint t, cl::Buffer (&d_buf)[DBufs], cl::Buffer& d_params)
    {
        (*ocl_kernel)(make_enqueue_args(n,wg_size),t,d_buf,d_params);
    };

    template<trial_config Config = TrialConfig, opencl_kernel_type KT = KernelType, size_t DBufs = Buffers>
    std::enable_if_t<Config == trial_config::coalesced && KT == opencl_kernel_type::ndrange && DBufs == 0> run_kernel(uint64_t n, uint t, cl::Buffer& d_params)
    {
        (*ocl_kernel)(make_enqueue_args(n,wg_size),t,d_params);
    };

    template<trial_config Config = TrialConfig, opencl_kernel_type KT = KernelType, size_t DBufs = Buffers>
    std::enable_if_t<Config == trial_config::queued && KT == opencl_kernel_type::swi && (DBufs > 0)> run_kernel(uint64_t n, uint t, cl::Buffer (&d_buf)[DBufs], cl::Buffer& d_params)
    {
        for (uint i = 0; i < t; ++ i)
            (*ocl_kernel)(cl::EnqueueArgs(queue,1),n,d_buf,d_params);
    };
    template<trial_config Config = TrialConfig, opencl_kernel_type KT = KernelType, size_t DBufs = Buffers>
    std::enable_if_t<Config == trial_config::queued && KT == opencl_kernel_type::swi && DBufs == 0> run_kernel(uint64_t n, uint t, cl::Buffer& d_params)
    {
        for (uint i = 0; i < t; ++ i)
            (*ocl_kernel)(cl::EnqueueArgs(queue,1),n,d_params);
    };

    template<trial_config Config = TrialConfig, opencl_kernel_type KT = KernelType, size_t DBufs = Buffers>
    std::enable_if_t<Config == trial_config::coalesced && KT == opencl_kernel_type::swi && (DBufs > 0)> run_kernel(uint64_t n, uint t, cl::Buffer (&d_buf)[DBufs], cl::Buffer& d_params)
    {
        cl_int err;
        (*ocl_kernel)({queue,{1}},t,n,d_buf,d_params,err);
        if (err != CL_SUCCESS)
            std::cerr << "Error running kernel: " << err << std::endl;
    };
    template<trial_config Config = TrialConfig, opencl_kernel_type KT = KernelType, size_t DBufs = Buffers>
    std::enable_if_t<Config == trial_config::coalesced && KT == opencl_kernel_type::swi && DBufs == 0> run_kernel(uint64_t n, uint t, cl::Buffer& d_params)
    {
        cl_int err;
        (*ocl_kernel)({queue,{1}},t,n,d_params,err);
        if (err != CL_SUCCESS)
            std::cerr << "Error running kernel: " << err << std::endl;
    };

public:
    roofline(uint wg_size, uint32_t ert_flops=1, uint64_t trials_min=1, uint unroll = 1, int els_per_bank=1)
      : context{}
      , device {}
      , program{}
      , queue{}
      , desired_wg_size{wg_size}
      , wg_size{}
      , ert_flops{ert_flops}
      , trials_min{trials_min}
      , unroll{unroll}
      , ocl_kernel{}
      , done{false}
      , outfile{}
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

        std::string buffers;
        if (Buffers == 0)
            buffers = "local";
        else
            buffers = std::to_string(Buffers);
        std::string devicename = device.getInfo<CL_DEVICE_NAME>();
        std::string shortdevname = devicename.substr(0,devicename.find(' '));
        std::map<std::string, std::string> kernel_params =
          {
              {"kernel_type", opencl_kernel_type_name_v<KernelType>},
              {"type", type_param_v<T>},
              {"trial_config",trial_config_name_v<TrialConfig>},
              {"ert_flop",std::to_string(ert_flops)},
              {"unroll",std::to_string(unroll)},
              {"buffers",buffers},
              {"device_type",shortdevname},
              {"els_per_bank",std::to_string(els_per_bank)},
          };
        std::string name = nersc::make_name((std::string("${device_type}_") + all_kernel_name_format<>).c_str(),kernel_params);
        std::string filename = name + ".log";
        done = std::filesystem::exists(filename);

        if (done)
            return;

        uint max_wg_size = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
        if (desired_wg_size > max_wg_size)
            wg_size = max_wg_size;
        else
            wg_size = desired_wg_size;
        queue = cl::CommandQueue(context);

        program = nersc::load_program<DeviceSelector,CodeType>(context,device,kernel_name_format<>,kernel_params);
        ocl_kernel = new kernel_type(program, "ocl_kernel");
        if constexpr (Buffers == 0)
            std::cout << "Kernel type: " <<  abi::__cxa_demangle(typeid(kernel_type).name(),0,0,0) << std::endl;
        else
            std::cout << "Kernel type: " <<  abi::__cxa_demangle(typeid(typename kernel_type::kernel_functor_type).name(),0,0,0) << std::endl;

        outfile = fopen(filename.c_str(),"w");

        for (auto out : {stdout, outfile})
            print_header(out);
    }

    void print_header(FILE *f = stdout) noexcept
    {
        fprintf(f, "%12s %12s %8s %8s %8s %12s %12s %15s %12s %12s %12s %12s\n",
            "type",
            "trial_config",
            "ert_flop",
            "unroll",
            "buffers",
            "nsize", "trials", "microsecs", "bytes", "flops", "GB/s", "GF/s");
        fflush(f);
    }

    void print_data(FILE *f, uint64_t working_set_size, int bytes_per_elem, uint64_t total_bytes, uint64_t total_flops, std::chrono::duration<double> seconds, uint64_t t)
    {
        fprintf(f,"%12s %12s %8u %8u %8u %12lu %12lu %15.3lf %12lu %12lu %12.3f %12.3f\n",
            type_param_v<T>,
            trial_config_name_v<TrialConfig>,
            ert_flops,
            unroll,
            Buffers,
            working_set_size * bytes_per_elem,
            t,
            std::chrono::duration<double,std::micro>(seconds).count(),
            total_bytes,
            total_flops,
            total_bytes/seconds.count()/1e9,
            total_flops/seconds.count()/1e9);
        fflush(f);
    }

    template<bool Verify>
    void run(uint64_t working_set_min = 1)
    {
        if (done)
            return;
        uint64_t n;
        uint64_t t;

        int line = 0;

        size_t real_memory_max = MemoryMax;
        if constexpr (DeviceSelector == nersc::device_selector::intel_fpga && Buffers == 1)
        {
            real_memory_max /= 2;
            if (working_set_min == MemoryMax/sizeof(T))
                working_set_min /= 2;
        }

        uint64_t TSIZE = real_memory_max;
        uint64_t PSIZE = TSIZE;

        uint64_t nsize = (PSIZE & (~(Align-1))) / sizeof(T);

        std::vector<T> buf(PSIZE/sizeof(T));
        std::vector<int> params(3);

        // initialize the data buffer
        if (Buffers > 0)
            std::fill(buf.begin(),buf.end(),1.0);

        cl_int err = 0;
        cl::Buffer d_params = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * 3, nullptr, &err);
        if (err != CL_SUCCESS)
            std::cerr << "Error creating d_params: " << err << std::endl;

        if (Buffers > 0)
            n = (working_set_min > unroll*Buffers) ? working_set_min : unroll*Buffers;
        else
            n = (working_set_min > unroll) ? working_set_min : unroll;

        while (n <= nsize) { // working set - nsize

            // number of trials derived from global memory size and working set size
            uint64_t ntrials = nsize / n;
            if (ntrials < 1)
                ntrials = 1;
            if (ntrials > 1<<26 && Buffers > 0)
                ntrials = 1 << 26;

            if (n == real_memory_max/sizeof(T) && Buffers > 0)
            {
                n -= 1_Mi/sizeof(T);
                nsize -= 1_Mi/sizeof(T);
            }

            // loop through increasing numbers of trials
            for (t = ntrials /*trials_min*/; t <= ntrials; t = t * 2) { // working set - ntrials
                cl::Buffer d_buf[Buffers];
                auto tr = t;
                if constexpr (Buffers == 0)
                {
                    if (tr < 1024)
                        tr = 1024;
                }
                else if constexpr (Buffers == 1)
                {
                    d_buf[0] = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(T) * n, nullptr, &err);
                    if (err != CL_SUCCESS)
                        std::cerr << "Error creating d_buf: " << err << std::endl;
                    cl::copy(queue, begin(buf), begin(buf)+n, d_buf[0]);
                }
                else if constexpr (Buffers == 2)
                {
                    d_buf[0] = cl::Buffer(context, CL_MEM_READ_WRITE|CL_CHANNEL_1_INTELFPGA, sizeof(T) * n/2, nullptr, &err);
                    if (err != CL_SUCCESS)
                        std::cerr << "Error creating d_buf[0]: " << err << " " << sizeof(T)*n/2 << " " << n << " " << MemoryMax << std::endl;
                    cl::copy(queue, begin(buf), begin(buf)+n/2, d_buf[0]);
                    d_buf[1] = cl::Buffer(context, CL_MEM_READ_WRITE|CL_CHANNEL_2_INTELFPGA, sizeof(T) * n/2, nullptr, &err);
                    if (err != CL_SUCCESS)
                        std::cerr << "Error creating d_buf[1]: " << err << " " << sizeof(T)*n/2 << " " << n << " " << MemoryMax << std::endl;
                    cl::copy(queue, begin(buf), begin(buf)+n/2, d_buf[1]);
                }

                // queue up trials
                // note: currently OCL kernel does not account for nid offset
                auto start_time = std::chrono::system_clock::now();
                if constexpr (Buffers == 0)
                    run_kernel<TrialConfig>(n, tr, d_params);
                else
                    run_kernel<TrialConfig>(n, tr, d_buf, d_params);
                queue.finish();  // wait for all trials to finish
                auto end_time = std::chrono::system_clock::now();

                // print out the resuls
                int bytes_per_elem;
                int mem_accesses_per_elem;

                cl::copy(queue, d_params, begin(params), end(params));
                bytes_per_elem = sizeof(T); //params[0];
                mem_accesses_per_elem = params[1];
                ert_flops = params[2];
                std::chrono::duration<double> seconds = end_time-start_time;
                uint64_t working_set_size = n;
                uint64_t total_bytes = tr * working_set_size * bytes_per_elem * mem_accesses_per_elem;
                uint64_t total_flops = tr * working_set_size * ert_flops;

                // nsize; trials; microseconds; bytes; flops; GB/s, GiF/s
                for (auto out : {stdout,outfile})
                    print_data(out, working_set_size, bytes_per_elem, total_bytes, total_flops, seconds, tr);

                std::get<1>(res).emplace_back(working_set_size*bytes_per_elem, tr, seconds, total_bytes, total_flops);

                if constexpr (Verify)
                    verify<T>(PSIZE, tr, n, d_buf);
            } // working set - ntrials

            //print_header(stdout);
            n *= 2; //1.1;

        } // working set - nsize
    }
    template<typename U=T>
    std::enable_if_t<std::is_floating_point_v<U>,bool> verify(size_t psize, size_t t, size_t n, cl::Buffer (&d_buf)[Buffers])
    {
        std::vector<T> bufv(psize/sizeof(T));
        const T epsilon=1e-6;
        const T factor=(1.0 - epsilon);
        T alpha = -epsilon;
        T beta;
        T result = 1.0;

        // as a runtime optimization, there's no need to copy data back unless debuging
        // as this is outside the timer loop
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
        for (size_t b = 0; b < Buffers; ++b)
        {
            cl::copy(queue, d_buf[b], begin(bufv), begin(bufv)+n/Buffers);
            for (size_t i=0; i<n/Buffers; ++i) {
                if (fabs(bufv[i] - result) > 1e-12) {
                    printf("ERROR: bufv[%zu] == %e != %e\n (Buffer %zu)", i, bufv[i], result, b);
                    fflush(stdout);
                    return false;
                }
            }
        }
        return true;
    }

    std::pair<roofline_config, std::vector<roofline_results>> results() const noexcept { return res; }

    ~roofline() { delete ocl_kernel; if (outfile) fclose(outfile); }
private:
    cl::Context context;
    cl::Device device;
    cl::Program program;
    cl::CommandQueue queue;
    uint desired_wg_size;
    uint wg_size;
    uint32_t ert_flops;
    uint64_t trials_min;
    uint unroll;
    kernel_type* ocl_kernel;
    bool done;
    FILE* outfile;
    std::pair<roofline_config, std::vector<roofline_results>> res;
};

int main(int argc, char *argv[]) {
    constexpr nersc::device_selector DeviceSelector = nersc::device_selector::intel_fpga;
    //constexpr nersc::device_selector DeviceSelector = nersc::device_selector::cpu; //FIXME
    //constexpr nersc::device_selector DeviceSelector = nersc::device_selector::autoselect;
    constexpr nersc::code_type CodeType = nersc::code_type::source;
    constexpr trial_config TrialConfig = trial_config::coalesced;
    //constexpr trial_config TrialConfig = trial_config::queued;
    constexpr size_t MemoryMax=8_Gi;
    constexpr bool Verify = false;
    //constexpr int Buffers = 2;
    using Buffers = std::integer_sequence<int,0,2>;
    constexpr auto KernelType = opencl_kernel_type::swi;
    //constexpr auto KernelType = opencl_kernel_type::ndrange;
    using nersc::wrap_integral;

    //roofline_type_runner<DeviceSelector,TrialConfig,MemoryMax>::run<float,double>(wg_sizes, flop_trials);
    //using Experiment = roofline<double,wrap_integral<DeviceSelector>,wrap_integral<TrialConfig>,wrap_integral<MemoryMax>>;
    using Experiment = roofline<float,wrap_integral<DeviceSelector>,wrap_integral<TrialConfig>,wrap_integral<KernelType>,wrap_integral<1>>;
    //Experiment e(1,1,1,1);
    //e.run<false>();
    //nersc::conduct_experiment_runtime_ensemble<Experiment,Verify>(wg_sizes, flop_trials);
    //nersc::conduct_experiment_ensemble<roofline,Verify,std::tuple<float,double>>(wg_sizes, flop_trials);
    //nersc::conduct_experiment_ensemble<roofline,Verify,float,wrap_integral<DeviceSelector>,wrap_integral<TrialConfig>,wrap_integral<KernelType>,wrap_integral<Buffers>>(wg_sizes, flop_trials, 1, unrolls);

    {
        std::set<uint> wg_sizes = {1};
        std::set<uint32_t> flop_trials = {1,2,4,8,16,32,64,128,256,512,1024};
        std::set<uint> unrolls = {1,2,3,4,5,6,7,8,16,32,64,128,192,256};
        //Runs every permutation of template combination enumerated by std::tuple and std::integral_sequence plus every combination of runtime parameters
        //Skips if kernel does not exist
        using DataTypes = std::tuple<long,int,double,float>;
        nersc::conduct_experiment_ensemble<
            roofline,
            Verify,
            DataTypes,
            wrap_integral<DeviceSelector>,
            wrap_integral<TrialConfig>,
            wrap_integral<KernelType>,
            std::integral_constant<int,2>,
            wrap_integral<MemoryMax>
        >(wg_sizes, flop_trials, 1, unrolls);
    }
    {
        std::set<uint> wg_sizes = {1};
        std::set<uint32_t> flop_trials = {187,3032};
        std::set<uint> unrolls = {1,2,4,8,16,32,64,128};
        nersc::conduct_experiment_ensemble<
            roofline,
            Verify,
            float,
            wrap_integral<DeviceSelector>,
            wrap_integral<TrialConfig>,
            wrap_integral<KernelType>,
            std::integral_constant<int,2>,
            wrap_integral<MemoryMax>
        >(wg_sizes, flop_trials, 1, unrolls);
    }
    {
        std::set<uint> wg_sizes = {1};
        std::set<uint32_t> flop_trials = {3,6,13,27,54,92,108};
        std::set<uint> unrolls = {1,2,4,8,16,32,64,128};
        nersc::conduct_experiment_ensemble<
            roofline,
            Verify,
            long,
            wrap_integral<DeviceSelector>,
            wrap_integral<TrialConfig>,
            wrap_integral<KernelType>,
            std::integral_constant<int,2>,
            wrap_integral<MemoryMax>
        >(wg_sizes, flop_trials, 1, unrolls);
    }
    {
        std::set<uint> wg_sizes = {1};
        std::set<uint32_t> flop_trials = {3,15,30,62,125,253,505};
        std::set<uint> unrolls = {1,2,4,8,16,32,64,128};
        nersc::conduct_experiment_ensemble<
            roofline,
            Verify,
            int,
            wrap_integral<DeviceSelector>,
            wrap_integral<TrialConfig>,
            wrap_integral<KernelType>,
            std::integral_constant<int,2>,
            wrap_integral<MemoryMax>
        >(wg_sizes, flop_trials, 1, unrolls);
    }
    {
        std::set<uint> wg_sizes = {1};
        std::set<uint32_t> flop_trials = {3,7,15,155};
        std::set<uint> unrolls = {1,2,4,8,16,32};
        nersc::conduct_experiment_ensemble<
            roofline,
            Verify,
            double,
            wrap_integral<DeviceSelector>,
            wrap_integral<TrialConfig>,
            wrap_integral<KernelType>,
            std::integral_constant<int,2>,
            wrap_integral<MemoryMax>
        >(wg_sizes, flop_trials, 1, unrolls);
    }
    {
        std::set<uint> wg_sizes = {1};
        std::set<uint32_t> flop_trials = {5,9,11,20,21,23,32,45,47,92,94,186,187,189,377,379,754,757,758,1514,1515,1516,3000,3030,3032};
        std::set<uint> unrolls = {1,2,4,8,16,32,64,128,256,512};
        std::set<size_t> els_per_bank = {1,2,4,8,16,32,64,128,256,512};
        nersc::conduct_experiment_ensemble<
            roofline,
            Verify,
            float,
            wrap_integral<DeviceSelector>,
            wrap_integral<TrialConfig>,
            wrap_integral<KernelType>,
            std::integral_constant<int,0>,
            std::integral_constant<size_t,16_Mi>
        >(wg_sizes, flop_trials, 1, unrolls, els_per_bank);
    }
    {
        std::set<uint32_t> flop_trials = {1,64,128,1024,1515};
        std::set<uint> unrolls = {1,2,4,8,16,32,64,128,256};
        nersc::conduct_experiment_ensemble<
            roofline,
            Verify,
            float,
            wrap_integral<DeviceSelector>,
            wrap_integral<TrialConfig>,
            wrap_integral<KernelType>,
            std::integral_constant<int,1>,
            wrap_integral<MemoryMax>
        >(1, flop_trials, 1, unrolls);
    }
    /*
    nersc::conduct_experiment_ensemble<
        roofline,
        Verify,
        float,
        wrap_integral<DeviceSelector>,
        wrap_integral<TrialConfig>,
        wrap_integral<opencl_kernel_type::swi>,
        std::integral_constant<int,1>,
        wrap_integral<MemoryMax>
        >(1, std::set<uint32_t>{2048,3030,3032}, 1, 1);
    nersc::conduct_experiment_ensemble<
        roofline,
        Verify,
        float,
        wrap_integral<DeviceSelector>,
        wrap_integral<TrialConfig>,
        wrap_integral<opencl_kernel_type::ndrange>,
        std::integral_constant<int,1>,
        wrap_integral<MemoryMax>
        >(1<<12, 1, 1, 1);
    */


    return 0;
}
