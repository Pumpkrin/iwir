//
//File      : configurator.cpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 28/09/2020
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//

#include "configurator.hpp"
#include "flag_set.hpp"

#include <fstream>

#include "TDirectory.h"

namespace iwir {

    std::vector< std::string > regex_split( std::string const & text_p, std::regex regex_p ) {
        std::vector<std::string> result_c;
        result_c.reserve(10);
        
        
        std::sregex_iterator match_i{ text_p.begin(), text_p.end(), regex_p };
        auto end_i = std::sregex_iterator{};
        
        
        for(auto iterator = match_i; iterator != end_i ; ++iterator){
            result_c.push_back( iterator->str() );
        }
        
        return result_c;
    }
    
    std::string regex_first_match( std::string const & text_p, std::regex regex_p ) {
        std::sregex_iterator match_i{ text_p.begin(), text_p.end(), regex_p };
        return match_i->str();
    }
    
    std::string remove_outer_tag( std::string text_p ){
        auto tag = regex_first_match( text_p, std::regex{"<[^<>]+>"} );
        text_p.erase( text_p.find(tag), tag.size() );
        text_p.erase( text_p.find(tag), tag.size() );
        return std::move(text_p);
    }
    
    double regex_arithmetic_value( std::string const & text_p, std::regex regex_p ){
        return std::stod( regex_first_match( text_p, std::move(regex_p) ) );
    }
    
    //-------------------------configurator---------------------------------------------
    
    
    void configurator::operator()( std::string const& config_file_p,
                                   std::string const & hist_list_p ) const
    {
 
        auto hist_c = find( regex_split(hist_list_p, std::regex{"(\\w| )+([^;]|$)"} ) );

        
        auto content = read(config_file_p);
        //add check on size ? match between hist size and config
        switch (content.opcode) {
            case flag_set<hist1d_flag, legend_flag, pave_text_flag>{}:{
                auto config = make_image< configuration< frame1d, histogram1d, legend, pave_text > >();
                config = fill( std::move(config), std::move(content.element_c) );
                apply( std::move(config), std::move( hist_c ) );
                break;
            }
            case flag_set<hist1d_flag, legend_flag>{} :{
                auto config = make_image< configuration< frame1d, histogram1d, legend > >();
                config = fill( std::move(config), std::move(content.element_c) );
                apply( std::move(config), std::move( hist_c ) );
                break;
            }
            case flag_set<hist1d_flag, pave_text_flag>{} :{
                auto config = make_image< configuration< frame1d, histogram1d, pave_text > >();
                config = fill( std::move(config), std::move(content.element_c) );
                apply( std::move(config), std::move( hist_c ) );
                break;
            }
            case flag_set<hist1d_flag>{} :{
                auto config = make_image< configuration< frame1d, histogram1d> >();
                config = fill( std::move(config), std::move(content.element_c) );
                apply( std::move(config), std::move( hist_c ) );
                break;
            }
            default:{
                std::cerr << "Unknown configuration: " << int(content.opcode) << '\n';
                break;
            }
        }
        
    }
    
    
    //will probably need reverse switch to get mask out and call proper find with th2
    //or base on configuration ?
    std::vector<TH1D*> configurator::find( std::vector<std::string> && hist_p ) const {
        std::vector<TH1D*> result_c;
        
        auto * current_directory_h = TDirectory::CurrentDirectory();
        auto& object_c = *current_directory_h->GetList();
        for( auto * object_h : object_c ){
            auto hist_i = std::find_if(
                                hist_p.begin(), hist_p.end(),
                                [&object_h]( std::string const& name_p )
                                { return name_p == std::string{ object_h->GetName() }; }
                                       );
            if( hist_i != hist_p.end() ){
                auto * hist_h = dynamic_cast<TH1D*>( object_h );
                if(hist_h){ result_c.push_back( hist_h ); }
            }
        }
        
        return result_c;
    }


    
    configurator::formatted_content configurator::read( std::string config_file_p ) const {
        std::ifstream input{ config_file_p };
        if( !input.is_open() ){
            std::cerr << "Could not open file: " << config_file_p << "\n";
            return {};
        }
        
        std::string file_content;
        input.seekg(0, std::ios::end);
        file_content.reserve(input.tellg());
        input.seekg(0, std::ios::beg);
        
        file_content.assign( std::istreambuf_iterator<char>(input),
                             std::istreambuf_iterator<char>() );
        
        
        auto user_text_c = regex_split( file_content,
                                         std::regex("\\[(.|\\n)*?\\]"));
        for(const auto& user_text : user_text_c){
            file_content.erase( file_content.find(user_text),
                                user_text.size());
        }
        
        file_content.erase(
                std::remove_if( file_content.begin(),
                                file_content.end(),
                                [](unsigned char c){
                                    return std::isspace(c);
                                    
                                } ),
                file_content.end()
                          );
        
        
        std::size_t position{0};
        for(const auto& user_text : user_text_c){
            position = file_content.find( "user_text:=", position );
            if( position != std::string::npos ){
                file_content.insert(
                            position + std::string{"user_text:="}.size(),
                            user_text
                                    );
            }
            position += std::string{"user_text:="}.size();
        }

        
        auto element_c = regex_split( file_content, std::regex{"<(\\w+)\\b>(.|\\n)*?<\\1>"} );
        uint8_t opcode {0};

        for( auto const& element : element_c ){
            auto tag = regex_first_match( element, std::regex{"<[^<>]+>"} );
            if( tag == "<legend>"    ){ opcode |= flag_set<legend_flag>{};}
            if( tag == "<pave_text>" ){ opcode |= flag_set<pave_text_flag>{}; }
            if( tag == "<hist1d>"    ){ opcode |= flag_set<hist1d_flag>{}; }
        }

        return { opcode, std::move( element_c ) };
    }


}//namespace iwir
