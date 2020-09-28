//
//File      : saver.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 11/09/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef saver_h
#define saver_h


//iwir header
#include "configuration_image.hpp"


//std headers
#include <fstream>


//ROOT header
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TLegendEntry.h"


template<class T>
struct TD;

namespace iwir {

struct saver{
    
public:
    void operator()(TCanvas const* canvas_ph, std::string output_filename_p);
    
private:
    template< class ... Ts>
    image< configuration<Ts...> > fill( image< configuration<Ts...> >&& image_p,
                                        TCanvas const* canvas_ph ) {
        image_p =  fill_element( std::move(image_p), canvas_ph, pad{});
        int expander[] = { 0, (image_p = fill_element( std::move(image_p), canvas_ph, Ts{}), void(), 0) ... };
        return image_p;
    }
    
    template< class ... Ts >
    image< configuration<Ts...> > fill_element( image< configuration<Ts...> >&& image_p,
                                                TCanvas const* canvas_ph,
                                                pad ) {
        auto & range_x = image_p.template retrieve_element<pad>()
                                .template retrieve_field<range<x>>();
        range_x.template fill<low, high>( canvas_ph->GetLeftMargin(),
                                          canvas_ph->GetRightMargin() );
        
        auto & range_y = image_p.template retrieve_element<pad>()
                                .template retrieve_field<range<y>>();
        range_y.template fill<low, high>( canvas_ph->GetBottomMargin(),
                                          canvas_ph->GetTopMargin() );

        return std::move(image_p);
    }
    
    template< class ... Ts >
    image< configuration<Ts...> > fill_element( image< configuration<Ts...> >&& image_p,
                                                TCanvas const* canvas_ph,
                                                pave_text ) {
        auto const& primitive_c = *( canvas_ph->GetListOfPrimitives() );
        for( auto const* primitive_h : primitive_c ) {
            if( std::string{primitive_h->ClassName()} ==  "TPaveText"  ){
                auto const * text_h = dynamic_cast<TPaveText const*>( primitive_h );
                
                auto& text_element = image_p.template retrieve_element<pave_text>().add_value();
               
                auto& range_x = text_element.template retrieve_field< range<x> >();
                range_x.template fill< low, high >( text_h->GetX1NDC(), text_h->GetX2NDC() );
                auto& range_y = text_element.template retrieve_field< range<y> >();
                range_y.template fill< low, high >( text_h->GetY1NDC(), text_h->GetY2NDC() );
                
                auto const& line_c = *text_h->GetListOfLines();
                for( auto const* line_h : line_c ){
                    auto const* text_line_h = dynamic_cast<TText const*>( line_h );
                    if(text_line_h){
                        auto& entry = text_element.template retrieve_field< header<multiple> >().add_value();
                        entry.template fill<user_text, size, color>(
                                            text_line_h->GetTitle(),
                                            text_line_h->GetTextSize(),
                                            text_line_h->GetTextColor()
                                                                    );
                    }
                }
            }
        }
        return std::move(image_p);
    }
    
    template< class ... Ts >
    image< configuration<Ts...> > fill_element( image< configuration<Ts...> >&& image_p,
                                                TCanvas const* canvas_ph,
                                                legend ) {
        auto const& primitive_c = *( canvas_ph->GetListOfPrimitives() );
        

        for( auto const* primitive_h : primitive_c ) {
            if( std::string{primitive_h->ClassName()} ==  "TLegend" ){
                auto const * legend_h = dynamic_cast<TLegend const*>( primitive_h );
                

                
                auto & legend_element = image_p.template retrieve_element<legend>();
                auto & header_field = legend_element.template retrieve_field< header<single> >();
                header_field.template fill<user_text, size, color> (
                                legend_h->GetHeader(),
                                legend_h->GetTextSize(),
                                legend_h->GetTextColor()
                                                                    );

                
                auto & range_x_field = legend_element.template retrieve_field< range<x> >();
                range_x_field.template fill<low, high>(legend_h->GetX1NDC(), legend_h->GetX2NDC() );
                
                
                auto & range_y_field = legend_element.template retrieve_field< range<y> >();
                range_y_field.template fill<low, high>( legend_h->GetY1NDC(), legend_h->GetY2NDC() );
                

                
                auto const& entry_c = *legend_h->GetListOfPrimitives();
                auto & hist_element = image_p.template retrieve_element<histogram1d>();
                
                for( auto const* entry_h : entry_c ){
                    auto const * legend_entry_h = dynamic_cast<TLegendEntry const*>( entry_h );
                        auto* obj_h = legend_entry_h->GetObject();
                        if(obj_h){
                            std::string hist_name = obj_h->GetName();
                            for( auto & hist : hist_element ){
                                auto const& name_field = hist.template retrieve_field<name>();
                                if( hist_name == name_field.retrieve().value() ){
                                    auto & legend_field = hist.template retrieve_field<legend_attributes>();
                                    legend_field.template fill<user_text, plain_text>(
                                                legend_entry_h->GetLabel(),
                                                legend_entry_h->GetOption()
                                                                              );
                                }
                            }
                        }
                }
            }
        }
        return std::move(image_p);
    }
    
