#ifndef ShapeAnalyzer_MetricError_h
#define ShapeAnalyzer_MetricError_h

#include "../../util/Error.h"

namespace metric {
    
    /// \brief Specific class for errors occurring with the metric calculation.
    /// @author Emanuel Laude and Zorah Lähner
    class MetricError : public util::Error {
    public:
        MetricError() : Error() {}
        MetricError(const std::string& str) : Error(str) {}
        ~MetricError() {}
    };
}
#endif
