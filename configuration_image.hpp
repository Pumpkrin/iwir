//
//File      : configuration_image.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 04/09/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef configuration_image_h
#define configuration_image_h

#include "constexpr_string.hpp"

#include <tuple>
#include <cstring>
#include <iostream>
#include <vector>

namespace iwir {
    
    //------------------------------entry----------------------------------------
    
    struct size {
        double size;
        constexpr double& value()       { return size; }
        constexpr double  value() const { return size; }
        std::string content() const { return details::concatenate( "size:=", std::to_string(size) );  }
    };
    
    struct style {
        int style;
        constexpr int& value()       { return style; }
        constexpr int  value() const { return style; }
        std::string content() const { return details::concatenate( "style:=", std::to_string(style) );  }
    };
    
    struct width {
        int width;
        constexpr int& value()       { return width; }
        constexpr int  value() const { return width; }
        std::string content() const { return details::concatenate( "width:=", std::to_string(width) );  }
    };
    
    struct offset {
        double offset;
        constexpr double& value()       { return offset; }
        constexpr double  value() const { return offset; }
        std::string content() const { return details::concatenate( "offset:=", std::to_string(offset) );  }
    };
    
    struct plain_text {
        std::string data;
        std::string plain_data() const { return data; }
        std::string      &  value()       { return data; }
        std::string const&  value() const { return data; }
        std::string content() const { return details::concatenate( "plain_text:=", data);  }
    };
    
    struct user_text {
        std::string data;
//        std::string user_data() const { return std::string{data.begin()+1, data.end()-1}; }
        std::string user_data() const { return data; }
        std::string      &  value()       { return data; }
        std::string const&  value() const { return data; }
        std::string content() const { return details::concatenate( "user_text:=[", data, "]");  }
    };
    
    struct low {
        double low;
        constexpr double& value(){ return low; }
        constexpr double const& value() const{ return low; }
        std::string content() const { return details::concatenate( "low:=", std::to_string(low) );  }
    };
    
    struct high {
        double high;
        constexpr double& value(){ return high; }
        constexpr double const& value() const{ return high; }
        std::string content() const { return details::concatenate( "high:=", std::to_string(high) );  }
    };
    
    struct color {
        int color;
        constexpr int& value(){ return color; }
        constexpr int const& value() const{ return color; }
        std::string content() const { return details::concatenate( "color:=", std::to_string(color) );  }
    };
    
    
    
    
    //------------------------------field----------------------------------------
    
    
    template<class Derived>
    struct filler {
        template<class ... Ts, class ...Us> //match with Ts ?
        constexpr void fill( Us... us_p ){
            int expander[] = { 0, (static_cast<Ts&>(derived()).value() = us_p, void(), 0) ... };
        }
        
    private:
        Derived& derived() { return static_cast<Derived&>(*this);}
        Derived const& derived() const {return static_cast<Derived const&>(*this);}
    };
    
    template<class Derived, class ... Ts>
    struct field_formatter {
        std::string retrieve_content() const {
            return details::concatenate_with_separator(";",  static_cast<Ts const&>(derived()).content()... );
        }
        
    private:
        Derived& derived() { return static_cast<Derived&>(*this);}
        Derived const& derived() const {return static_cast<Derived const&>(*this);}
    };
    
    
    struct x{ static constexpr auto anchor = details::make_constexpr_stringview("_x"); };
    struct y{ static constexpr auto anchor = details::make_constexpr_stringview("_y"); };
    struct none{ static constexpr auto anchor = details::make_constexpr_string(""); };
    struct single{ static constexpr auto anchor = details::make_constexpr_string(""); };
    struct multiple{ static constexpr auto anchor = details::make_constexpr_string(""); };
    
    template<class T> struct field;
    template<class T> struct single_value_field;
    template<class T> struct multiple_value_field;
    
    template<class T>
    struct field_traits{
        using value_type = single_value_field<T>;
        using is_silent = std::false_type;
    };
    
    
    template< class T >
    struct label : size, offset,
                   filler<label<T>>,
                   field_formatter<label<T>, size, offset> {
        static constexpr decltype("label" + T::anchor) anchor = "label" + T::anchor;
    };
    
    template< class T >
    struct range : low, high,
                   filler<range<T>>,
                   field_formatter<range<T>, low, high> {
        static constexpr decltype("range" + T::anchor) anchor = "range" + T::anchor;
    };
    
    template< class T >
    struct title : size, offset, user_text,
                   filler<title<T>>,
                   field_formatter<title<T>, size, offset, user_text> {
        static constexpr decltype("title" + T::anchor) anchor = "title" + T::anchor;
    };
    