    template< class ... Ts >
    image< configuration<Ts...> > fill_element( image< configuration<Ts...> >&& image_p,
                                                TCanvas const* canvas_ph,
                                                histogram1d ) {
        TIter primitive_i = canvas_ph->GetListOfPrimitives(); //needed to access GetOption()
        TObject const* object_h = nullptr;
        while( (object_h = primitive_i.Next()) ) {
            if( object_h->InheritsFrom( TH1::Class() )  ){
                auto const * histogram_h = dynamic_cast<TH1 const*>( object_h );
                auto & histogram = image_p.template retrieve_element<histogram1d>().add_value();
                
                auto & name_field = histogram.template retrieve_field<name>();
                name_field.template fill<plain_text>( histogram_h->GetName() );
                
                auto & option_field = histogram.template retrieve_field<option>();
                option_field.template fill<plain_text>( primitive_i.GetOption() );
                
                auto & marker_field = histogram.template retrieve_field<marker>();
                marker_field.template fill<size, style, color>(
                                        histogram_h->GetMarkerSize(),
                                        histogram_h->GetMarkerStyle(),
                                        histogram_h->GetMarkerColor()
                                                               );
                
                auto & line_field = histogram.template retrieve_field<line>();
                line_field.template fill<width, style, color>(
                                        histogram_h->GetLineWidth(),
                                        histogram_h->GetLineStyle(),
                                        histogram_h->GetLineColor()
                                                               );

            }
        }
        return std::move(image_p);
    }
    
    
    template< class ... Ts >
    image< configuration<Ts...> > fill_element( image< configuration<Ts...> >&& image_p,
                                                TCanvas const* canvas_ph,
                                                frame1d ) {
        auto fill_using = [&image_p]( TH1 const* frame_p )
                    {
                        auto & frame_element = image_p.template retrieve_element<frame1d>();
                        
                        auto & title_x = frame_element.template retrieve_field< title<x> >();
                        title_x.template fill<user_text, size, offset>(
                                    frame_p->GetXaxis()->GetTitle(),
                                    frame_p->GetXaxis()->GetTitleSize(),
                                    frame_p->GetXaxis()->GetTitleOffset()
                                                                      );
                        
                        auto & range_x = frame_element.template retrieve_field< range<x> >();
                        range_x.template fill<low, high>(
                                    frame_p->GetXaxis()->GetFirst(),
                                    frame_p->GetXaxis()->GetLast()
                                                        );
                        
                        auto & label_x = frame_element.template retrieve_field< label<x> >();
                        label_x.template fill<size, offset>(
                                    frame_p->GetXaxis()->GetLabelSize(),
                                    frame_p->GetXaxis()->GetLabelOffset()
                                                            );
                        
                        auto & title_y = frame_element.template retrieve_field< title<y> >();
                        title_y.template fill<user_text, size, offset>(
                                    frame_p->GetYaxis()->GetTitle(),
                                    frame_p->GetYaxis()->GetTitleSize(),
                                    frame_p->GetYaxis()->GetTitleOffset()
                                                                       );
                        
                        auto & range_y = frame_element.template retrieve_field< range<y> >();
                        range_y.template fill<low, high>(
                                    frame_p->GetYaxis()->GetFirst(),
                                    frame_p->GetYaxis()->GetLast()
                                                        );
                        
                        auto & label_y = frame_element.template retrieve_field< label<y> >();
                        label_y.template fill< size, offset>(
                                    frame_p->GetYaxis()->GetLabelSize(),
                                    frame_p->GetYaxis()->GetLabelOffset()
                                                            );
                    };
        
        auto const& primitive_c = *( canvas_ph->GetListOfPrimitives() );
        for( auto const* primitive_h : primitive_c ) {
            if( primitive_h->InheritsFrom( TH1::Class() )  ){
                auto const * histogram_h = dynamic_cast<TH1 const*>( primitive_h );
                if( std::string{histogram_h->GetTitle()} == "frame" ){
                    fill_using( histogram_h );
                    return std::move(image_p);
                }
            }
        }
        
        for( auto const* primitive_h : primitive_c ) {
            if( primitive_h->InheritsFrom( TH1::Class() )  ){
                auto const * histogram_h = dynamic_cast<TH1 const*>( primitive_h );
                fill_using( histogram_h );
                return std::move(image_p);
            }
        }
        

        return std::move(image_p);
    }
    
//    template< class ... Ts >
//    image< configuration<Ts...> > fill_element( image< configuration<Ts...> >&& image_p,
//                                               TCanvas const* canvas_ph,
//                                               histogram2d ) {
//        auto const& primitive_c = *( canvas_ph->GetListOfPrimitives() );
//        for( auto const* primitive_h : primitive_c ) {
//            if( primitive_h->InheritsFrom( TH2::Class() )  ){  }
//        }
//        return std::move(image_p);
//    }
    
    template<class ... Ts>
    void write( std::string const& output_filename_p,
                          image<Ts...> config_p ) {
        std::ofstream output{ output_filename_p.c_str(), std::ios::out | std::ios::trunc };
        if( !output.good() ){
            std::cerr << "Something went wrong with the output file\n";
        }
        output << config_p.retrieve_content();
    }
};
    
    
} //namespace iwir

#endif /* saver_h */
