#ifndef NERSC__CL_TEST_FRAMEWORK_HPP
#define  NERSC__CL_TEST_FRAMEWORK_HPP
#include <CL/cl2.hpp>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <tuple>

namespace nersc
{

constexpr size_t operator"" _Ki(unsigned long long v) { return v*1024ULL; }
constexpr size_t operator"" _Mi(unsigned long long v) { return v*1024_Ki; }
constexpr size_t operator"" _Gi(unsigned long long v) { return v*1024_Mi; }

enum class device_selector
{
    autoselect,
    cpu,
    gpu,
    accelerator,
    intel_fpga, 
    xilinx_fpga,
    custom,
    all
};

enum class code_type
{
    source,
    binary
};

template<device_selector Selector>
struct opencl_device_type_flag;

template<device_selector Selector>
inline constexpr cl_device_type opencl_device_type_flag_v = opencl_device_type_flag<Selector>::value;

template<device_selector DeviceSelector>
struct is_fpga;

template<device_selector DeviceSelector>
inline constexpr bool is_fpga_v = is_fpga<DeviceSelector>::value;

template<device_selector DeviceSelector,typename=void>
struct default_device_code_type;

template<device_selector DeviceSelector>
inline constexpr code_type default_device_code_type_v = default_device_code_type<DeviceSelector>::value;

template<device_selector DeviceSelector, code_type CodeType>
struct file_suffix;

template<device_selector DeviceSelector, code_type CodeType>
inline constexpr const char* file_suffix_v = file_suffix<DeviceSelector,CodeType>::value;

template<device_selector DeviceSelector>
std::tuple<cl::Context,cl::Device> init_opencl();

template<device_selector DeviceSelector, code_type CodeType>
std::string make_filename(std::string format, const std::map<std::string,std::string>& params = {});

template<device_selector DeviceSelector, code_type CodeType>
struct program_loader;

template<device_selector DeviceSelector, code_type CodeType>
cl::Program load_program(const cl::Context& context, const cl::Device& device, const std::string& format, const std::map<std::string,std::string>& params = {});

/******************************************************
 * opencl_device_type_flag implementation
 *****************************************************/

template<>
struct opencl_device_type_flag<device_selector::autoselect> : std::integral_constant<cl_device_type, CL_DEVICE_TYPE_DEFAULT> {};

template<>
struct opencl_device_type_flag<device_selector::cpu> : std::integral_constant<cl_device_type, CL_DEVICE_TYPE_CPU> {};

template<>
struct opencl_device_type_flag<device_selector::gpu> : std::integral_constant<cl_device_type, CL_DEVICE_TYPE_GPU> {};

template<>
struct opencl_device_type_flag<device_selector::accelerator> : std::integral_constant<cl_device_type, CL_DEVICE_TYPE_ACCELERATOR> {};

template<>
struct opencl_device_type_flag<device_selector::intel_fpga> : std::integral_constant<cl_device_type, CL_DEVICE_TYPE_ACCELERATOR> {};

template<>
struct opencl_device_type_flag<device_selector::xilinx_fpga> : std::integral_constant<cl_device_type, CL_DEVICE_TYPE_ACCELERATOR> {};

template<>
struct opencl_device_type_flag<device_selector::custom> : std::integral_constant<cl_device_type, CL_DEVICE_TYPE_CUSTOM> {};

template<>
struct opencl_device_type_flag<device_selector::all> : std::integral_constant<cl_device_type, CL_DEVICE_TYPE_ALL> {};

/******************************************************
 * is_fpga implementation
 *****************************************************/

template<device_selector DeviceSelector>
struct is_fpga : std::false_type {};

template<>
struct is_fpga<device_selector::intel_fpga> : std::true_type {};

template<>
struct is_fpga<device_selector::xilinx_fpga> : std::true_type {};

/******************************************************
 * file_suffix implementation
 *****************************************************/

template<device_selector DeviceSelector>
struct file_suffix<DeviceSelector,code_type::source>
{
    static_assert(!is_fpga_v<DeviceSelector>, "FPGAs must use precompiled OpenCL");
    static constexpr const char* value = ".cl";
};

template<device_selector DeviceSelector>
struct file_suffix<DeviceSelector,code_type::binary>
{
    static constexpr const char* value = ".clb";
};


template<>
struct file_suffix<device_selector::intel_fpga,code_type::binary>
{
    static constexpr const char* value = ".aocx";
};

template<>
struct file_suffix<device_selector::xilinx_fpga,code_type::binary>
{
    static constexpr const char* value = ".xclbin";
};

/******************************************************
 * default_device_code_type implementation
 *****************************************************/

template<device_selector DeviceSelector,typename>
struct default_device_code_type
{
    static constexpr code_type value = code_type::source;
};

template<device_selector DeviceSelector>
struct default_device_code_type<DeviceSelector,std::enable_if_t<is_fpga_v<DeviceSelector>>>
{
    static constexpr code_type value = code_type::binary;
};

/******************************************************
 * init_opencl implementation
 *****************************************************/

template<device_selector DeviceSelector>
std::tuple<cl::Context,cl::Device> init_opencl()
{
    cl::Context context(opencl_device_type_flag_v<DeviceSelector>);
    cl::Device device = context.getInfo<CL_CONTEXT_DEVICES>()[0];
    std::string vendor = device.getInfo<CL_DEVICE_VENDOR>();
    std::string device_name = device.getInfo<CL_DEVICE_NAME>();
    std::cout << "Using device: " << vendor << ": " << device_name << std::endl;
    return {context,device};
}

template<>
std::tuple<cl::Context,cl::Device> init_opencl<device_selector::intel_fpga>()
{ 
    cl::Context context(opencl_device_type_flag_v<device_selector::intel_fpga>);
    constexpr const char intel_vendor[] = "Intel";
    for (const auto& candidate : context.getInfo<CL_CONTEXT_DEVICES>())
    {
        std::string vendor = candidate.getInfo<CL_DEVICE_VENDOR>();
        if (std::equal(std::begin(intel_vendor),std::end(intel_vendor),vendor.begin()))
        {
            std::string device_name = candidate.getInfo<CL_DEVICE_NAME>();
            std::cout << "Using device: Intel: " << device_name << std::endl;
            return {context,candidate};
        }
    }
    throw std::runtime_error("No Intel FPGA device found");
}

/******************************************************
 * make_filename implementation
 *****************************************************/

template<device_selector DeviceSelector, code_type CodeType>
inline std::string make_filename(std::string format, const std::map<std::string,std::string>& params)
{
    for (auto& [key,value] : params) 
    {
        format = std::regex_replace(format,std::regex(std::string("\\$\\{") + key + "\\}"),value);
    }
    format += file_suffix_v<DeviceSelector,CodeType>;
#ifndef NDEBUG
    std::cout << "Filename: " << format << std::endl;
#endif
    return format;
}

/******************************************************
 * program_loader and load_program implementation
 *****************************************************/

template<device_selector DeviceSelector>
struct program_loader<DeviceSelector,code_type::source>
{
    static cl::Program load(const cl::Context& context, const cl::Device& device, const std::string& format, const std::map<std::string,std::string>& params = {})
    {
        std::string filename = make_filename<DeviceSelector,code_type::source>(format,params);
        std::ifstream stream(filename);
        if (!stream.is_open()) {
            throw std::runtime_error(std::string("Cannot open file: ") + filename);
        }
     
        std::string source(
            std::istreambuf_iterator<char>(stream),
            (std::istreambuf_iterator<char>()));
        cl::Program program{context, std::move(source)};
        std::string args = "-I .";
        for (auto& [key,value] : params)
        {
            std::string upperkey;
            upperkey.resize(key.length());
            std::transform(key.begin(), key.end(), upperkey.begin(), (int(*)(int)) std::toupper);
            args += " -D" + upperkey + "=" + value;
        }
#ifndef NDEBUG
        std::cout << "Building OpenCL source with arguments: " << args << std::endl;
#endif
        if (program.build({device},args.c_str()) != CL_SUCCESS) {
            throw std::runtime_error(filename + " failed to build");
        }
        return program;
    }
};
template<device_selector DeviceSelector>
struct program_loader<DeviceSelector,code_type::binary>
{
    static cl::Program load(const cl::Context& context, const cl::Device& device, const std::string& format, const std::map<std::string,std::string>& params = {})
    {
        std::string filename = make_filename<DeviceSelector,code_type::binary>(format,params);
        std::ifstream stream(filename,std::ios::in | std::ios::binary);
        if (!stream.is_open()) {
            throw std::runtime_error(std::string("Cannot open file: ") + filename);
        }

        stream.ignore(std::numeric_limits<std::streamsize>::max());
        std::streamsize length = stream.gcount();
        stream.clear();
        stream.seekg(0, std::ios_base::beg);
        std::vector<unsigned char> binary(length);
        std::copy(std::istreambuf_iterator<char>(stream),std::istreambuf_iterator<char>(),binary.begin());
    }
};

template<device_selector DeviceSelector, code_type CodeType>
cl::Program load_program(const cl::Context& context, const cl::Device& device, const std::string& format, const std::map<std::string,std::string>& params)
{
    return program_loader<DeviceSelector,CodeType>::load(context,device,format,params);
}

}

#endif //NERSC__CL_TEST_FRAMEWORK_HPP
