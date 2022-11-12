#pragma once

#include <tuple>

namespace rtcsdk::impl {

template<typename... Types>
struct vector
{
    using type = vector;
};

template<typename T>
struct size;

template<typename... Types>
struct size<vector<Types...>>
{
    static const size_t value = sizeof...(Types);
};

template<typename Vector, typename T>
struct push_back;

template<typename... Types, typename T>
struct push_back<vector<Types...>, T>
{
    using type = vector<Types..., T>;
};

template<typename Vector, typename T>
using push_back_t = typename push_back<Vector, T>::type;

template<typename Vector, typename T>
struct push_front;

template<typename... Types, typename T>
struct push_front<vector<Types...>, T>
{
    using type = vector<T, Types...>;
};

template<typename Vector, typename T>
using push_front_t = typename push_front<Vector, T>::type;

template<typename A, class B>
struct append;

template<typename... Types, typename B>
struct append<vector<Types...>, B>
{
    using type = vector<Types..., B>;
};

template<typename A, typename... Types>
struct append<A, vector<Types...>>
{
    using type = vector<A, Types...>;
};

template<typename... TypesA, typename... TypesB>
struct append<vector<TypesA...>, vector<TypesB...>>
{
    using type = vector<TypesA..., TypesB...>;
};

template<typename A, typename B>
using append_t = typename append<A, B>::type;

template<typename T>
struct front;

template<typename First, typename... Rest>
struct front<vector<First, Rest...>>
{
    using type = First;
};

template<typename T>
using front_t = typename front<T>::type;

template<typename T>
struct back;

template<typename... First, typename Last>
struct back<vector<First..., Last>>
{
    using type = Last;
};

template<typename T>
using back_t = typename back<T>::type;

template<typename T>
struct remove_front;

template<typename First, typename... Rest>
struct remove_front<vector<First, Rest...>>
{
    using type = vector<Rest...>;
};

template<typename T>
using remove_front_t = typename remove_front<T>::type;

template<typename T>
struct remove_back;

template<typename... First, typename Last>
struct remove_back<vector<First..., Last>>
{
    using type = vector<First...>;
};

template<typename T>
using remove_back_t = typename remove_back<T>::type;

template<typename T>
struct as_tuple;

template<typename... Types>
struct as_tuple<vector<Types...>>
{
    using type = std::tuple<Types...>;
};

template<typename T>
using as_tuple_t = typename as_tuple<T>::type;

}
