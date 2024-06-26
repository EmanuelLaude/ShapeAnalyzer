#ifndef __ShapeAnalyzer__MetricSampling__
#define __ShapeAnalyzer__MetricSampling__

#include "Sampling.h"
#include "../metric/Metric.h"

using namespace metric;

namespace sampling {

///
/// \brief Abstract class for samplings that are based on a metric.
///
class MetricSampling : public Sampling {
public:
    /// \brief Basic constructor.
    /// @param Shape* The shape that is sampled.
    /// @param Metric* A refernce to the metric that is used for the computation of the sampled points.
    MetricSampling(shared_ptr<Shape> shape, shared_ptr<Metric> metric) : Sampling(shape), metric_(metric) {}
    
protected:
    shared_ptr<Metric> metric_;
};
    
}

#endif /* defined(__ShapeAnalyzer__MetricSampling__) */
