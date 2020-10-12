//
//File      : configurator.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 28/09/2020
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//

#ifndef configurator_hpp
#define configurator_hpp

#include "configuration_image.hpp"

#include <vector>
#include <string>
#include <regex>

#include "TH1.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TLegend.h"

namespace iwir {
    

    
    std::vector< std::string > regex_split( std::string const & text_p, std::regex regex_p );
    std::string regex_first_match( std::string const & text_p, std::regex regex_p );
    std::string remove_outer_tag( std::string text_p );
    double regex_arithmetic_value( std::string const & text_p, std::regex regex_p );
    
    struct configurator {
        struct formatted_content {
            uint8_t opcode;
            std::vector<std::string> element_c;
        };
        
        
    public:
        void operator()( std::string const& config_file_p, std::string const& hist_list_p ) const;
        
    private:
        std::vector<TH1D*> find( std::vector<std::string> && hist_p ) const ;
        
    private:
        formatted_content read( std::string config_file_p ) const;
        
        
        
        ///-------------------fill-----------------------
        struct element {
            std::string content;
            bool is_already_used{false};
        };
        
    private:
        template< class ... Ts>
        image< configuration<Ts...> > fill( image< configuration<Ts...> >&& image_p,
                                            std::vector<std::string> element_content_pc ) const {

            std::vector<element> element_c;
            element_c.reserve( element_content_pc.size() );
            for( auto const& element_content : element_content_pc){
                element_c.push_back( element{element_content} );
            }
            
            image_p = fill_element( std::move(image_p), element_c, pad{});
            int expander[] = { 0, ((image_p = fill_element( std::move(image_p), element_c, Ts{})), void(), 0) ... };
            
            return image_p;
        }
        
        template< class ... Ts,
                  class T,
                  typename std::enable_if_t< element_traits<T>::is_single_value::value, std::nullptr_t> = nullptr >
        image< configuration<Ts...> > fill_element( image< configuration<Ts...> >&& image_p,
                                                    std::vector<element>& element_pc,
                                                    T ) const {
            

            std::string content;

            for( auto & element : element_pc ){
                if( !element.is_already_used &&
                    std::string{ T::anchor } == regex_first_match( element.content,
                                                                     std::regex{"[^<>]+"} ) ){
                    element.is_already_used = true;
                    content = remove_outer_tag( element.content );
                    break;
                }
            }
            
            auto field_c = regex_split( content, std::regex{"<(\\w+)\\b>(.|\\n)*?<\\1>"} );
            
            return fill_element_impl(  std::move(image_p), std::move(field_c), T{} );
        }
        
        template< class ... Ts,
                  class T,
                  typename std::enable_if_t< !element_traits<T>::is_single_value::value, std::nullptr_t> = nullptr >
        image< configuration<Ts...> > fill_element( image< configuration<Ts...> >&& image_p,
                                                    std::vector<element>& element_pc,
                                                    T ) const {
            std::vector<std::string> content_c;

            for( auto & element : element_pc ){
                if( !element.is_already_used &&
                   std::string{ T::anchor } == regex_first_match( element.content,
                                                                 std::regex{"[^<>]+"} ) ){

                    element.is_already_used = true;
                    content_c.push_back( remove_outer_tag( element.content ) );
                }
            }
            
            for( auto const& content : content_c ){
                auto field_c = regex_split( content, std::regex{"<(\\w+)\\b>(.|\\n)*?<\\1>"} );
                image_p = fill_element_impl(  std::move(image_p), std::move(field_c), T{} );
            }
            
            return std::move(image_p);
        }
        
        
        template<class T>
        void fill_range( T & range_p, std::vector<std::string> const& entry_c ) const {
            for( auto const& entry : entry_c) {
                auto name = regex_first_match( entry, std::regex{"[^:=]+"} );
                if( name == "low" ){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    range_p.template fill<low>( value );
                }
                
                if( name == "high"){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    range_p.template fill<high>( value );
                }
            }
        }
        
