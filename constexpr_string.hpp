//
//File      : constexpr_string.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 24/09/2020
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//

#ifndef constexpr_string_h
#define constexpr_string_h

#include <utility>
#include <string>
#include <cstring>

namespace details{
    
    struct container_tag{};
    struct view_tag{};
    
    
    //------------------------------primary_template----------------------------------------
    template<std::size_t N, class Impl = container_tag >
    struct constexpr_string_impl {
        char const data [N+1];
        
//        template< std::size_t N1, class Tag1,
//                  std::size_t N2, class Tag2>
//        friend constexpr auto operator+( constexpr_string_impl<N1, Tag1> const& lhs_p,
//                                         constexpr_string_impl<N2, Tag2> const& rhs_p);
//
//        template< std::size_t N1,
//                  std::size_t N2, class Tag2>
//        friend constexpr auto operator+( char const (&lhs_p)[N1],
//                                         constexpr_string_impl<N2, Tag2> const& rhs_p);
//
//        template< std::size_t N1, class Tag1,
//                  std::size_t N2>
//        friend constexpr auto operator+( constexpr_string_impl<N1, Tag1> const& lhs_p,
//                                         char const (&rhs_p)[N2]);
        
        
    public:
        template<std::size_t ... Indices>
        constexpr constexpr_string_impl(char const (&s)[N+1], std::index_sequence<Indices...>) : data{ s[Indices]... } {}
        
        template< std::size_t N1, class Tag1, std::size_t ... Indices1,
                  class Tag2, std::size_t ... Indices2>
        constexpr constexpr_string_impl( constexpr_string_impl<N1, Tag1> const& lhs_p,
                                         constexpr_string_impl<N-N1, Tag2> const& rhs_p,
                                         std::index_sequence<Indices1...>,
                                         std::index_sequence<Indices2...> ) :
            data{ lhs_p[Indices1]..., rhs_p[Indices2]... } {}
       
        template< std::size_t N1, std::size_t ... Indices1,
                  class Tag2, std::size_t ... Indices2>
        constexpr constexpr_string_impl( char const (&lhs_p)[N1],
                                         constexpr_string_impl<N-N1, Tag2> const& rhs_p,
                                         std::index_sequence<Indices1...>,
                                         std::index_sequence<Indices2...> ) :
            data{ lhs_p[Indices1]..., rhs_p[Indices2]... } {}
       
        template< std::size_t N1, class Tag1, std::size_t ... Indices1,
                  std::size_t ... Indices2>
        constexpr constexpr_string_impl( constexpr_string_impl<N1, Tag1> const& lhs_p,
                                         char const (&rhs_p)[N-N1],
                                         std::index_sequence<Indices1...>,
                                         std::index_sequence<Indices2...> ) :
            data{ lhs_p[Indices1]..., rhs_p[Indices2]... } {}
        
        
    public:
        constexpr explicit constexpr_string_impl(char const (&s)[N+1]) : constexpr_string_impl(s, std::make_index_sequence<N+1>{}) {}
        
        constexpr std::size_t size() const { return N; }
        
        constexpr char& operator[]( std::size_t index_p )       {return data[index_p];}
        constexpr char  operator[]( std::size_t index_p ) const {return data[index_p];}
        
        constexpr operator char const* const() const { return data; }
    };
    
    //------------------------------container alias and maker----------------------------------------
    template<std::size_t N>
    using constexpr_string = constexpr_string_impl<N, container_tag>;
    
    template<std::size_t N>
    constexpr auto make_constexpr_string( char const (&literal_p)[N] ) {
        return constexpr_string<N-1>{literal_p};
    }
    
    
    //------------------------------operator+----------------------------------------
    template< std::size_t N1, class Tag1,
              std::size_t N2, class Tag2>
    constexpr auto operator+( constexpr_string_impl<N1, Tag1> const& lhs_p,
                              constexpr_string_impl<N2, Tag2> const& rhs_p){
        return constexpr_string<N1+N2>{ lhs_p, rhs_p, std::make_index_sequence<N1>{}, std::make_index_sequence<N2>{} };
    }
    
    template< std::size_t N1,
              std::size_t N2, class Tag2>
    constexpr auto operator+( char const (&lhs_p)[N1],
                              constexpr_string_impl<N2, Tag2> const& rhs_p){
        return constexpr_string<N1+N2>{ lhs_p, rhs_p, std::make_index_sequence<N1-1>{}, std::make_index_sequence<N2>{} };
    }
    
