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

namespace impl
{
template<typename T,typename=void>
struct is_iterable_impl;
}

inline namespace literals {

constexpr size_t operator"" _Ki(unsigned long long v) noexcept { return v*1024ULL; }
constexpr size_t operator"" _Mi(unsigned long long v) noexcept { return v*1024_Ki; }
constexpr size_t operator"" _Gi(unsigned long long v) noexcept { return v*1024_Mi; }
constexpr size_t operator"" _K(unsigned long long v) noexcept { return v*1e3; }
constexpr size_t operator"" _M(unsigned long long v) noexcept { return v*1e6; }
constexpr size_t operator"" _G(unsigned long long v) noexcept { return v*1e9; }

}

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

std::string make_name(std::string format, const std::map<std::string,std::string>& params = {});

template<device_selector DeviceSelector, code_type CodeType>
std::string make_filename(std::string format, const std::map<std::string,std::string>& params = {});

template<device_selector DeviceSelector, code_type CodeType>
struct program_loader;

template<device_selector DeviceSelector, code_type CodeType>
cl::Program load_program(const cl::Context& context, const cl::Device& device, const std::string& format, const std::map<std::string,std::string>& params = {});

template<auto I>
using wrap_integral = std::integral_constant<decltype(I),I>;

template<template<typename...> typename T, typename U>
struct is_same_template;

template<template<typename...> typename T, typename U>
inline constexpr bool is_same_template_v = is_same_template<T,U>::value;

template<typename T>
using is_iterable = impl::is_iterable_impl<T>;

template<typename T>
inline constexpr bool is_iterable_v = is_iterable<T>::value;

template<typename Experiment,typename=void>
struct experiment_has_results;

template<typename Experiment>
inline constexpr bool experiment_has_results_v = experiment_has_results<Experiment>::value;

template<typename... Ts>
struct type_pack {};

template<typename T, typename Template>
struct prepend_template;

template<typename T, typename Template>
using prepend_template_type = typename prepend_template<T,Template>::type;

template<typename... Ts>
struct pack_cat;

template<typename... Ts>
using pack_cat_type = typename pack_cat<Ts...>::type;

template<typename Template, typename T>
struct prepend_each_template;

template<typename Template, typename T>
using prepend_each_template_type = typename prepend_each_template<Template, T>::type;

template<typename TemplateWithPack, typename... Ts>
struct prepend_all_all_template;

template<typename TemplateWithPack, typename... Ts>
using prepend_all_all_template_type = typename prepend_all_all_template<TemplateWithPack,Ts...>::type;

template<typename... Ts>
struct expand_template_collection;

template<typename... Ts>
using expand_template_collection_type = typename expand_template_collection<Ts...>::type;

template<typename Experiment, bool Verify, typename... RuntimeParams>
decltype(auto) conduct_experiment(RuntimeParams&&... params);

template<typename Experiment, bool Verify, typename... RuntimeParams>
decltype(auto) conduct_experiment_runtime_ensemble(RuntimeParams&&... params);

template<template<typename...> typename Experiment, bool Verify, typename... Params, typename... RuntimeParams>
decltype(auto) conduct_experiment_ensemble(RuntimeParams&&... params);

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
        if (std::equal(std::begin(intel_vendor),std::end(intel_vendor)-1,vendor.begin()))
        {
            std::string device_name = candidate.getInfo<CL_DEVICE_NAME>();
            return {context,candidate};
        }
    }
    throw std::runtime_error("No Intel FPGA device found");
}

/******************************************************
 * make_filename implementation
 *****************************************************/

inline std::string make_name(std::string format, const std::map<std::string,std::string>& params)
{
    for (auto& [key,value] : params) 
    {
        format = std::regex_replace(format,std::regex(std::string("\\$\\{") + key + "\\}"),value);
    }
    return format;
}

template<device_selector DeviceSelector, code_type CodeType>
inline std::string make_filename(std::string format, const std::map<std::string,std::string>& params)
{
    format = make_name(format,params);
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
        //cl::Program::Binaries binaries{1,binary};
        //std::vector<cl::Device> devices{1,device};
        return {context, {device}, {binary}};
    }
};

template<device_selector DeviceSelector, code_type CodeType>
cl::Program load_program(const cl::Context& context, const cl::Device& device, const std::string& format, const std::map<std::string,std::string>& params)
{
    return program_loader<DeviceSelector,CodeType>::load(context,device,format,params);
}

/******************************************************
 * is_same_template implementation
 *****************************************************/

template<template<typename...> typename T, typename U>
struct is_same_template : std::false_type {};

template<template<typename...> typename T,  typename... Us>
struct is_same_template<T,T<Us...>> : std::true_type {};