        template<class T>
        void fill_title( T & title_p, std::vector<std::string> const& entry_c ) const {
            for( auto const& entry : entry_c) {
                auto name = regex_first_match( entry, std::regex{"[^:=]+"} );
                
                if( name == "size" ){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    title_p.template fill<size>( value );
                }
                if( name == "offset"){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    title_p.template fill<offset>( value );
                }
                if( name == "user_text"){
                    std::string content = entry;
                    content.erase( content.find(name), name.size() + 2  );
                    auto value = regex_first_match( content, std::regex{"[^\\[\\]]+"} );
                    title_p.template fill<user_text>( value );
                }
            }
        }
        
        template<class T>
        void fill_label( T & label_p, std::vector<std::string> const& entry_c ) const {
            for( auto const& entry : entry_c) {
                auto name = regex_first_match( entry, std::regex{"[^:=]+"} );

                if( name == "size" ){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    label_p.template fill<size>( value );
                }
                if( name == "offset"){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    label_p.template fill<offset>( value );
                }
            }
        }
        
        template<class T>
        void fill_header( T & header_p, std::vector<std::string> const& entry_c ) const {
            for( auto const& entry : entry_c) {
                auto name = regex_first_match( entry, std::regex{"[^:=]+"} );
                
                if( name == "size" ){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    header_p.template fill<size>( value );
                }
                if( name == "color"){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    header_p.template fill<color>( value );
                }
                if( name == "user_text"){
                    std::string content = entry;
                    content.erase( content.find(name), name.size() +2 );
                    auto value = regex_first_match( content, std::regex{"[^\\[\\]]+"} );
                    header_p.template fill<user_text>( value );
                }
            }
        }
        
        template<class T>
        void fill_legend_attributes( T & header_p, std::vector<std::string> const& entry_c ) const {
            for( auto const& entry : entry_c) {
                auto name = regex_first_match( entry, std::regex{"[^:=]+"} );
                
                if( name == "plain_text"){
                    std::string content = entry;
                    content.erase( content.find(name), name.size() );
                    auto value = regex_first_match( content, std::regex{"\\w+"} );
                    header_p.template fill<plain_text>( value );
                }
                if( name == "user_text"){
                    std::string content = entry;
                    content.erase( content.find(name), name.size() +2 );
                    auto value = regex_first_match( content, std::regex{"[^\\[\\]]+"} );
                    header_p.template fill<user_text>( value );
                }
            }
        }
        
        template<class T>
        void fill_option( T & header_p, std::vector<std::string> const& entry_c ) const {
            for( auto const& entry : entry_c) {
                auto name = regex_first_match( entry, std::regex{"[^:=]+"} );

                if( name == "plain_text"){
                    std::string content = entry;
                    content.erase( content.find(name), name.size() );
                    auto value = regex_first_match( content, std::regex{"\\w+"} );
                    header_p.template fill<plain_text>( value );
                }
            }
        }
        
        template<class T>
        void fill_marker( T & marker_p, std::vector<std::string> const& entry_c ) const {
            for( auto const& entry : entry_c) {
                auto name = regex_first_match( entry, std::regex{"[^:=]+"} );
                
                if( name == "size" ){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    marker_p.template fill<size>( value );
                }
                if( name == "color"){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    marker_p.template fill<color>( value );
                }
                if( name == "style"){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    marker_p.template fill<style>( value );
                }
            }
        }
        
        template<class T>
        void fill_line( T & line_p, std::vector<std::string> const& entry_c ) const {
            for( auto const& entry : entry_c) {
                auto name = regex_first_match( entry, std::regex{"[^:=]+"} );
                
                if( name == "width" ){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    line_p.template fill<width>( value );
                }
                if( name == "color"){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    line_p.template fill<color>( value );
                }
                if( name == "style"){
                    auto value = regex_arithmetic_value( entry, std::regex{"([0-9]|\\.)+"} );
                    line_p.template fill<style>( value );
                }
            }
        }
        
        
        template< class ... Ts >
        image< configuration<Ts...> > fill_element_impl( image< configuration<Ts...> >&& image_p,
                                                         std::vector<std::string>&& field_pc,
                                                         pad ) const {

            for(auto const& field : field_pc) {
                std::string entries = remove_outer_tag(field);
                auto entry_c = regex_split(entries, std::regex{"[^;]+"} );
                
                auto field_name = regex_first_match( field, std::regex{"[^<>]+"} );
                if( field_name == "range_x"  ){
                    auto & range_x = image_p.template retrieve_element<pad>()
                                            .template retrieve_field< range<x> >();
                    fill_range( range_x, entry_c );
                }
                if( field_name == "range_y"  ){
                    auto & range_y = image_p.template retrieve_element<pad>()
                                            .template retrieve_field<range<y>>();
                    fill_range( range_y, entry_c );
                }
            }

            return std::move(image_p);
        }
        
