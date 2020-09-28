//
//File      : saver.cpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 11/09/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#include "saver.hpp"





namespace iwir {
    
    //fields
    template<class T>
    constexpr decltype("range" + T::anchor) range<T>::anchor;
    template<class T>
    constexpr decltype("label" + T::anchor) label<T>::anchor;
    template<class T>
    constexpr decltype("title" + T::anchor) title<T>::anchor;
    
    template<class T>
    constexpr details::constexpr_string<6> header<T>::anchor;
    
    constexpr details::constexpr_string<17> legend_attributes::anchor;
    constexpr details::constexpr_string<6> option::anchor;
    constexpr details::constexpr_string<4> line::anchor;
    constexpr details::constexpr_string<6> marker::anchor;

    
    
    //element
    constexpr details::constexpr_string<9> pave_text::anchor;
    constexpr details::constexpr_string<6> legend::anchor;
    constexpr details::constexpr_string<3> pad::anchor;
    constexpr details::constexpr_string<6> histogram1d::anchor;
    constexpr details::constexpr_string<7> frame1d::anchor;

    
    
    
    void saver::operator()(TCanvas const* canvas_ph, std::string output_filename_p) {
        //loop on canvas primitives
        //retrieve opcode
        //fill corresponding configuration image
        //write it to file
        
        uint8_t opcode{};
        auto const& primitive_c = *canvas_ph->GetListOfPrimitives();
        for( auto const* primitive_h : primitive_c ) {
            auto bit = opcode  & 1U;
            if( bit != 1U && std::string{primitive_h->ClassName()} == "TPaveText"  ){ opcode ^= 1U;  }
            bit = (opcode >> 1 ) & 1U;
            if( bit != 1U && std::string{primitive_h->ClassName()} ==  "TLegend"  ){ opcode ^= 1U << 1; }
            bit = (opcode >> 2 ) & 1U;
            if( bit != 1U && primitive_h->InheritsFrom( TH1::Class() )  ){ opcode ^= 1U << 2; }
//            bit = (opcode >> 3 ) & 1U;
//            if( bit != 1U && primitive_h->InheritsFrom( TH2::Class() )  ){ opcode ^= 1U << 3; }
        }
        
        switch( opcode ) {
//        case 0b1001: {
//            auto config = make_image< configuration< histogram2d, text > >();
//            config = fill_element( std::move(config), canvas_ph, canvas{} );
//            config = fill_element( std::move(config), canvas_ph, histogram2d{} );
//            config = fill_element( std::move(config), canvas_ph, text{} );
//            write( output_filename_p, std::move(config)  );
//            break;
//        }
//        case 0b1000:{
//            auto config = make_image< configuration< histogram2d > >();
//            config = fill_element( std::move(config), canvas_ph, canvas{} );
//            config = fill_element( std::move(config), canvas_ph, histogram2d{} );
//            write( output_filename_p, std::move(config)  );
//            break;
//        }
                //legend should always comme after histogram
        case 0b111:{
            auto config = make_image< configuration< histogram1d, frame1d, legend, pave_text > >();
            config = fill(std::move(config), canvas_ph);
            write( output_filename_p, std::move(config)  );
            break;
        }
                //legend should always comme after histogram
        case 0b110:{
            auto config = make_image< configuration< histogram1d, frame1d, legend > >();
            config = fill(std::move(config), canvas_ph);
            write( output_filename_p, std::move(config)  );
            break;
        }
        case 0b101:{
            auto config = make_image< configuration< histogram1d, frame1d, pave_text > >();
            config = fill(std::move(config), canvas_ph);
            write( output_filename_p, std::move(config)  );
            break;
        }
        case 0b100:{
            auto config = make_image< configuration< histogram1d, frame1d > >();
            config = fill(std::move(config), canvas_ph);
            write( output_filename_p, std::move(config)  );
            break;
        }
        default: {
            std::cerr << "given configuration of canvas as not been implemented yet\n";
        }
        }
        
        
        
    }
    
    
    
} // namespace iwir