/******************************************************
 * experiment_has_results implementation
 *****************************************************/

template<typename Experiment,typename>
struct experiment_has_results : std::false_type {};

template<typename Experiment>
struct experiment_has_results<Experiment, std::void_t<decltype(std::declval<Experiment>().results())>> : std::true_type {};

/******************************************************
 * is_iterable implementation
 *****************************************************/

namespace impl {

template<typename T,typename>
struct is_iterable_impl : std::false_type {};

using std::begin;
using std::end;

template<typename T>
struct is_iterable_impl<T,std::void_t<
    decltype(begin(std::declval<T&>()) != end(std::declval<T&>())),
    decltype(++begin(std::declval<T&>())),
    decltype(*begin(std::declval<T&>()))>>
    : std::true_type {};

}

/******************************************************
 * prepend_template implementation
 *****************************************************/

template<typename T,template<typename...> typename Template, typename... Ts>
struct prepend_template<T,Template<Ts...>>
{
    using type = Template<T,Ts...>;
};

/******************************************************
 * pack_cat implementation
 *****************************************************/

template<template<typename...> typename Outer, typename... Ts>
struct pack_cat<Outer<Ts...>>
{
    using type = Outer<Ts...>;
};

template<template<typename...> typename Outer, typename... Ts, typename... Us>
struct pack_cat<Outer<Ts...>,Outer<Us...>>
{
    using type = Outer<Ts...,Us...>;
};

template<template<typename...> typename Outer, typename... Ts, typename... Us, typename... More>
struct pack_cat<Outer<Ts...>,Outer<Us...>,More...>
    : pack_cat<Outer<Ts...,Us...>,typename pack_cat<More...>::type>
{};

/******************************************************
 * prepend_each_template implementation
 *****************************************************/

template<template<typename...> typename Outer, typename... Inners, typename T>
struct prepend_each_template<Outer<Inners...>,T>
{
    using type = Outer<prepend_template_type<T,Inners>...>;
};

/******************************************************
 * prepend_all_all_template implementation
 *****************************************************/
template<typename TemplateWithPack, typename... Ts>
struct prepend_all_all_template
{
    using type = pack_cat_type<typename prepend_each_template<TemplateWithPack,Ts>::type...>;
};

/******************************************************
 * expand_template_collection implementation
 *****************************************************/

template<typename T, typename... Ts>
struct expand_template_collection<T,Ts...>
{
    using type = prepend_each_template_type<typename expand_template_collection<Ts...>::type,T>;
};

template<typename...Inners, typename... Ts>
struct expand_template_collection<std::tuple<Inners...>,Ts...>
{
    using type = prepend_all_all_template_type<typename expand_template_collection<Ts...>::type,Inners...>;
};

template<typename I, I... Is, typename... Ts>
struct expand_template_collection<std::integer_sequence<I,Is...>,Ts...>
{
    using type = prepend_all_all_template_type<typename expand_template_collection<Ts...>::type,std::integral_constant<I,Is>...>;
};

template<typename...Ts>
struct expand_template_collection<std::tuple<Ts...>>
{
    using type = type_pack<type_pack<Ts>...>;
};

template<typename I, I... Is>
struct expand_template_collection<std::integer_sequence<I,Is...>>
{
    using type = type_pack<type_pack<std::integral_constant<I,Is>>...>;
};

template<typename T>
struct expand_template_collection<T>
{
    using type = type_pack<type_pack<T>>;
};

template<>
struct expand_template_collection<>
{
    using type = type_pack<>;
};

/******************************************************
 * conduct_experiment implementation
 *****************************************************/

template<typename Experiment, bool Verify, typename... RuntimeParams>
decltype(auto) conduct_experiment(RuntimeParams&&... params)
{
    Experiment e(std::forward<RuntimeParams>(params)...);
    e.template run<Verify>();
    if constexpr (experiment_has_results_v<Experiment>)
        return e.results();
}

/******************************************************
 * conduct_experiment_runtime_ensemble implementation
 *****************************************************/

