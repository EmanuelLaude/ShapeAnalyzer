#include "GeodesicMetric.h"

///////////////////////////////////////////////////////////////////////////////
// Constructors and Destructor
///////////////////////////////////////////////////////////////////////////////
metric::GeodesicMetric::GeodesicMetric(shared_ptr<Shape> shape) : Metric(shape) {
    points_ = new geodesicPoints(shape_->getPolyData());
    faces_ = new geodesicFaces(shape_->getPolyData());
    
    try {
    mesh_.initialize_mesh_data(*points_, *faces_);		//create internal mesh data structure including edges
    } catch (geodesic_error& e) {
        throw MetricError(string("The internal geodesic data structure was not build, this is probably due to a faulty mesh: ").append(e.what()).append(" in ").append(__PRETTY_FUNCTION__));
    }
    
    algorithm_ = new GeodesicAlgorithmExact(&mesh_);
}

///////////////////////////////////////////////////////////////////////////////
metric::GeodesicMetric::~GeodesicMetric() {
    delete algorithm_;
    delete points_;
    delete faces_;
}


///////////////////////////////////////////////////////////////////////////////
// Implemented Metric Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkDoubleArray> metric::GeodesicMetric::getAllDistances(vtkIdType s) {
    // argument check
    if(s >= points_->size()) {
        throw invalid_argument("Source point (" + to_string(s) + ") larger than number of points (" + to_string(points_->size()) + ") in " + __PRETTY_FUNCTION__);
    }
    
    // initialize algorithm for this source
    SurfacePoint source(&mesh_.vertices()[s]);
    vector<SurfacePoint> all_sources(1, source);
    try {
        algorithm_->propagate(all_sources);
    } catch (geodesic_error& e) {
        throw MetricError(string("The Geodesic failed to compute, this is probably due to a faulty mesh: ").append(e.what()).append(" in ").append(__PRETTY_FUNCTION__));
    }
    
    vtkSmartPointer<vtkDoubleArray> distances = vtkSmartPointer<vtkDoubleArray>::New();
    distances->SetNumberOfValues(shape_->getPolyData()->GetNumberOfPoints());
    for(vtkIdType i = 0; i< mesh_.vertices().size(); ++i) {
        geodesic::SurfacePoint p(&mesh_.vertices()[i]);
        
        double distance;
        try {
            algorithm_->best_source(p, distance); //for a given surface point, find closets source and distance to this source
        } catch (geodesic_error& e) {
            throw MetricError(string("The Geodesic failed to compute, this is probably due to a faulty mesh: ").append(e.what()).append(" in ").append(__PRETTY_FUNCTION__));
        }
        
        distances->SetValue(i, distance);
    }
    
    return distances;
}

///////////////////////////////////////////////////////////////////////////////
double metric::GeodesicMetric::getDistance(vtkIdType a, vtkIdType b) {
    // argument check
    if(a >= shape_->getPolyData()->GetPoints()->GetNumberOfPoints()
       || b >= shape_->getPolyData()->GetPoints()->GetNumberOfPoints()) {
        throw invalid_argument("Source point (" + to_string(a) + " or " + to_string(b) + ") larger than number of points (" + to_string(points_->size()) + ") in " + __PRETTY_FUNCTION__);
    }
    
    SurfacePoint source(&mesh_.vertices()[a]);
    SurfacePoint target(&mesh_.vertices()[b]);
    
    vector<SurfacePoint> path;
    
    try {
        algorithm_->geodesic(source, target, path);
    } catch (geodesic_error& e) {
        throw MetricError(string("The Geodesic failed to compute, this is probably due to a faulty mesh: ").append(e.what()).append(" in ").append(__PRETTY_FUNCTION__));
    }
    
    return length(path);
}


///////////////////////////////////////////////////////////////////////////////
vtkIdType metric::GeodesicMetric::getFarthestPoint(vtkSmartPointer<vtkIdList> sources) {
    vector<SurfacePoint> all_sources;
    all_sources.resize(sources->GetNumberOfIds());
    
    // create SurfacePoints for all ids
    for(int i = 0; i < sources->GetNumberOfIds(); i++) {
        // argument check
        if(sources->GetId(i) >= points_->size()) {
            throw invalid_argument("Source point (" + to_string(sources->GetId(i)) + ") larger than number of points (" + to_string(points_->size()) + ") in " + __PRETTY_FUNCTION__);
        }
        
        SurfacePoint source(&mesh_.vertices()[sources->GetId(i)]); //create source
        all_sources[i] = source;
    }
    
    // recomputation of geodesics
    try {
        algorithm_->propagate(all_sources);
    } catch (geodesic_error& e) {
        throw MetricError(string("The Geodesic failed to compute, this is probably due to a faulty mesh: ").append(e.what()).append(" in ").append(__PRETTY_FUNCTION__));
    }
    
    double maxDist = 0.0;
    
    vtkIdType id = 0;
    
    // iterate over all points
    for(int i = 0; i < mesh_.vertices().size(); i++) {
        SurfacePoint target(&mesh_.vertices()[i]); //create source
        
        // calculate shortest distance to some source
        double dist;
        try {
            algorithm_->best_source(target, dist);
        } catch (geodesic_error& e) {
            throw MetricError(string("The Geodesic failed to compute, this is probably due to a faulty mesh: ").append(e.what()).append(" in ").append(__PRETTY_FUNCTION__));
        }
        
        // check if shortest distance is the greatest so far
        if(dist > maxDist) {
            maxDist = dist;
            id = i;
        }
        
    }
    
    return id;
}

///////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkIntArray> metric::GeodesicMetric::getVoronoiCells(vtkSmartPointer<vtkIdList> seeds) {
    // argument check
    if(seeds->GetNumberOfIds() == 0) {
        throw invalid_argument(string("Input list empty in ").append(__PRETTY_FUNCTION__));
    }
    
    vtkSmartPointer<vtkIntArray> voronoiCells = vtkSmartPointer<vtkIntArray>::New();
    voronoiCells->SetNumberOfValues(shape_->getPolyData()->GetPoints()->GetNumberOfPoints());
    
    
    vector<SurfacePoint> all_sources;
    all_sources.resize(seeds->GetNumberOfIds());
    
    // create SurfacePoints for all ids
    for(int i = 0; i < seeds->GetNumberOfIds(); i++) {
        // argument check
        if(seeds->GetId(i) >= points_->size()) {
            throw invalid_argument("Source point (" + to_string(seeds->GetId(i)) + ") larger than number of points (" + to_string(points_->size()) + ") in " + __PRETTY_FUNCTION__);
        }
        
        SurfacePoint source(&mesh_.vertices()[seeds->GetId(i)]); //create source
        all_sources[i] = source;
    }
    
    // recomputation of geodesics
    try {
        algorithm_->propagate(all_sources);
    } catch (geodesic_error& e) {
        throw MetricError(string("The Geodesic failed to compute, this is probably due to a faulty mesh: ").append(e.what()).append(" in ").append(__PRETTY_FUNCTION__));
    }
    
    double dist;
    
    for(vtkIdType i = 0; i < shape_->getPolyData()->GetPoints()->GetNumberOfPoints(); i++) {
        SurfacePoint p(&mesh_.vertices()[i]);
        try {
        voronoiCells->SetValue(i, algorithm_->best_source(p, dist));
        } catch(geodesic_error& e) {
            throw MetricError(string("The Geodesic failed to compute, this is probably due to a faulty mesh: ").append(e.what()).append(" in ").append(__PRETTY_FUNCTION__));
        }
    }
    
    return voronoiCells;
}



