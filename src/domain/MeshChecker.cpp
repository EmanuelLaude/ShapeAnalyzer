#include "MeshChecker.h"

///////////////////////////////////////////////////////////////////////////////
// Constructor and Destructor
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
MeshChecker::MeshChecker(shared_ptr<Shape> shape) : shape_(shape) {
    if (shape_ == nullptr) {
        throw invalid_argument(string("Null pointer input 'shape' in ").append(__PRETTY_FUNCTION__));
    }
}


///////////////////////////////////////////////////////////////////////////////
// Public Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
bool MeshChecker::checkForBorders(vector<pair<vtkIdType, vtkIdType> >*  borders) {
    if(halfEdges_.empty()) {
        createHalfEdgeStructure();
    }
    
    bool borderFound = false;
    
    for (int i = 0; i < halfEdges_.size(); i++) {
        for (int j = 0; j < i; j++) {
            // unsymmetric edge
            if((*(halfEdges_[i]))[j] + (*(halfEdges_[j]))[i] == 1) {
                borderFound = true;
                
                // add border ids if requested
                if (borders != nullptr) {
                    // all borders are oriented the same way
                    if ((*(halfEdges_[i]))[j] > (*(halfEdges_[j]))[i]) {
                        borders->push_back(make_pair(i, j));
                    } else {
                        borders->push_back(make_pair(j, i));
                    }
                }
            }
        }
    }
    
    return borderFound;
}


///////////////////////////////////////////////////////////////////////////////
bool MeshChecker::checkOrientation(vector<pair<vtkIdType, vtkIdType> >*  unorientedEdges) {
    if(halfEdges_.empty()) {
        createHalfEdgeStructure();
    }
    
    bool unorientedEdgeFound = false;
    
    for (int i = 0; i < halfEdges_.size(); i++) {
        for (int j = 0; j < i; j++) {
            // symmetry if the sum of symmetric entries is 2
            if(((*(halfEdges_[i]))[j] + (*(halfEdges_[j]))[i] == 2)
               && ((*(halfEdges_[i]))[j] != (*(halfEdges_[j]))[i])) {
                unorientedEdgeFound = true;
                
                // add ids if requested
                if (unorientedEdges != nullptr) {
                    unorientedEdges->push_back(make_pair(i, j));
                }
            }
        }
    }
    
    return unorientedEdgeFound;
}

///////////////////////////////////////////////////////////////////////////////
bool MeshChecker::checkInconsistency(vector<pair<vtkIdType, vtkIdType> >*  inconsistentEdges) {
    if(halfEdges_.empty()) {
        createHalfEdgeStructure();
    }
    
    bool inconsistentEdgeFound = false;
    
    for (int i = 0; i < halfEdges_.size(); i++) {
        for (int j = 0; j < i; j++) {
            // symmetry if the sum of symmetric entries is 2
            if(((*(halfEdges_[i]))[j] + (*(halfEdges_[j]))[i] > 2)) {
                inconsistentEdgeFound = true;
                
                // add ids if requested
                if (inconsistentEdges != nullptr) {
                    inconsistentEdges->push_back(make_pair(i, j));
                }
            }
        }
    }
    
    return inconsistentEdgeFound;
}


///////////////////////////////////////////////////////////////////////////////
bool MeshChecker::checkTriangulation(vector<pair<vtkIdType, vtkIdType> >*  nonTriangles) {
    bool nonTriangleFound = false;
    
    // check if number of vertex ids is 3 for every cell
    for (vtkIdType i = 0; i < shape_->getPolyData()->GetNumberOfCells(); i++) {
        vtkSmartPointer<vtkIdList> ids = shape_->getPolyData()->GetCell(i)->GetPointIds();
        
        if(ids->GetNumberOfIds() != 3 && nonTriangles != nullptr) {
            nonTriangleFound = true;
            // returns id of cell and number of vertices
            nonTriangles->push_back(make_pair(i, ids->GetNumberOfIds()));
        }
    }
    
    return nonTriangleFound;
}


///////////////////////////////////////////////////////////////////////////////
int MeshChecker::checkNumberOfRegions() {
    vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
    vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
    
    connectivityFilter->SetInputData(shape_->getPolyData());
    connectivityFilter->Update();
    
    return connectivityFilter->GetNumberOfExtractedRegions();
}


///////////////////////////////////////////////////////////////////////////////
// Private Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void MeshChecker::createHalfEdgeStructure() {
    vtkIdType numberOfVertices = shape_->getPolyData()->GetPoints()->GetNumberOfPoints();
    
    halfEdges_.reserve(numberOfVertices);
    
    // create fake array with 0s everywhere
    for (vtkIdType i = 0; i < numberOfVertices; i++) {
        halfEdges_.push_back(make_shared<vector<int>>(numberOfVertices, 0));
    }
    
    // raise value [i, j] for every edge of an cell between the vertices i and j
    for (vtkIdType i = 0; i < shape_->getPolyData()->GetNumberOfCells(); i++) {
        vtkSmartPointer<vtkIdList> ids = shape_->getPolyData()->GetCell(i)->GetPointIds();
        
        // first n - 1 edges
        for(vtkIdType j = 0; j < ids->GetNumberOfIds(); j++) {
            vtkIdType id1 = ids->GetId(j);
            vtkIdType id2;
            if (j == ids->GetNumberOfIds() - 1) {
                id2 = ids->GetId(0);
            }
            else {
                id2 = ids->GetId(j+1);
            }
            
            (*halfEdges_[id1])[id2] = (*halfEdges_[id1])[id2] + 1;
        }
    }
}