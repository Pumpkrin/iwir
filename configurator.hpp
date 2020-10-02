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
    
    template<class ... Ts> struct regex_engine{};
    
    struct text_tag{};
    struct pattern_tag{};
    
    template<>
    struct regex_engine<pattern_tag, text_tag> {
        friend regex_engine<pattern_tag>;
        friend regex_engine<text_tag>;
        
        struct iterator{
            std::string operator*() const {
                return underlying_m->str();
            }
            
            friend bool operator!=( iterator lhs_p, iterator rhs_p ) {
                return lhs_p.underlying_m != rhs_p.underlying_m;
            }
            
            iterator const& operator++() { ++underlying_m; return *this; }
            
            iterator() = default;
            iterator( std::string const& text_p, std::regex const& regex_p) :
            underlying_m{ text_p.begin(), text_p.end(), regex_p } {}
            
        private:
            std::sregex_iterator underlying_m;
        };
        
        
    public:
        auto begin() const { return iterator{text_m, regex_m}; }
        auto end() const { return iterator{}; }
        //std::size_t size() const { return std::distance( begin(), end() ); }
        //probably bad idea anyhow + needs to meet requirements
        
    private:
        regex_engine( std::regex regex_p, std::string text_p ) : regex_m{regex_p}, text_m{text_p} {}
        
    private:
        std::regex regex_m;
        std::string text_m;
        
    };
    
    
    template<>
    struct regex_engine<text_tag> {
        friend regex_engine<>;
        
    public:
        
        regex_engine<pattern_tag, text_tag> find_pattern( std::regex regex_p ) && {
            return regex_engine<pattern_tag, text_tag>{ std::move(regex_p), std::move(text_m) };
        }
        
    private:
        regex_engine( std::string text_p ) : text_m{text_p} {}
        
    private:
        std::string text_m;
    };
    
    template<>
    struct regex_engine<pattern_tag> {
        friend regex_engine<>;
        
    public:
        regex_engine<pattern_tag, text_tag> in( std::string text_p ) && {
            return regex_engine<pattern_tag, text_tag>{ std::move(regex_m), std::move(text_p) };
        }
        
    private:
        regex_engine( std::regex regex_p ) : regex_m{regex_p} {}
        
    private:
        std::regex regex_m;
        
    };
    
    template<>
    struct regex_engine<> {
        friend inline regex_engine make_regex_engine();
        
        regex_engine<text_tag> in( std::string text_p ) && {
            return regex_engine<text_tag>{ std::move(text_p) };
        }
        
        regex_engine<pattern_tag> find_pattern( std::regex regex_p ) && {
            return regex_engine<pattern_tag>{ std::move(regex_p) };
        }
    private:
        regex_engine() = default;
    };
    
    inline regex_engine<> make_regex_engine() { return {}; }
    
    
    
    
    std::vector< std::string > regex_split( std::string const & text_p, std::regex regex_p ) {
        std::vector<std::string> result_c;
        result_c.reserve(10);
        
        
        std::sregex_iterator match_i{ text_p.begin(), text_p.end(), std::move( regex_p ) };
        auto end_i = std::sregex_iterato{};
        
        
        for(auto iterator = match_i; iterator != end_i ; ++iterator){
            result_c.push_back( iterator->str() );
        }
        
        return result_c;
    }
    
    
    
    
    
    
    
    struct configurator {
        
    public:
        void operator()( std::string const& config_file_p, std::string const& hist_list_p ) const;
        
    private:
        std::vector<std::string> split( std::string const& hist_p ) const ;
        std::vector<TH1D*> find( std::vector<std::string> && hist_p ) const ;
        
    private:
        template< class ... Ts >
        image< configuration<Ts ...> > read(  std::string config_file_p ) const;
        
    private:
        template< class ... Ts>
        void create( image< configuration<Ts...> >&& image_p,
                     std::vector<TH1D *>&& hist_pc ) const {
            create_element( std::move(image_p), std::move(hist_pc), pad{});
            int expander[] = { 0, (create_element( std::move(image_p), std::move(hist_pc), Ts{}), void(), 0) ... };
        }
        
        
        template< class ... Ts >
        void create_element( image< configuration<Ts...> >&& image_p,
                             std::vector<TH1D const*>&& hist_pc,
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
        void create_element( image< configuration<Ts...> >&& image_p,
                             std::vector<TH1D *>&& /*hist_pc*/,
                             frame1d ) const {
            auto & frame_element = image_p.template retrieve_element<frame1d>();
            auto * frame_h = new TH1D{"frame","",1,0,1};
            
            auto & title_x = frame_element.template retrieve_field< title<x> >().retrieve();
            frame_h->GetXaxis()->SetTitle( title_x.text() );
            frame_h->GetXaxis()->SetTitleSize( title_x.size );
            frame_h->GetXaxis()->SetTitleOffset( title_x.offset );
            
            auto & range_x = frame_element.template retrieve_field< range<x> >().retrieve();
            frame_h->GetXaxis()->SetRangeUser(range_x.low, range_x.high);
            
            auto & label_x = frame_element.template retrieve_field< label<x> >().retrieve();
            frame_h->GetXaxis()->SetLabelSize( label_x.size );
            frame_h->GetXaxis()->SetLabelOffset( label_x.offset );
            
            auto & title_y = frame_element.template retrieve_field< title<y> >().retrieve();
            frame_h->GetYaxis()->SetTitle( title_y.text() );
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
        void create_element( image< configuration<Ts...> >&& image_p,
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
                    auto * text_h = pave_text_h->AddText( header.text().c_str() );
                    text_h->SetTextSize( header.size );
                    text_h->SetTextColor( header.color );
                }
                
                pave_text_h->Draw("same");
            }
            
        }
        
        template< class ... Ts >
        void create_element( image< configuration<Ts...> >&& image_p,
                             std::vector<TH1D*>&& hist_pc,
                             histogram1d ) const {
            
            auto hist_i = hist_pc.begin();
            for( auto const& hist_element : image_p.template retrieve_element<histogram1d>() ) {
                auto * hist_h = *hist_i;
                
                auto const& name_field = hist_element.template retrieve_field< name >().retrieve();
                hist_h->SetTitle(name_field.text);
                
                auto const& marker_field = hist_element.template retrieve_field< marker >().retrieve();
                hist_h->SetMarkerStyle( marker_field.style );
                hist_h->SetMarkerSize( marker_field.size );
                hist_h->SetMarkerColor( marker_field.color );
                
                auto const& line_field = hist_element.template retrieve_field< line >().retrieve();
                hist_h->SetLineStyle( line_field.style );
                hist_h->SetLineWidth( line_field.width );
                hist_h->SetLineColor( line_field.color );
                
                auto const& option_field = hist_element.template retrieve_field< option >().retrieve();
                hist_h->Draw( std::string{"same " + option_field.text}.c_str() ) ;
                
                ++hist_i;
            }
            
        }
        
        
        template< class ... Ts >
        void create_element( image< configuration<Ts...> >&& image_p,
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
            legend_h->SetHeader( header_field.text() );
            legend_h->SetTextColor( header_field.color );
            legend_h->SetTextSize( header_field.size );
            
            
            auto hist_i = hist_pc.begin();
            for( auto const& hist_element : image_p.template retrieve_element<histogram1d>() ) {
                auto const& name_field = hist_element.template retrieve_field< name >().retrieve();
                
                auto const& attributes_field = hist_element.template retrieve_field< legend_attributes >().retrieve();
                legend_h->AddEntry(*hist_i, attributes_field.text() , attributes_field.text );
                
                ++hist_i;
            }
            
            legend_h->Draw("same");
            
        }
        

    };
    
}//namespace iwir


#endif /* configurator_hpp */
