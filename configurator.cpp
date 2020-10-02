//
//File      : configurator.cpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 28/09/2020
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//

#include "configurator.hpp"

#include "TDirectory.h"

namespace iwir {

    void configurator::operator()( std::string const& config_file_p,
                                   std::string const & hist_list_p ) const
    {
        //read -> outputs image<configuration>
        //split -> vector<string>
        //find
        //create

        
//        auto hist_name_c = split(hist_list_p) ;
//        for( auto & hist_name : hist_name_c ){ std::cout << hist << '\n'; }
        auto hist_c = find( regex_split(hist_list_p, std::regex{"(\\w| )+([^;]|$)"} ) );
        for( auto const * hist_h : hist_c ){ std::cout << hist_h->GetTitle() << '\n'; }

        
    }
    
    

    
    
    //will probably need reverse switch to get mask out and call proper find with th2
    //or base on configuration ?
    //everything should be known once it's loaded ?
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


}//namespace iwir
