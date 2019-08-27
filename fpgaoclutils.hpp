#ifndef NERSC__FPGAOCLUTILS_HPP
#define NERSC__FPGAOCLUTILS_HPP

#include <type_traits>
#include <CL/cl2.hpp>
#include <cxxabi.h>

namespace nersc {

namespace detail
{

template<typename T, typename Template>
struct prepend_template;

template<typename T, template<typename...> typename Template, typename... Ts>
struct prepend_template<T,Template<Ts...>>
{
    using type = Template<T,Ts...>;
};

template<typename T, typename Template>
using prepend_template_type = typename prepend_template<T,Template>::type;

}

template<typename T, size_t N = 1, size_t Count = N>
struct duplicate;

/**
 * Duplicates a type N times specified in the duplicate tag.
 */

template<template<typename...> class KernelClass, typename...Ts>
struct template_pack_type_duplicator;

template<template<typename...> class KernelClass, typename...Ts>
using template_pack_type_duplicator_type = typename template_pack_type_duplicator<KernelClass,Ts...>::type;

/**
 * Duplicator that always duplicates N times
 */

template<size_t N, template<typename...> class KernelClass, typename...Ts>
struct template_pack_type_n_duplicator;

template<size_t N, template<typename...> class KernelClass, typename...Ts>
using template_pack_type_n_duplicator_type = typename template_pack_type_n_duplicator<N,KernelClass,Ts...>::type;

template<typename T>
struct array_to_duplicate_converter
{
    using type = T;
};

template<typename T, size_t N>
struct array_to_duplicate_converter<T[N]>
{
    using type = duplicate<T,N>;
};

template<typename T>
using array_to_duplicate_converter_type = typename array_to_duplicate_converter<T>::type;

namespace detail {

template<size_t N, typename...Ts>
struct duplicator_index_sequence_impl;

template<size_t N, typename T, typename...Ts>
struct duplicator_index_sequence_impl<N,T,Ts...>
{
    using type = prepend_template_type<std::integral_constant<size_t,N-sizeof...(Ts)-1>,typename duplicator_index_sequence_impl<N,Ts...>::type>;
};

template<size_t N, size_t N2, size_t Count, typename T, typename...Ts>
struct duplicator_index_sequence_impl<N,duplicate<T,N2,Count>,Ts...>
{
    using type = prepend_template_type<std::tuple<std::integral_constant<size_t,N-sizeof...(Ts)-1>,std::integral_constant<std::size_t,N2-Count>>,typename duplicator_index_sequence_impl<N,duplicate<T,N2,Count-1>,Ts...>::type>;
};

template<size_t N, size_t N2, typename T, typename...Ts>
struct duplicator_index_sequence_impl<N,duplicate<T,N2,0>,Ts...>
{
    using type = typename duplicator_index_sequence_impl<N,Ts...>::type;
};

template<size_t N>
struct duplicator_index_sequence_impl<N>
{
    using type = std::tuple<>;
};

template<size_t Dup, size_t N, typename...Ts>
struct duplicator_n_index_sequence_impl;

template<size_t Dup, size_t N, typename T, typename...Ts>
struct duplicator_n_index_sequence_impl<Dup,N,T,Ts...>
{
    using type = prepend_template_type<std::integral_constant<size_t,N-sizeof...(Ts)-1>,typename duplicator_n_index_sequence_impl<Dup,N,Ts...>::type>;
};

template<size_t Dup, size_t N, size_t N2, typename T, typename...Ts>
struct duplicator_n_index_sequence_impl<Dup,N,duplicate<T,N2,N2>,Ts...>
{
    using type = prepend_template_type<std::tuple<std::integral_constant<size_t,N-sizeof...(Ts)-1>,std::integral_constant<std::size_t,0>>,typename duplicator_n_index_sequence_impl<Dup,N,duplicate<T,Dup,Dup-1>,Ts...>::type>;
};

template<size_t Dup, size_t N, size_t N2, size_t Count, typename T, typename...Ts>
struct duplicator_n_index_sequence_impl<Dup,N,duplicate<T,N2,Count>,Ts...>
{
    using type = prepend_template_type<std::tuple<std::integral_constant<size_t,N-sizeof...(Ts)-1>,std::integral_constant<std::size_t,N2-Count>>,typename duplicator_n_index_sequence_impl<Dup,N,duplicate<T,N2,Count-1>,Ts...>::type>;
};

template<size_t Dup, size_t N, size_t N2, typename T, typename...Ts>
struct duplicator_n_index_sequence_impl<Dup,N,duplicate<T,N2,0>,Ts...>
{
    using type = typename duplicator_n_index_sequence_impl<Dup,N,Ts...>::type;
};

template<size_t Dup, size_t N>
struct duplicator_n_index_sequence_impl<Dup,N>
{
    using type = std::tuple<>;
};

}

template<typename... Ts>
struct duplicator_index_sequence
{
    using type = typename detail::duplicator_index_sequence_impl<sizeof...(Ts),Ts...>::type;
};

template<size_t Dup, typename... Ts>
struct duplicator_n_index_sequence
{
    using type = typename detail::duplicator_n_index_sequence_impl<Dup,sizeof...(Ts),Ts...>::type;
};

template<typename... Ts>
using duplicator_index_sequence_type = typename duplicator_index_sequence<Ts...>::type;

template<size_t Dup, typename... Ts>
using duplicator_n_index_sequence_type = typename duplicator_n_index_sequence<Dup,Ts...>::type;

template<typename I>
struct forward_as_duplicated_tuple_getter;

template<size_t I>
struct forward_as_duplicated_tuple_getter<std::integral_constant<size_t,I>>
{
    template<typename Tuple>
    static decltype(auto) get(Tuple&& args)
    {
        return std::get<I>(std::forward<Tuple>(args));
    }
};

template<size_t I, size_t I2>
struct forward_as_duplicated_tuple_getter<std::tuple<std::integral_constant<size_t,I>,std::integral_constant<size_t,I2>>>
{
    template<typename Tuple>
    static decltype(auto) get(Tuple&& args)
    {
        return std::get<I>(std::forward<Tuple>(args))[I2];
    }
};

template<typename...Is, typename Tuple>
decltype(auto) forward_as_duplicated_tuple_impl(std::tuple<Is...>,Tuple&& args)
{
    return std::forward_as_tuple(forward_as_duplicated_tuple_getter<Is>::get(std::forward<Tuple>(args))...);
};

template<typename...Ts>
decltype(auto) forward_as_duplicated_tuple(Ts&&... ts)
{
    return forward_as_duplicated_tuple_impl(duplicator_index_sequence_type<array_to_duplicate_converter_type<std::remove_reference_t<Ts>>...>{},std::forward_as_tuple(std::forward<Ts>(ts)...));
};

template<typename...Ts>
class KernelDuplicateBufferFunctor : public template_pack_type_duplicator_type<cl::KernelFunctor,array_to_duplicate_converter_type<Ts>...>
{
public:
    using kernel_functor_type = template_pack_type_duplicator_type<cl::KernelFunctor,array_to_duplicate_converter_type<Ts>...>;
    KernelDuplicateBufferFunctor(cl::Kernel kernel) : kernel_functor_type(kernel) {}
    KernelDuplicateBufferFunctor(const ::cl::Program& program, const cl::string name, cl_int * err = NULL) : kernel_functor_type(program, name, err) {}
    KernelDuplicateBufferFunctor() = delete;
private:
    template<size_t... Is, typename Tuple>
    cl::Event apply(const cl::EnqueueArgs& args, std::index_sequence<Is...>, Tuple&& ts)
    {
        return kernel_functor_type::operator()(args, std::get<Is>(std::forward<Tuple>(ts))...);
    }