    struct marker : size, style, color,
                    filler<marker>,
                    field_formatter<marker, size, style, color> {
        static constexpr details::constexpr_string<6> anchor = details::make_constexpr_string("marker");
    };
    
    
    struct line : width, style, color,
                  filler<line>,
                  field_formatter<line, width, style, color> {
        static constexpr details::constexpr_string<4> anchor = details::make_constexpr_string("line");
    };
    
    
    struct legend_attributes : user_text, plain_text,
                               filler<legend_attributes >,
                               field_formatter<legend_attributes, user_text, plain_text > {
        static constexpr details::constexpr_string<17> anchor = details::make_constexpr_string("legend_attributes");
    };
    
    
    struct option : plain_text,
                    filler<option>,
                    field_formatter<option, plain_text> {
        static constexpr details::constexpr_string<6> anchor = details::make_constexpr_string("option");
    };
    
    template<class T>
    struct header : user_text, size, color,
                    filler<header<T>>,
                    field_formatter<header<T>, user_text, size, color> {
        static constexpr details::constexpr_string<6> anchor = details::make_constexpr_string("header");
    };
    
    
    template<> struct field_traits< header<single> >
    {
        using value_type = single_value_field< header<single> >;
        using is_silent = std::false_type;
    };
    template<> struct field_traits< header<multiple> >
    {
        using value_type = multiple_value_field< header<multiple> >;
        using is_silent = std::false_type;
    };
    
    
    struct name : plain_text,
                  filler<name> {};
    
    template<> struct field_traits< name >
    {
        using value_type = single_value_field< name >;
        using is_silent = std::true_type;
    };
    
    
    //------------------------------element----------------------------------------
    
    template<class T> struct element;
    template<class T> struct single_value_element;
    template<class T> struct multiple_value_element;
    template<class T>
    struct element_traits{
        using value_type = single_value_element<T>;
        using is_single_value = std::true_type;
    };
    
    
    
    
    struct histogram1d{
        static constexpr details::constexpr_string<6> anchor = details::make_constexpr_string("hist1d");
        using fields = std::tuple< name, legend_attributes, option, marker, line >;
    };
    
    template<>
    struct element_traits<histogram1d>{
        using value_type = multiple_value_element<histogram1d>;
        using is_single_value = std::false_type;
    };
    
    //    struct histogram2d{
    //        static constexpr details::constexpr_string anchor = "h2";
    //        using fields = std::tuple< draw_option >;
    //    };
    
    
    struct frame1d{
        static constexpr details::constexpr_string<7> anchor = details::make_constexpr_string("frame1d");
        using fields = std::tuple< title<x>, range<x>, label<x>, title<y>, range<y>, label<y>>;
    };
    
    struct pad{
        static constexpr details::constexpr_string<3> anchor = details::make_constexpr_string("pad");
        using fields = std::tuple< range<x>, range<y>>;
    };
    
    struct legend{
        static constexpr details::constexpr_string<6> anchor = details::make_constexpr_string("legend");
        using fields = std::tuple< header<single>, range<x>, range<y> >;
    };
    
    struct pave_text{
        static constexpr details::constexpr_string<9> anchor = details::make_constexpr_string("pave_text");
        using fields = std::tuple< header<multiple>, range<x>, range<y>>;
    };
    template<>
    struct element_traits<pave_text>{
        using value_type = multiple_value_element<pave_text>;
        using is_single_value = std::false_type;
    };
    
    
    ////------------------------------------------conversion------------------------------------------
    
    
    template<template<class> class, class ...> struct convert;
    template< template<class> class Converter, class ... Ts>
    struct convert< Converter, std::tuple<Ts...> > {
        using type_list = typename std::tuple< Converter<Ts>... >;
    };
    template< template<class> class Converter, class T, class ... Ts>
    struct convert< Converter, T, std::tuple<Ts...> > {
        using type_list = typename std::tuple< Converter<T>, Converter<Ts>... >;
    };
    
    template<class ...Ts>
    struct convert< field, std::tuple<Ts...> >{
        using type_list = typename std::tuple< typename field_traits<Ts>::value_type... >;
    };
    template<class ...Ts>
    struct convert< element, std::tuple<Ts...> >{
        using type_list = typename std::tuple< typename element_traits<Ts>::value_type... >;
    };
    template<class T, class ...Ts>
    struct convert< element, T, std::tuple<Ts...> >{
        using type_list = typename std::tuple<typename element_traits<T>::value_type, typename element_traits<Ts>::value_type... >;
    };
    
    
    
    
    //------------------------------------------field------------------------------------------
    
    template<class T>
    struct field {
        
    private:
        template<class T_ = T, typename std::enable_if_t< !field_traits<T_>::is_silent::value, std::nullptr_t> = nullptr >
        std::string retrieve_content_impl() const {
            return details::concatenate( "\n<", T::anchor, ">", data_m.retrieve_content(), "<", T::anchor, ">" );
        }
        
        template<class T_ = T, typename std::enable_if_t< field_traits<T_>::is_silent::value, std::nullptr_t> = nullptr >
        std::string retrieve_content_impl() const {
            return std::string{};
        }
        
    public:
        std::string retrieve_content() const {
            return retrieve_content_impl();
        }
        
        template<class ...Entries, class ...Values>
        constexpr void fill(Values&&... vs_p){
            int expander[] = { 0, (data_m.template fill<Entries>( std::forward<Values>(vs_p) ), void(), 0) ... };
        }
        
