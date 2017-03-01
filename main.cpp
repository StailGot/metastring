#include <array>
#include <iostream>
#include <type_traits>

template < class T >
constexpr auto V = 42; 

namespace meta::string{

template <uint8_t ... >
struct string_t{};

namespace details{
template <typename T, typename R, size_t I, bool>
struct __get_str;

template <typename T, uint8_t ...Xs, size_t I>
struct __get_str<T, string_t< Xs... >, I, false > { using type = string_t<Xs...>; };

template <typename T, uint8_t ...Xs, size_t I>
struct __get_str<T, string_t< Xs... >, I, true >
{
   using type = typename __get_str<T, string_t<Xs...,T{}.data[I]>, I + 1, T{}.data[I] != '\0' >::type;
};


template <typename T>
struct __length;

template < template < uint8_t ... > class T, uint8_t ... Xs >
struct __length< T< Xs... > >
{
  constexpr static auto value = sizeof...(Xs);
};


namespace io{
template < uint8_t ... Xs >
std::ostream & operator<<( std::ostream & os, meta::string::string_t< Xs ... > )
{
  return [&]{for( auto i : {Xs ...} ) os << i;}(), os;
}
}}

template <typename T>
using get_str_t = typename details::__get_str< T, string_t<>, 0, true>::type;
using details::io::operator<<;

template < typename F >
constexpr auto to_constexpr_string( F && f )
{
  return get_str_t< decltype(f()) >{};
}

template< typename T >
constexpr auto length( T && )
{
  return details::__length<T>::value;
}

}

#define CONST_STRING(str) []{ struct { const char * data = str; } _; return _; }
#define TO_CONST_STRING(str) meta::string::to_constexpr_string( CONST_STRING(str) )

int main()
{
  constexpr auto foo_str = []{ struct { const char * data = "wtf is going here?"; } _; return _; };
  constexpr auto foo = []{ return 42; };

  uint8_t data[foo()] = {};
  static_assert( sizeof data == foo() );
  std::cout << sizeof data << "\n";

  // {
  //   using result = meta::string::get_str_t< decltype(foo_str()) >;
  //   std::cout << result{};
  // }

  {
    uint8_t data[meta::string::length(TO_CONST_STRING("123")) ] {};
    static_assert( sizeof data == 4 );

    std::cout << TO_CONST_STRING("constexpr string") << "\n";
  }

}