    template< std::size_t N1, class Tag1,
              std::size_t N2>
    constexpr auto operator+( constexpr_string_impl<N1, Tag1> const& lhs_p,
                              char const (&rhs_p)[N2]){
        return constexpr_string<N1+N2>{ lhs_p, rhs_p, std::make_index_sequence<N1>{}, std::make_index_sequence<N2-1>{} };
    }
    
    
    
    

    
    
    //------------------------------partial specialization view----------------------------------------
    template<std::size_t N>
    struct constexpr_string_impl<N, view_tag> {
        char const (&data) [N+1];
        
        constexpr constexpr_string_impl(char const (&s)[N+1]) : data(s) {}
        constexpr std::size_t size() const { return N; }
        
        constexpr char& operator[]( std::size_t index_p )       {return data[index_p];}
        constexpr char  operator[]( std::size_t index_p ) const {return data[index_p];}
        
        constexpr operator char const* const() const { return data; }
    };
    
    template<std::size_t N>
    using constexpr_stringview = constexpr_string_impl<N, view_tag>;
    
    template<std::size_t N>
    constexpr auto make_constexpr_stringview( char const (&literal_p)[N] ) -> constexpr_stringview<N-1> {
        return {literal_p};
    }
    
    
    

    
    
    
    //------------------------------string_size----------------------------------------
    
    template<class T>
    std::size_t string_size(T const & t_p);
    template<class T>
    std::size_t string_size(T* t_p);
    
    
    template<>
    inline std::size_t string_size(char const * input_ph)
    {
        return input_ph ? strlen(input_ph) : 0 ;
    };
    template<>
    inline std::size_t string_size(std::string const & s_p)
    {
        return s_p.size();
    };
    template<std::size_t N>
    inline std::size_t string_size(char (&) [N])
    {
        return N;
    };
    template<std::size_t N, class Tag>
    inline std::size_t string_size(constexpr_string_impl<N, Tag> s_p)
    {
        return s_p.size();
    };

    
    
    template<class T, class ... Ts>
    std::size_t string_size(T const & t_p,  Ts const & ... ts_p )
    {
        return string_size(t_p) + string_size(ts_p...);
    }
    
    template<class T, class U>
    std::size_t string_size( T const & t_p, U const & u_p )
    {
        return string_size(t_p) + string_size(u_p);
    }
    
    
    
    
    
    //------------------------------concatenation----------------------------------------
    
    template<class ... Ts>
    struct concatenate_with_separator_impl;
    
    template<class T, class ... Ts>
    struct concatenate_with_separator_impl<T, Ts...>
    {
        static void apply(std::string & result_p, char const* const separator_p, T && t_p, Ts && ... ts_p )
        {
            result_p += std::forward<T>(t_p);
            result_p += separator_p;
            concatenate_with_separator_impl<Ts...>::apply(result_p, separator_p, std::forward<Ts>(ts_p)...);
        }
    };
    
    template<class T, class U>
    struct concatenate_with_separator_impl<T, U>
    {
        static void apply(std::string & result_p, char const* const separator_p, T && t_p, U && u_p)
        {
            result_p += std::forward<T>(t_p);
            result_p += separator_p;
            result_p += std::forward<U>(u_p);
        }
    };
    
    template<class T>
    struct concatenate_with_separator_impl<T>
    {
        static void apply(std::string & result_p, char const* const /*separator_p*/, T && t_p)
        {
            result_p += std::forward<T>(t_p);
        }
    };
    
    template<class ... Ts>
    std::string concatenate_with_separator( char const* const separator_p, Ts && ... ts_p)
    {
        std::string result;
        result.reserve( string_size(ts_p ...) + (sizeof...(Ts)-1)*string_size(separator_p) );
        concatenate_with_separator_impl<Ts...>::apply(result, separator_p, std::forward<Ts>(ts_p)...);
        return result;
    }
    
    template<class ... Ts>
    std::string concatenate(Ts && ... ts_p)
    {
        return concatenate_with_separator("", std::forward<Ts>(ts_p)...);
    }
    
    template<class T, typename std::enable_if_t< std::is_arithmetic<T>::value  , std::nullptr_t> = nullptr>
    inline std::string to_string(T const& value_p){
        return std::to_string( value_p );
    }
    
    inline std::string to_string(std::string const& value_p){
        return value_p;
    }

} //namespace details

#endif /* constexpr_string_h */
