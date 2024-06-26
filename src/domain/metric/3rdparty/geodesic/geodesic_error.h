//
//  geodesic_error.h
//  ShapeAnalyzer
//
//  Created by Zorah on 21.07.14.
//
//

#ifndef ShapeAnalyzer_geodesic_error_h
#define ShapeAnalyzer_geodesic_error_h

#include <string>
#include <exception>

class geodesic_error : exception {
public:
    geodesic_error() : what_("An error occured while calculating the geodesic.") {}
    geodesic_error(const std::string& str) : what_(str) {
    }
    
    virtual const char* what() const throw() { return what_.c_str(); }
    
private:
    std::string what_;
};


#endif