        T& retrieve() {return data_m;}
        T const& retrieve() const {return data_m;}
        
    private:
        T data_m;
    };
    
    
    template<class T>
    struct single_value_field {
        std::string retrieve_content() const {
            return field_m.retrieve_content() ;
        }
        
        template<class ... Entries, class ... Values >
        void fill( Values&& ... vs_p ){
            int expander[] = { 0, (field_m.template fill<Entries>( std::forward<Values>(vs_p) ), void(), 0) ... };
        }
        
        T& retrieve() {return field_m.retrieve();}
        T const& retrieve() const {return field_m.retrieve();}
        
    private:
        field<T> field_m;
    };
    
    
    
    template<class T>
    struct multiple_value_field {
        std::string retrieve_content() const {
            std::string result;
            for( auto const& value : value_mc ){
                result += value.retrieve_content() ;
            }
            return result ;
        }
        
        field<T> & operator[](std::size_t index_p){ return value_mc[index_p]; }
        field<T> const& operator[](std::size_t index_p) const { return value_mc[index_p]; }
        
        field<T> & add_value(){ value_mc.push_back( field<T>{}); return value_mc.back(); }
        
        auto begin() { return value_mc.begin(); }
        auto begin() const { return value_mc.begin(); }
        auto end() { return value_mc.end(); }
        auto end() const { return value_mc.end(); }
        
    private:
        std::vector< field<T> > value_mc;
    };
    
    
    
    //------------------------------------------element------------------------------------------
    
    template<class T>
    struct element{
        using field_tuple = typename convert<field, typename T::fields>::type_list;
        
    private:
        template<std::size_t ... Indices>
        std::string retrieve_content_impl( std::index_sequence<Indices...> ) const {
            return details::concatenate(
                                        "<", T::anchor, ">",
                                        std::get<Indices>(field_mc).retrieve_content()...,
                                        "\n<", T::anchor, ">"
                                        );
        }
        
    public:
        std::string retrieve_content() const {
            return retrieve_content_impl( std::make_index_sequence< std::tuple_size<field_tuple>::value >{});
        }
        
        template<class Field>
        constexpr typename field_traits<Field>::value_type const& retrieve_field() const{
            return std::get< typename field_traits<Field>::value_type >(field_mc);
        }
        template<class Field>
        constexpr typename field_traits<Field>::value_type & retrieve_field() {
            return std::get< typename field_traits<Field>::value_type >(field_mc);
        }
        
    private:
        field_tuple field_mc;
    };
    
    
    template<class T>
    struct single_value_element {
        
        std::string retrieve_content() const {
            return value_m.retrieve_content();
        }
        
        template<class Field>
        typename field_traits<Field>::value_type const& retrieve_field() const { return value_m.template retrieve_field<Field>(); }
        
        template<class Field>
        typename field_traits<Field>::value_type & retrieve_field() { return value_m.template retrieve_field<Field>(); }
        
    private:
        element<T> value_m;
    };
    
    template<class T>
    struct multiple_value_element {
        std::string retrieve_content() const {
            std::string result;
            for( auto const& value : value_mc ){
                result += '\n' + value.retrieve_content();
            }
            return result ;
        }
        
        element<T> & operator[](std::size_t index_p){ return value_mc[index_p]; }
        element<T> const& operator[](std::size_t index_p) const { return value_mc[index_p]; }
        
        element<T> & add_value(){ value_mc.emplace_back( element<T>{} ); return value_mc.back(); }
        
        auto begin() { return value_mc.begin(); }
        auto begin() const { return value_mc.begin(); }
        auto end() { return value_mc.end(); }
        auto end() const { return value_mc.end(); }
        
    private:
        std::vector< element<T> > value_mc;
    };
    
    
    
    //------------------------------------------image------------------------------------------
    
    
    template<class ... Ts>
    struct configuration {
        using elements = std::tuple<pad, Ts...>;
    };
    
    
    template<class Configuration>
    struct image {
        using element_tuple = typename convert< element, typename Configuration::elements>::type_list;
        
    private:
        template<std::size_t ... Indices>
        std::string retrieve_content_impl( std::index_sequence<Indices...>) {
            return details::concatenate( std::get<Indices>(element_mc).retrieve_content()... );
        }
        
    public:
        std::string retrieve_content() {
            return retrieve_content_impl( std::make_index_sequence< std::tuple_size<element_tuple>::value>{} );
        }
        
        template<class Element>
        constexpr auto & retrieve_element() {
            return std::get< typename element_traits< Element >::value_type >(element_mc);
        }
        template<class Element>
        constexpr auto const& retrieve_element() const {
            return std::get< typename element_traits< Element >::value_type >(element_mc);
        }
        
    private:
        element_tuple element_mc;
    };
    
    
    template<class Configuration>
    constexpr auto make_image() -> image<Configuration> { return {}; }
    
} //namespace iwir

#endif /* configuration_image_h */
