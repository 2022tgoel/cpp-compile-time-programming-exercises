// https://www.slamecka.cz/posts/2021-03-17-cpp-metaprogramming-exercises-1/
//
// This is a set of exercises for type level programming in C++, suitable for
// beginners or as a refresher for intermediate programmers.
//
// Scope:
// - Most exercises in this set are confined to vectors of integers for simplicity.
// - Exercises are centered around basic functional techniques since they are easy to express with template metaprogramming.
//
// How to solve it:
// - You should be able to compile with gcc version 10.2, g++ --std=c++20 typelevel_set1.cpp
// - You should understand the requested behaviours from static_assert-s,
//   except for print, which comes with a test in main().
// - Tests are commented-out and you should uncomment them as you go.
// - You might want to read some introductory material before attempting these:
//
//   https://en.cppreference.com/w/cpp/language/templates
//
//   https://en.cppreference.com/w/cpp/language/parameter_pack
//
//   https://en.cppreference.com/w/cpp/language/template_parameters
//      in this set we'll only see non-type template parameters and type template parameters
//
//   https://en.cppreference.com/w/cpp/header/type_traits
//
//   https://en.cppreference.com/w/cpp/language/partial_specialization
//      also see https://stackoverflow.com/questions/17005985/what-is-the-partial-ordering-procedure-in-template-deduction
//

#include <iostream>
#include <type_traits>
#include <array>
#include <cstddef>
#include <iostream>
#include <tuple>
#include <utility>