        template< class ... Ts >
        image< configuration<Ts...> > fill_element_impl( image< configuration<Ts...> >&& image_p,
                                                         std::vector<std::string>&& field_pc,
                                                         frame1d ) const {
            
            for(auto const& field : field_pc) {
                std::string entries = remove_outer_tag(field);
                auto entry_c = regex_split(entries, std::regex{"[^;]+"} );
                
                auto field_name = regex_first_match( field, std::regex{"[^<>]+"} );
                if( field_name == "range_x" ){
                    auto & range_x = image_p.template retrieve_element<frame1d>()
                                            .template retrieve_field< range<x> >();
                    fill_range( range_x, entry_c );
                }
                if( field_name == "title_x"  ){
                    auto & title_x = image_p.template retrieve_element<frame1d>()
                                            .template retrieve_field< title<x> >();
                    fill_title( title_x, entry_c );
                }
                if( field_name == "label_x" ){
                    auto & label_x = image_p.template retrieve_element<frame1d>()
                                            .template retrieve_field< label<x> >();
                    fill_label( label_x, entry_c );
                }
                if( field_name == "range_y" ){
                    auto & range_y = image_p.template retrieve_element<frame1d>()
                                            .template retrieve_field<range<y>>();
                    fill_range( range_y, entry_c );
                }
                if( field_name == "title_y" ){
                    auto & title_y = image_p.template retrieve_element<frame1d>()
                                            .template retrieve_field< title<y> >();
                    fill_title( title_y, entry_c );
                }
                if( field_name == "label_y"  ){
                    auto & label_y = image_p.template retrieve_element<frame1d>()
                                            .template retrieve_field< label<y> >();
                    fill_label( label_y, entry_c );
                }
            }
            
            return std::move(image_p);
        }
        
        template< class ... Ts >
        image< configuration<Ts...> > fill_element_impl( image< configuration<Ts...> >&& image_p,
                                                        std::vector<std::string>&& field_pc,
                                                        histogram1d ) const {
            auto& hist_element = image_p.template retrieve_element<histogram1d>().add_value();
            for(auto const& field : field_pc) {
                std::string entries = remove_outer_tag(field);
                auto entry_c = regex_split(entries, std::regex{"[^;]+"} );
                
                auto field_name = regex_first_match( field, std::regex{"[^<>]+"} );
                if( field_name == "legend_attributes" ){
                    auto & attributes_field = hist_element.template retrieve_field<legend_attributes>();
                    fill_legend_attributes( attributes_field, entry_c );
                }
                if( field_name == "option" ){
                    auto & option_field = hist_element.template retrieve_field<option>();
                    fill_option( option_field, entry_c );
                }
                if( field_name == "marker" ){
                    auto & marker_field = hist_element.template retrieve_field<marker>();
                    fill_marker( marker_field, entry_c );
                }
                if( field_name == "line" ){
                    auto & line_field = hist_element.template retrieve_field<line>();
                    fill_line( line_field, entry_c );
                }
            }
            return std::move( image_p );
        }
        
        template< class ... Ts >
        image< configuration<Ts...> > fill_element_impl( image< configuration<Ts...> >&& image_p,
                                                        std::vector<std::string>&& field_pc,
                                                        legend ) const {
            for(auto const& field : field_pc) {
                std::string entries = remove_outer_tag(field);
                auto entry_c = regex_split(entries, std::regex{"[^;]+"} );
                
                auto field_name = regex_first_match( field, std::regex{"[^<>]+"} );
                if( field_name == "header" ){
                    auto & header_element = image_p.template retrieve_element<legend>()
                                                   .template retrieve_field< header<single> >();
                    fill_header( header_element , entry_c );
                }
                if( field_name == "range_x" ){
                    auto & range_x = image_p.template retrieve_element<legend>()
                                            .template retrieve_field<range<x> >();
                    fill_range( range_x, entry_c);
                }
                if( field_name == "range_y" ){
                    auto & range_y = image_p.template retrieve_element<legend>()
                                            .template retrieve_field< range<y> >();
                    fill_range( range_y, entry_c);
                }
            }
            
            return std::move( image_p );
        }
        
