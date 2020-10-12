//
//File      : iwir.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 11/09/2020
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//

#ifndef iwir_h
#define iwir_h


//iwir header


//std headers
#include <fstream>


//ROOT header
#include "TCanvas.h"


void save_configuration(TCanvas const* canvas_p, std::string output_filename_p );

void apply_configuration(std::string config_p, std::string hist_list_p);

void hello();

namespace iwir {
    
    
    

//TO DO: add name output also + bundle rather than configuration image



} //namespace iwir

#endif /* iwir_h */
