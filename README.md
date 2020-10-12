# iwir
IWIR Will Improve ROOT

IWIR is a simple project aiming to reduce the use of macros to produce graphical outputs using the ROOT (CERN) software librairies. It makes use of the c++14 standard. 

To compile and run, a working CMake installation is required ( version > 3.12 ), as well as c++14 ROOT installation ( version = 6.1x ). A typical installation will look like the following: 
   - mkdir build && cd build;
   - cmake ../path/to/source && make;
   - path/to/build/lib should then be added to either the LD_LIBRARY_PATH (Linux) or the DYLD_LIBRARY_PATH (macosX).

The rootlogon.c will be invoked by the commmand line interpreter at the start of the ROOT shell and will load the library (it should be found in the directory you are working from). This should enable, on principle, the root command line interpreter to access the few functions defined be IWIR. 
Once installed, two functions are available : 
  - save_configuration(const TCanvas* canvas_p, string output_filename_p), which takes a pointer to a ROOT TCanvas as an input as well as the name of the configuration file that will be generated accordingly
  - apply_configuration(string config_p, string hist_list_p), which takes the name of the configuration file to load into memory and apply on a list of histograms, defined in hist_list_p, this list should be separated by semi-colons in order to be read and found by IWIR's engine. 
  

As of now, IWIR is in its very-first version, i.e. v1.0-alpha. Therefore, as lot of work remains, a lot of bugs are bound to be found.