namespace impl
{

template<size_t I, typename Experiment, bool Verify, typename RuntimeParams, typename Results, typename... Ts, std::enable_if_t<I == std::tuple_size_v<RuntimeParams>>* = nullptr>
void conduct_experiment_runtime_ensemble_impl(RuntimeParams&& params, Results *results, Ts&&... args)
{
    try
    {
        if constexpr (experiment_has_results_v<Experiment>)
        {
            results->emplace(conduct_experiment<Experiment,Verify>(std::forward<Ts>(args)...));
        } else {
            conduct_experiment<Experiment,Verify>(std::forward<Ts>(args)...);
        }
    } catch (std::runtime_error e)
    {
        std::cerr << "Experiment failed: " << e.what() << std::endl;
    }
}

template<size_t I, typename Experiment, bool Verify, typename RuntimeParams, typename Results, typename... Ts, std::enable_if_t<I != std::tuple_size_v<RuntimeParams>>* = nullptr>
void conduct_experiment_runtime_ensemble_impl(RuntimeParams&& params, Results *results, Ts&&... args)
{
    using param_type = std::remove_reference_t<std::tuple_element_t<I,RuntimeParams>>; 
    if constexpr (is_iterable_v<param_type>)
    {
        for (auto& param : std::get<I>(std::forward<RuntimeParams>(params)))
        {
            try
            {
                conduct_experiment_runtime_ensemble_impl<I+1,Experiment,Verify>(std::forward<RuntimeParams>(params), results, std::forward<Ts>(args)..., param);
            } catch (std::runtime_error e)
            {
                std::cerr << "Experiment failed: " << e.what() << std::endl;
            }
        }
    } else {
        try 
        {
            conduct_experiment_runtime_ensemble_impl<I+1,Experiment,Verify>(std::forward<RuntimeParams>(params), results, std::forward<Ts>(args)..., std::get<I>(std::forward<RuntimeParams>(params)));
        } catch (std::runtime_error e)
        {
            std::cerr << "Experiment failed: " << e.what() << std::endl;
        }
    }
}

}

template<typename Experiment, bool Verify, typename... RuntimeParams>
decltype(auto) conduct_experiment_runtime_ensemble(RuntimeParams&&... params)
{
    if constexpr (experiment_has_results_v<Experiment>) 
    {
        using results_type = decltype(std::declval<Experiment>().results());
        using key_type = std::remove_reference_t<decltype(std::get<0>(std::declval<results_type&>()))>;
        using value_type = std::remove_reference_t<decltype(std::get<1>(std::declval<results_type&>()))>;
        std::map<key_type, value_type> results;
        impl::conduct_experiment_runtime_ensemble_impl<0,Experiment,Verify>(std::forward_as_tuple(std::forward<RuntimeParams>(params)...),&results);
        return results;
    } else {
        impl::conduct_experiment_runtime_ensemble_impl<0,Experiment,Verify>(std::forward_as_tuple(std::forward<RuntimeParams>(params)...),nullptr);
    }
}

/******************************************************
 * conduct_experiment_runtime_ensemble implementation
 *****************************************************/

namespace impl
{

template<template<typename...> typename Experiment, bool Verify, template<typename...> typename Packer, typename... Params, typename... RuntimeParams>
decltype(auto) conduct_experiment_ensemble_impl2(Packer<Params...>, RuntimeParams&&... params)
{
    using RealExperiment = Experiment<Params...>;
    return conduct_experiment_runtime_ensemble<RealExperiment,Verify>(std::forward<RuntimeParams>(params)...);
}

template<template<typename...> typename Experiment, bool Verify, template<typename...> typename Packer, typename FirstTest, typename... Tests, typename... RuntimeParams>
decltype(auto) conduct_experiment_ensemble_impl(Packer<FirstTest,Tests...>, RuntimeParams&&... params)
{
    if constexpr (experiment_has_results_v<Experiment<FirstTest>>)
    {
        auto results = conduct_experiment_ensemble_impl2<Experiment,Verify>(FirstTest{},std::forward<RuntimeParams>(params)...);
        ((results.merge(conduct_experiment_ensemble_impl2<Experiment,Verify>(Tests{},std::forward<RuntimeParams>(params)...)),0),...);
        return results;
    } 
    else 
    {
        ((conduct_experiment_ensemble_impl2<Experiment,Verify>(Tests{},std::forward<RuntimeParams>(params)...),0),...);
    }
}

template<template<typename...> typename Experiment, bool Verify, template<typename...> typename Packer, typename... RuntimeParams>
decltype(auto) conduct_experiment_ensemble_impl(Packer<>, RuntimeParams&&... params)
{
    return conduct_experiment_runtime_ensemble<Experiment<>,Verify>(std::forward<RuntimeParams>(params)...);
}

}

template<template<typename...> typename Experiment, bool Verify, typename... Params, typename... RuntimeParams>
decltype(auto) conduct_experiment_ensemble(RuntimeParams&&... params)
{
    using ensemble = typename expand_template_collection<Params...>::type;
    return impl::conduct_experiment_ensemble_impl<Experiment,Verify>(ensemble{},std::forward<RuntimeParams>(params)...);
}

}

#endif //NERSC__CL_TEST_FRAMEWORK_HPP
