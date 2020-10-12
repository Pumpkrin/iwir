//
//File      : flag_set.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 02/10/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef flag_set_h
#define flag_set_h


namespace details{
    template<class ... Ts>
    struct any_of : std::false_type {};
    
    template<class T>
    struct any_of<T> : T  {};
    
    template< class T, class ... Ts >
    struct any_of<T, Ts ...> : std::conditional< bool(T::value), T, any_of<Ts...> >::type {};
} // namespace details

template< class T > struct flag_traits{ using is_authorized = std::false_type; };




//rather than checking type, just check that there is no 2 index that are the same ? flag traits can be removed that way
struct hist1d_flag{
    static constexpr uint8_t shift = 2;
};
template<> struct flag_traits<hist1d_flag>{ using is_authorized = std::true_type; };

struct legend_flag{
    static constexpr uint8_t shift = 1;
};
template<> struct flag_traits<legend_flag>{ using is_authorized = std::true_type; };

struct pave_text_flag{
    static constexpr uint8_t shift = 0;
};
template<> struct flag_traits<pave_text_flag>{ using is_authorized = std::true_type; };









template< class ... Ts >
struct flag_set{
    
    template< class ... Flags>
    friend class flag_set_builder;
    
    
public:
    constexpr operator uint8_t() const { return compute_value(); }
    
    
private:
    flag_set() = default;
    
    constexpr uint8_t compute_value() const {
        uint8_t result = 0;
        int expander[] = { 0, ( result |= 1UL << Ts::shift , void(), 0) ... };
        return result;
        // return 1;
    }
};



template< class ... Ts  >
struct flag_set_builder{
    friend constexpr flag_set_builder<> start_flag_set();
    
    template< class T,
    class AuthorizedFlag = std::enable_if_t< flag_traits<T>::is_authorized::value >,
    typename std::enable_if_t< details::any_of< std::is_same<T, Ts>... >::value , std::nullptr_t> = nullptr >
    constexpr flag_set_builder< Ts...> add(T&& t_p) && {
        return *this;
    }
    
    
    template< class T,
    class AuthorizedFlag = std::enable_if_t< flag_traits<T>::is_authorized::value >,
    typename std::enable_if_t< !details::any_of< std::is_same<T, Ts>... >::value , std::nullptr_t> = nullptr >
    constexpr flag_set_builder<T, Ts...> add(T&& t_p) && {
        return flag_set_builder<T, Ts...>{};
    }
    
    constexpr flag_set<Ts...> finish() && {
        return {};
    }
    
private:
    constexpr flag_set_builder() = default;
};

template<>
struct flag_set_builder<> {
    template< class T,
    class AuthorizedFlag = std::enable_if_t< flag_traits<T>::is_authorized::value > >
    constexpr flag_set_builder<T> add(T&& t_p) && {
        return flag_set_builder<T>{};
    }
    
private:
    constexpr flag_set_builder() = default;
};

constexpr flag_set_builder<> start_flag_set() { return flag_set_builder<>{}; }


#endif /* flag_set_h */
