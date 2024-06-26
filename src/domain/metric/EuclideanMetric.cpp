#include "EuclideanMetric.h"
#include <limits>


///////////////////////////////////////////////////////////////////////////////
// Public Functions from abstract class Metric
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
double metric::EuclideanMetric::getDistance(vtkIdType a, vtkIdType b) {
    // argument check
    if(a >= shape_->getPolyData()->GetPoints()->GetNumberOfPoints()
       || b >= shape_->getPolyData()->GetPoints()->GetNumberOfPoints()) {
        throw invalid_argument("Source point (" + to_string(a) + " or " + to_string(b) + ") larger than number of points (" + to_string(shape_->getPolyData()->GetPoints()->GetNumberOfPoints()) + ") in " + __PRETTY_FUNCTION__);
    }
    
    double p1[3], p2[3];
    shape_->getPolyData()->GetPoints()->GetPoint(a, p1);
    shape_->getPolyData()->GetPoints()->GetPoint(b, p2);
    
    return sqrt(pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2) + pow(p1[2] - p2[2], 2));
}

///////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkDoubleArray> metric::EuclideanMetric::getAllDistances(vtkIdType source) {
    vtkSmartPointer<vtkDoubleArray> distances = vtkSmartPointer<vtkDoubleArray>::New();
    distances->SetNumberOfValues(shape_->getPolyData()->GetNumberOfPoints());
    
    for(vtkIdType i = 0; i < shape_->getPolyData()->GetPoints()->GetNumberOfPoints(); i++) {
        distances->SetValue(i, getDistance(source, i));
    }
    
    return distances;
}

///////////////////////////////////////////////////////////////////////////////
vtkIdType metric::EuclideanMetric::getFarthestPoint(vtkSmartPointer<vtkIdList> sources) {
    double maxDist = 0;
    vtkIdType id;
    
    // iterate over all points on the shape
    for(vtkIdType i = 0; i < shape_->getPolyData()->GetPoints()->GetNumberOfPoints(); i++) {
        double dist = std::numeric_limits<double>::infinity();
        // iterate over all sources
        for (vtkIdType j = 0; j < sources->GetNumberOfIds(); j++) {
            
            // test for minimum distance of sources
            double d = getDistance(i, sources->GetId(j));
            if (d < dist)
                dist = d;
        }
        
        // test for maximum distance over all points
        if (dist > maxDist) {
            maxDist = dist;
            id = i;
        }
    }
    
    return id;
}


///////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkIntArray> metric::EuclideanMetric::getVoronoiCells(vtkSmartPointer<vtkIdList> seeds) {
    
    if(seeds->GetNumberOfIds() == 0) {
        throw invalid_argument(string("Input list empty in ").append(__PRETTY_FUNCTION__));
    }
    
    vtkSmartPointer<vtkIntArray> voronoiCells = vtkSmartPointer<vtkIntArray>::New();
    voronoiCells->SetNumberOfValues(shape_->getPolyData()->GetPoints()->GetNumberOfPoints());
    

    for(vtkIdType i = 0; i < shape_->getPolyData()->GetPoints()->GetNumberOfPoints(); i++) {
        double minDist = std::numeric_limits<double>::infinity();
        for(vtkIdType j = 0; j < seeds->GetNumberOfIds(); j++) {
            double dist = getDistance(seeds->GetId(j), i);
            if(dist < minDist) {
                minDist = dist;
                voronoiCells->SetValue(i, j);
            }
        }
            
    }
    
    return voronoiCells;
}