namespace exer {

/**
 * 1. Define Vector, a template level list of integers.
 * Hint: Use non-type template parameter pack.
 */

// Your code goes here:

template <int... Elements>
struct Vector {};

static_assert(std::is_same_v<Vector<1,2>, Vector<1,2>>);


/**
 * 2. Define function print() that prints Vector-s.
 * Example: print(Vector<1,2,3>{}); // prints "1 2 3" (newline)
 * See main() below.
 */

// Your code goes here:

template <int H, int... T>
void print(Vector<H, T...> vec) {
    std::cout << H << " ";
    print(Vector<T...>{});
}

template <int... T>
void print(Vector<T...> vec) {
    std::cout << std::endl;
}


/**
 * 3. Define Prepend.
 * Hint: Use `using type = ...` inside a struct that has both non-type and type template parameters.
 */

// Your code goes here:

template <int H, typename T>
struct Prepend {
};

template <int H, int... T>
struct Prepend<H, Vector<T...>> {
    using type = Vector<H, T...>;
};


static_assert(std::is_same_v<Prepend<1, Vector<2,3>>::type, Vector<1,2,3>>);


/**
 * 4. Define PrependT that can be used without ::type.
 * Hint: See how enable_if_t is defined in terms of enable_if.
 *
 * This technique is not used further to reduce boilerplate.
 */

// Your code goes here:
template <int H, typename T>
using PrependT = Prepend<H, T>::type;

static_assert(std::is_same_v<PrependT<1, Vector<2,3>>, Vector<1,2,3>>);


/**
 * 5. Define Append.
 */

// Your code goes here:
template <int H, typename T>
struct Append {
};

template <int H, int... T>
struct Append<H, Vector<T...>> {
    using type = Vector<T..., H>;
};

static_assert(std::is_same_v< Append<4, Vector<1,2,3>>::type , Vector<1,2,3,4> >);


/**
 * 6. Define PopBack.
 */

// Your code goes here:

template <typename T>
struct PopBack {
    using type = void;
};

template <int H>
struct PopBack<Vector<H>> {
    using type = Vector<>; // Remove the last element
};

template <int H,int... T>
struct PopBack<Vector<H, T...>> {
    using type = PrependT<H, typename PopBack<Vector<T...>>::type>; // Remove the last element
};

static_assert(std::is_same_v< PopBack<Vector<1,2,3,4>>::type , Vector<1,2,3> >);


/**
 * 7. Define RemoveFirst, that removes the first occurence of element R from vector V.
 */

// Your code goes here:
template <int R, typename T>
struct RemoveFirst {
    using type = void;
};

template <bool b, int R, typename T>
struct RemoveFirstEvaluated {
    using type = void;
};

template <int R, int H, int... T>
struct RemoveFirstEvaluated<true, R, Vector<H, T...>> {
    using type = Vector<T...>;
};

template <int R, int H, int... T>
struct RemoveFirstEvaluated<false, R, Vector<H, T...>> {
    using type = PrependT<H, typename RemoveFirst<R, Vector<T...>>::type>;
};

template <int R, int H, int... T>
struct RemoveFirst<R, Vector<H, T...>> {  
    using type = RemoveFirstEvaluated<R == H, R, Vector<H, T...>>::type;
};


static_assert(std::is_same_v<RemoveFirst<1, Vector<1,1,2>>::type, Vector<1,2>>);
static_assert(std::is_same_v<RemoveFirst<1, Vector<2,1,1,2>>::type, Vector<2,1,2>>);


/**
 * 8. Define RemoveAll, that removes all occurences of element R from vector V.
 */

// Your code goes here:
template <int R, typename T>
struct RemoveAll {
    using type = Vector<>;
};

template <bool b, int R, typename T>
struct RemoveAllEvaluated {
    using type = Vector<>;
};

template <int R, int H, int... T>
struct RemoveAllEvaluated<true, R, Vector<H, T...>> {
    using type = typename RemoveAll<R, Vector<T...>>::type;
};

template <int R, int H, int... T>
struct RemoveAllEvaluated<false, R, Vector<H, T...>> {
    using type = PrependT<H, typename RemoveAll<R, Vector<T...>>::type>;
};

template <int R, int H, int... T>
struct RemoveAll<R, Vector<H, T...>> {  
    using type = RemoveAllEvaluated<R == H, R, Vector<H, T...>>::type;
};

static_assert(std::is_same_v<RemoveAll<9, Vector<1,9,2,9,3,9>>::type, Vector<1,2,3>>);


/**
 * 9. Define Length.
 * Hint: Use `static constexpr int value = ...` inside the struct.
 */

// Your code goes here:
template <typename T>
struct Length;

template <>
struct Length<Vector<>> {
    static constexpr int value = 0;
};

template <int H, int... T>
struct Length<Vector<H, T...>> {
    static constexpr int value = 1 + Length<Vector<T...>>::value;
};

static_assert(Length<Vector<1,2,3>>::value == 3);


/**
 * 10. Define length, which works like Length<V>::value.
 * Hint: See how is_same_v is defined in terms of is_same.
 */

// Your code goes here:
template <typename T>
constexpr int length = Length<T>::value;

static_assert(length<Vector<>> == 0);
static_assert(length<Vector<1,2,3>> == 3);


/**
 * 11. Define Min, that stores the minimum of a vector in its property `value`.
 */

// Your code goes here:
template <typename T>
struct Min;

template <int T>
struct Min<Vector<T>> {
    static constexpr int value = T;
};

template <int H, int... T>
struct Min<Vector<H, T...>> {
    static constexpr int value = (H < Min<Vector<T...>>::value) ? H : Min<Vector<T...>>::value;
};


static_assert(Min<Vector<3,1,2>>::value == 1);
static_assert(Min<Vector<1,2,3>>::value == 1);
static_assert(Min<Vector<3,2,1>>::value == 1);


/**
 * 12. Define Sort.
 */

// Your code goes here:

template <typename T>
struct Sort;

template <>
struct Sort<Vector<>> {
    using type = Vector<>;
};


template <int... T>
struct Sort<Vector<T...>> {
    static constexpr int min = Min<Vector<T...>>::value;
    using RestOfArr = typename RemoveFirst<min, Vector<T...>>::type;
    using type = PrependT<min, typename Sort<RestOfArr>::type>;
};

static_assert(std::is_same_v<Sort<Vector<4,1,2,5,6,3>>::type, Vector<1,2,3,4,5,6>>);
static_assert(std::is_same_v<Sort<Vector<3,3,1,1,2,2>>::type, Vector<1,1,2,2,3,3>>);
static_assert(std::is_same_v<Sort<Vector<2,2,1,1,3,3>>::type, Vector<1,1,2,2,3,3>>);


/**
 * 13. Define Uniq.
 */

// Your code goes here:
// ^ Your code goes here

// static_assert(std::is_same_v<Uniq<Vector<1,1,2,2,1,1>>::type, Vector<1,2,1>>);


/**
 * 14. Define type Set.
 */

// Your code goes here:
// ^ Your code goes here

// static_assert(std::is_same_v<Set<2,1,3,1,2,3>::type, Set<1,2,3>::type>);


/**
 * 15. Define SetFrom.
 */

// Your code goes here:
// ^ Your code goes here

// static_assert(std::is_same_v<SetFrom<Vector<2,1,3,1,2,3>>::type, Set<1,2,3>::type>);


/**
 * 16. Define Get for Vector.
 * Provide an improved error message when accessing outside of Vector bounds.
 */

// Your code goes here:
template <int I, typename T>
struct Get;

template <int H, int... T>
struct Get<0, Vector<H, T...>> {
    using vec = Vector<H, T...>;
    static_assert(0 < length<vec>, "Index is out of bounds");
    static constexpr int value = H;
};

template <int I, int H, int... T>
struct Get<I, Vector<H, T...>> {
    using vec = Vector<H, T...>;
    static_assert(I < length<vec>, "Index is out of bounds");
    static constexpr int value = Get<I - 1, Vector<T...>>::value;
};

static_assert(Get<0, Vector<0,1,2>>::value == 0);
static_assert(Get<1, Vector<0,1,2>>::value == 1);
static_assert(Get<2, Vector<0,1,2>>::value == 2);
// static_assert(Get<9, Vector<0,1,2>>::value == 2); // How good is your error message?


/**
 * 17. Define BisectLeft for Vector.
 * Given n and arr, return the first index i such that arr[i] >= n.
 * If it doesn't exist, return the length of the array.
 *
 * Don't worry about complexity but aim for the binary search pattern.
 *
 * Hint: You might find it convenient to define a constexpr helper function.
 */

// Your code goes here:
// ^ Your code goes here

// static_assert(BisectLeft<  3, Vector<0,1,2,3,4>>::value == 3);
// static_assert(BisectLeft<  3, Vector<0,1,2,4,5>>::value == 3);
// static_assert(BisectLeft<  9, Vector<0,1,2,4,5>>::value == 5);
// static_assert(BisectLeft< -1, Vector<0,1,2,4,5>>::value == 0);
// static_assert(BisectLeft<  2, Vector<0,2,2,2,2,2>>::value == 1);


/**
 * 18. Define Insert for Vector, it should take position, value and vector.
 * Don't worry about bounds.
 * Hint: use the enable_if trick, e.g.
 *   template<typename X, typename Enable = void> struct Foo;
 *   template<typename X> struct<std::enable_if_t<..something      about X..>> Foo {...};
 *   template<typename X> struct<std::enable_if_t<..something else about X..>> Foo {...};
 */

// Your code goes here:
// ^ Your code goes here

// static_assert(std::is_same_v<Insert<0, 3, Vector<4,5,6>>::type, Vector<3,4,5,6>>);
// static_assert(std::is_same_v<Insert<1, 3, Vector<4,5,6>>::type, Vector<4,3,5,6>>);
// static_assert(std::is_same_v<Insert<2, 3, Vector<4,5,6>>::type, Vector<4,5,3,6>>);
// static_assert(std::is_same_v<Insert<3, 3, Vector<4,5,6>>::type, Vector<4,5,6,3>>);


/** 
 * 19. (of my own design) Make an std::array of length N.
 */

template <typename T, T... ints>
constexpr auto make_arr(std::integer_sequence<T, ints...> seq) {
    return std::array<T, sizeof...(ints)>{{ints...}};
}

template <int n>
constexpr auto make_arr() {
    return make_arr(std::make_integer_sequence<int, n>{});
}

/** 
 * 20. (of my own design) Mimic how enable_if_t works
 */
template <typename A, typename B>
struct is_same {
    static constexpr bool value = false;
};

template <typename A>
struct is_same<A, A> {
    static constexpr bool value = true;
};

template <typename A, typename B>
static constexpr bool is_same_v = is_same<A, B>::value;

template <bool b, typename T>
struct enable_if {};

template <typename T>
struct enable_if<true, T> {
    using type = T;
};

template <bool b, typename T = void>
using enable_if_t = enable_if<b, T>::type;

// Now verify with some SFINAE
template <int I, typename Op, typename = void> 
struct Int {
    static constexpr int value = I;
};

// To be honest, enable_if_t is pretty useless in 
// this example. Maybe I'll come up with a better 
// example later. 

struct AddOneType {};
struct SubOneType {};

template <int I, typename Op>
struct Int<I, Op, enable_if_t<is_same_v<Op, AddOneType>>> {
    static constexpr int value = I + 1;
};

template <int I, typename Op>
struct Int<I, Op, enable_if_t<is_same_v<Op, SubOneType>>> {
    static constexpr int value = I - 1;
};

static_assert(Int<3, AddOneType>::value == 4);

}