        template< class ... Ts >
        image< configuration<Ts...> > fill_element_impl( image< configuration<Ts...> >&& image_p,
                                                        std::vector<std::string>&& field_pc,
                                                        pave_text ) const {
            auto& text_element = image_p.template retrieve_element<pave_text>().add_value();
            for(auto const& field : field_pc) {
                std::string entries = remove_outer_tag(field);
                auto entry_c = regex_split(entries, std::regex{"[^;]+"} );
                
                auto field_name = regex_first_match( field, std::regex{"[^<>]+"} );
                if( field_name == "header" ){
                    auto & header_element = text_element.template retrieve_field< header<multiple> >().add_value();
                    fill_header( header_element , entry_c );
                }
                if( field_name == "range_x" ){
                    auto & range_x = text_element.template retrieve_field< range<x> >();
                    fill_range( range_x, entry_c);
                }
                if( field_name == "range_y" ){
                    auto & range_y = text_element.template retrieve_field< range<y> >();
                    fill_range( range_y, entry_c);
                }
            }
            
            return std::move( image_p );
        }

        
        
        ///-------------------apply-----------------------
    private:
        template< class ... Ts>
        void apply( image< configuration<Ts...> >&& image_p,
                     std::vector<TH1D *>&& hist_pc ) const {
            apply_element( std::move(image_p), std::move(hist_pc), pad{});
            int expander[] = { 0, (apply_element( image_p, std::move(hist_pc), Ts{}), void(), 0) ... };
        }
        
        
        template< class ... Ts >
        void apply_element( image< configuration<Ts...> > const& image_p,
                             std::vector<TH1D*> const&& /*hist_pc*/,
                             pad ) const {
            
            auto const & margin_x = image_p.template retrieve_element<pad>()
                                          .template retrieve_field< range<x> >()
                                          .retrieve();
            
            auto const & margin_y = image_p.template retrieve_element<pad>()
                                          .template retrieve_field<range<y>>()
                                          .retrieve();
            
            auto * canvas_h = new TCanvas{};
            
            canvas_h->SetTopMargin(margin_y.high);
            canvas_h->SetRightMargin(margin_x.high);
            canvas_h->SetBottomMargin(margin_y.low);
            canvas_h->SetLeftMargin(margin_x.low);
        }
        
        template< class ... Ts >
        void apply_element( image< configuration<Ts...> > const& image_p,
                             std::vector<TH1D *>&& /*hist_pc*/,
                             frame1d ) const {
            
            auto & frame_element = image_p.template retrieve_element<frame1d>();
            auto * frame_h = new TH1D{"frame","",1,0,1};
            
            auto & title_x = frame_element.template retrieve_field< title<x> >().retrieve();
            frame_h->GetXaxis()->SetTitle( title_x.user_data().c_str() );
            frame_h->GetXaxis()->SetTitleSize( title_x.size );
            frame_h->GetXaxis()->SetTitleOffset( title_x.offset );
            
            auto & range_x = frame_element.template retrieve_field< range<x> >().retrieve();
            frame_h->GetXaxis()->SetRangeUser(range_x.low, range_x.high);
            
            auto & label_x = frame_element.template retrieve_field< label<x> >().retrieve();
            frame_h->GetXaxis()->SetLabelSize( label_x.size );
            frame_h->GetXaxis()->SetLabelOffset( label_x.offset );
            
            auto & title_y = frame_element.template retrieve_field< title<y> >().retrieve();
            frame_h->GetYaxis()->SetTitle( title_y.user_data().c_str() );
            frame_h->GetYaxis()->SetTitleSize( title_y.size );
            frame_h->GetYaxis()->SetTitleOffset( title_y.offset );
            
            auto & range_y = frame_element.template retrieve_field< range<y> >().retrieve();
            frame_h->GetYaxis()->SetRangeUser( range_y.low, range_y.high  );
            
            auto & label_y = frame_element.template retrieve_field< label<y> >().retrieve();
            frame_h->GetYaxis()->SetLabelSize( label_y.size ) ;
            frame_h->GetYaxis()->SetLabelOffset( label_y.offset );
            
            frame_h->Draw();
        }
        
