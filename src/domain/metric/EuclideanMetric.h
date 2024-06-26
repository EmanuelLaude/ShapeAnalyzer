#ifndef __ShapeAnalyzer__EuclideanMetric__
#define __ShapeAnalyzer__EuclideanMetric__

#include <iostream>

#include "Metric.h"
#include "../Shape.h"


#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace metric {

    ///
    /// \brief Class for the representation and computation of the euclidean metric on a Shape.
    /// \author Emanuel Laude and Zorah Lähner
    ///
    class EuclideanMetric : public Metric {
    public:
        EuclideanMetric(shared_ptr<Shape> shape) : Metric(shape) {}
        
        virtual vtkSmartPointer<vtkDoubleArray> getAllDistances(vtkIdType source);
        virtual double getDistance(vtkIdType a, vtkIdType b);
        virtual vtkIdType getFarthestPoint(vtkSmartPointer<vtkIdList> sources);
        virtual vtkSmartPointer<vtkIntArray> getVoronoiCells(vtkSmartPointer<vtkIdList> seeds);
    };
}


#endif /* defined(__ShapeAnalyzer__EuclideanMetric__) */
