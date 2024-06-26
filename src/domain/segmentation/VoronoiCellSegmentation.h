#ifndef __ShapeAnalyzer__VoronoiCellSegmentation__
#define __ShapeAnalyzer__VoronoiCellSegmentation__

#include "Segmentation.h"
#include "../metric/Metric.h"
#include "../samplings/Sampling.h"

using namespace metric;
using namespace sampling;

namespace segmentation {
    /// \brief Class for the computation of Voronoi cells on a shape.
    class VoronoiCellSegmentation : public Segmentation {
    public:
        /// \brief Constructor.
        /// @param Shape* The shape on which a segmentation should be computed.
        /// @param Metric* A refernence to a metric on the referenced shape that is used for the computation of the Voronoi cells.
        /// @param Sampling* A sampling of point ids on the referenced shape that is used to obtain the seeds for the segments
        VoronoiCellSegmentation(shared_ptr<Shape> shape,
                                shared_ptr<Metric> metric,
                                shared_ptr<Sampling> sampling
                                ) : Segmentation(shape), metric_(metric), sampling_(sampling) {
            // argument check
            if(metric_ == nullptr) {
                throw invalid_argument(string("Null pointer input 'metric' in ").append(__PRETTY_FUNCTION__));
            }
            if(sampling_ == nullptr) {
                throw invalid_argument(string("Null pointer input 'sampling' in ").append(__PRETTY_FUNCTION__));
            }
        }
        
        /// \brief Virtual destructor
        virtual ~VoronoiCellSegmentation() {}
        
        virtual vtkSmartPointer<vtkIntArray> getSegments();
    private:
        /// \brief Reference to the metric used to calculate the voronoi cells.
        /// \details Must match the shape_ of this object
        shared_ptr<Metric> metric_;
        /// \brief Reference to the sampling the segmentation is based on.
        /// \details Must macth the shape_ of this object
        shared_ptr<Sampling> sampling_;
    };
    
}

#endif /* defined(__ShapeAnalyzer__VoronoiCellSegmentation__) */
