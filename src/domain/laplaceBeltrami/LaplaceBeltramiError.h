#ifndef ShapeAnalyzer_LaplaceBeltramiError_h
#define ShapeAnalyzer_LaplaceBeltramiError_h

#include "../../util/Error.h"

namespace laplaceBeltrami {
    
/// \brief Specific class for errors occurring while calculation the Laplace Beltrami operator.
/// @author Emanuel Laude and Zorah Lähner
class LaplaceBeltramiError : public util::Error {
    public:
        LaplaceBeltramiError() : Error() {}
        LaplaceBeltramiError(const std::string& str) : Error(str) {}
        ~LaplaceBeltramiError() {}
    };
}

#endif