        template< class ... Ts >
        void apply_element( image< configuration<Ts...> > const& image_p,
                             std::vector<TH1D *>&& /*hist_pc*/,
                             pave_text ) const {
            
            for( auto const& text_element : image_p.template retrieve_element<pave_text>() ) {
                auto& range_x = text_element.template retrieve_field< range<x> >().retrieve();
                auto& range_y = text_element.template retrieve_field< range<y> >().retrieve();
                
                auto * pave_text_h = new TPaveText{
                    range_x.low,
                    range_y.low,
                    range_x.high,
                    range_y.high
                };
                
                for( auto const& header_field : text_element.template retrieve_field< header<multiple> >() ){
                    auto const& header = header_field.retrieve();
                    auto * text_h = pave_text_h->AddText( header.user_data().c_str() );
                    text_h->SetTextSize( header.size );
                    text_h->SetTextColor( header.color );
                }
                
                pave_text_h->Draw("same");
            }
            
        }
        
        template< class ... Ts >
        void apply_element( image< configuration<Ts...> > const& image_p,
                             std::vector<TH1D*>&& hist_pc,
                             histogram1d ) const {
            
            auto hist_i = hist_pc.begin();
            for( auto const& hist_element : image_p.template retrieve_element<histogram1d>() ) {
                auto * hist_h = *hist_i;
                
                auto const& name_field = hist_element.template retrieve_field< name >().retrieve();
                hist_h->SetTitle(name_field.plain_data().c_str());
                
                auto const& marker_field = hist_element.template retrieve_field< marker >().retrieve();
                hist_h->SetMarkerStyle( marker_field.style );
                hist_h->SetMarkerSize( marker_field.size );
                hist_h->SetMarkerColor( marker_field.color );
                
                auto const& line_field = hist_element.template retrieve_field< line >().retrieve();
                hist_h->SetLineStyle( line_field.style );
                hist_h->SetLineWidth( line_field.width );
                hist_h->SetLineColor( line_field.color );
                
                auto const& option_field = hist_element.template retrieve_field< option >().retrieve();
                hist_h->Draw( std::string{"same " + option_field.plain_data()}.c_str() ) ;
                
                ++hist_i;
            }
            
        }
        
        
        template< class ... Ts >
        void apply_element( image< configuration<Ts...> > const& image_p,
                             std::vector<TH1D*>&& hist_pc,
                             legend ) const {
            
            auto const& legend_element = image_p.template retrieve_element<legend>();
            
            auto const& range_x = legend_element.template retrieve_field< range<x> >().retrieve();
            auto const& range_y = legend_element.template retrieve_field< range<y> >().retrieve();
            
            auto * legend_h = new TLegend{
                range_x.low,
                range_y.low,
                range_x.high,
                range_y.high
            };
            
            auto const& header_field = legend_element.template retrieve_field< header<single> >().retrieve();
            legend_h->SetHeader( header_field.user_data().c_str() );
            legend_h->SetTextColor( header_field.color );
            legend_h->SetTextSize( header_field.size );
            
            
            auto hist_i = hist_pc.begin();
            for( auto const& hist_element : image_p.template retrieve_element<histogram1d>() ) {
                auto const& name_field = hist_element.template retrieve_field< name >().retrieve();
                
                auto const& attributes_field = hist_element.template retrieve_field< legend_attributes >().retrieve();
                legend_h->AddEntry(
                                *hist_i,
                                attributes_field.user_data().c_str() ,
                                attributes_field.plain_data().c_str()
                                   );
                
                ++hist_i;
            }
            
            legend_h->Draw("same");
            
        }
        

    };
    
}//namespace iwir


#endif /* configurator_hpp */
