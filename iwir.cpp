//
//File      : iwir.cpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 11/09/2020
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//


#include "iwir.hpp"
#include "saver.hpp"
#include "configurator.hpp"

#include <iostream>

void save_configuration(TCanvas const* canvas_p, std::string output_filename_p = "default.config") {
    iwir::saver{}( canvas_p, output_filename_p );
}

void apply_configuration(std::string config_p, std::string hist_list_p) {
    iwir::configurator{}( config_p, hist_list_p );
}

void hello() {
    std::cout << "hello !\n";
}