    template<typename Tuple>
    cl::Event apply(const cl::EnqueueArgs& args, Tuple&& ts)
    {
        return apply(args, std::make_index_sequence<std::tuple_size_v<Tuple>>{}, std::forward<Tuple>(ts));
    }

    template<size_t... Is, typename Tuple>
    cl::Event apply(const cl::EnqueueArgs& args, std::index_sequence<Is...>, Tuple&& ts, cl_int& error)
    {
        return kernel_functor_type::operator()(args, std::get<Is>(std::forward<Tuple>(ts))..., error);
    }

    template<typename Tuple>
    cl::Event apply(const cl::EnqueueArgs& args, Tuple&& ts, cl_int& error)
    {
        return apply(args, std::make_index_sequence<std::tuple_size_v<Tuple>>{}, std::forward<Tuple>(ts), error);
    }

public:

    template<typename...T2s>
    cl::Event operator()(const cl::EnqueueArgs &args, T2s&&...ts)
    {
        return apply(args,forward_as_duplicated_tuple(std::forward<T2s>(ts)...));
    }

    template<typename...T2s>
    cl::Event operator()(const cl::EnqueueArgs &args, T2s&&...ts, cl_int &error)
    {
        return apply(args,forward_as_duplicated_tuple(std::forward<T2s>(ts)...),error);
    }

};

template<typename T, size_t N, size_t Count>
struct duplicate
{
    using type = T;
};

template<template<typename...> class KernelClass, typename T, typename... Ts>
struct template_pack_type_duplicator<KernelClass,T,Ts...>
{
    using type = detail::prepend_template_type<T, typename template_pack_type_duplicator<KernelClass,Ts...>::type>;
};

template<template<typename...> class KernelClass, typename T, size_t N, size_t Count, typename...Ts>
struct template_pack_type_duplicator<KernelClass,duplicate<T,N,Count>,Ts...>
{
    using type = detail::prepend_template_type<T,typename template_pack_type_duplicator<KernelClass,duplicate<T,N,Count-1>,Ts...>::type>;
};

template<template<typename...> class KernelClass, typename T, size_t N, typename...Ts>
struct template_pack_type_duplicator<KernelClass,duplicate<T,N,0>,Ts...>
{
    using type = typename template_pack_type_duplicator<KernelClass,Ts...>::type;
};

template<template<typename...> class KernelClass>
struct template_pack_type_duplicator<KernelClass>
{
    using type = KernelClass<>;
};

template<size_t N, template<typename...> class KernelClass, typename T, typename... Ts>
struct template_pack_type_n_duplicator<N,KernelClass,T,Ts...>
{
    using type = detail::prepend_template_type<T, typename template_pack_type_n_duplicator<N,KernelClass,Ts...>::type>;
};

template<size_t N, template<typename...> class KernelClass, typename T, size_t N2, size_t Count, typename...Ts>
struct template_pack_type_n_duplicator<N,KernelClass,duplicate<T,N2,Count>,Ts...>
{
    using type = detail::prepend_template_type<T,typename template_pack_type_n_duplicator<N,KernelClass,duplicate<T,N2,Count-1>,Ts...>::type>;
};

template<size_t N, template<typename...> class KernelClass, typename T, size_t N2, typename...Ts>
struct template_pack_type_n_duplicator<N,KernelClass,duplicate<T,N2,N2>,Ts...>
{
    using type = detail::prepend_template_type<T,typename template_pack_type_n_duplicator<N,KernelClass,duplicate<T,N,N-1>,Ts...>::type>;
};

template<size_t N, template<typename...> class KernelClass, typename T, size_t N2, typename...Ts>
struct template_pack_type_n_duplicator<N,KernelClass,duplicate<T,N2,0>,Ts...>
{
    using type = typename template_pack_type_n_duplicator<N,KernelClass,Ts...>::type;
};

template<size_t N, template<typename...> class KernelClass>
struct template_pack_type_n_duplicator<N,KernelClass>
{
    using type = KernelClass<>;
};

}

#endif //NERSC__FPGAOCLUTILS_HPP