/** 
 * Weird stuff with delctype.
 * If you do decltype(E) it  tells you the type of the with 
 * which the variable, field, or non-type template parameter was declared.
 * If you do decltype((E)), it tells you the type of the expressoin.
 * For example, if you have 
 * int i = 5;
 * decltype(i) is int,
 * but decltype((i)) is int& (lvalue reference to int)
 * because you can do something like int& x = i;
 */

template<typename T> constexpr const char *category = "prvalue";
template<typename T> constexpr const char *category<T&> = "lvalue";
template<typename T> constexpr const char *category<T&&> = "xvalue";

#define SHOW(E) std::cout << #E << ": " << category<decltype((E))> << std::endl

decltype(auto)
fn_A(int i) {
    return i;
}

decltype(auto)
fn_B(int& i) {
    return (i);
}

struct S {
    int i;
};

decltype(auto)
fn_C(int i) {
    S thing{i};
    return (thing.i);
}

decltype(auto)
fn_D(int i) {
    return (S{i}.i);
}

int main()
{
    int i = 6;
    SHOW(fn_A(0));
    SHOW(fn_B(i));
    SHOW(fn_C(0));
    SHOW(fn_D(0));
    fn_B(i) = 7;
    std::cout << i << std::endl;
    using namespace exer;
    print(Vector<>{});
    print(Vector<1>{});
    print(Vector<1,2,3,4,5,6>{});
    std::cout << typeid(Vector<1,2,3,4,5,6>{}).name() << '\n';

    auto arr = make_arr<3>();
    std::cout << "{" << arr[0] << ", " << arr[1] << ", " << arr[2] << "}" << std::endl; 
}