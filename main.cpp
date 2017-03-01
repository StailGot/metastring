#include <array>
#include <iostream>

template < class T >
constexpr auto V = 42; 

namespace meta::string{

template <uint8_t ... >
struct string_t{};

namespace details{
template <typename T, typename R, size_t I, bool>
struct get_str;

template <typename T, uint8_t ...Xs, size_t I>
struct get_str<T, string_t< Xs... >, I, false > { using type = string_t<Xs...>; };

template <typename T, uint8_t ...Xs, size_t I>
struct get_str<T, string_t< Xs... >, I, true >
{
   using type = typename get_str<T, string_t<Xs...,T{}.data[I]>, I + 1, T{}.data[I] != '\0' >::type;
};

namespace io
{
  template < uint8_t ... Xs >
  std::ostream & operator<<( std::ostream & os, meta::string::string_t< Xs ... > )
  {
    return [&]{for( auto i : {Xs ...} ) os << i;}(), os;
  }
}
}

template <typename T>
using get_str_t = typename details::get_str< T, string_t<>, 0, true>::type;
using details::io::operator<<;

}



int main()
{
  constexpr auto foo_str = []{ struct _ { const char * data = "wtf is that"; }; return _{}; };
  constexpr auto foo = []{ return 42; };

  uint8_t data[foo()] = {};
  static_assert( sizeof data == foo() );
  std::cout << sizeof data << "\n";

  using result = meta::string::get_str_t< decltype(foo_str()) >;
  std::cout << result{};
}