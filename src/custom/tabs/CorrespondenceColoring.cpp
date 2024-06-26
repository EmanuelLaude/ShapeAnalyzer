#include "CorrespondenceColoring.h"
#include <utility>

///////////////////////////////////////////////////////////////////////////////
custom::tabs::CorrespondenceColoring::CorrespondenceColoring(
                                               const HashMap<vtkActor*, shared_ptr<Shape>>& shapes,
                                               const HashMap<shared_ptr<PointCorrespondence>, bool>& pointCorrespondences,
                                               const HashMap<shared_ptr<FaceCorrespondence>, bool>&  faceCorrespondences,
                                               shared_ptr<Shape> reference)
: shapes_(shapes), pointCorrespondences_(pointCorrespondences), faceCorrespondences_(faceCorrespondences), reference_(reference)
{
    if(reference_ == nullptr) {
        reference_ = shapes_.begin()->second;
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::CorrespondenceColoring::showPointCorrespondences(
                            shared_ptr<vector<pair<vtkIdType, double>>> percentageMatched,
                            shared_ptr<vector<pair<vtkIdType, double>>> percentageMultiple)
{
    pointAttributes_.clear();
    
    // completely color reference shape
    reference_->colorPointsCoordinates();
    
    vtkIdType referenceId = reference_->getId();
    vtkSmartPointer<vtkUnsignedCharArray> referenceColors = vtkUnsignedCharArray::SafeDownCast(reference_->getColoring()->values);
    pointAttributes_.insert(make_pair(referenceId, referenceColors));
    
    // for evaluating matched points
    unordered_map<vtkIdType, shared_ptr<vector<int>>> matched;
    
    // initialize color arrays for all shapes
    for (auto entry : shapes_) {
        if(entry.second != reference_) {
            vtkSmartPointer<vtkUnsignedCharArray> array = vtkSmartPointer<vtkUnsignedCharArray>::New();
            array->SetNumberOfComponents(3);
            array->SetNumberOfTuples(entry.second->getPolyData()->GetPoints()->GetNumberOfPoints());
            array->SetName("Colors");
            pointAttributes_.insert(make_pair(entry.second->getId(), array));
            

            double white[3] = {255, 255, 255};
            for(vtkIdType i = 0; i < entry.second->getPolyData()->GetPoints()->GetNumberOfPoints(); i++) {
                array->SetTuple(i, white);
            }
            
            // if requested, evaluate matched points
            if (percentageMatched != nullptr || percentageMultiple != nullptr) {
                shared_ptr<vector<int>> mArray = make_shared<vector<int>>(entry.second->getPolyData()->GetPoints()->GetNumberOfPoints(), 0);
                matched.insert(make_pair(entry.second->getId(), mArray));
            }
        }
    }
    
    // look up colors for non-reference shapes
    for(auto entry : pointCorrespondences_) {
        const vector<shared_ptr<Shape>>& shapes = entry.first->getShapes();
        const vector<vtkIdType>& points = entry.first->getCorrespondingIds();
        
        // id of correspondence on reference shape
        vtkIdType referenceCorrespondence = -1;
        
        // check if reference is in correspondence
        for(int i = 0; i < shapes.size(); i++) {
            if (shapes.at(i)->getId() == referenceId) {
                referenceCorrespondence = points[i];
                break;
            }
        }
        
        // if reference found
        if(referenceCorrespondence != -1) {
            // look up color for each shape in correspondence
            for (int i = 0; i < shapes.size(); i++) {
                // dont color reference shape
                if (shapes.at(i)->getId() != referenceId) {
                    if(pointAttributes_.count(shapes.at(i)->getId()) != 0) {
                        vtkSmartPointer<vtkUnsignedCharArray> colors = pointAttributes_.find(shapes.at(i)->getId())->second;
                        // color of point is the same as the corresponding point on the reference shape has
                        double color[3];
                        referenceColors->GetTuple(referenceCorrespondence, color);
                        colors->SetTuple(points[i], color);
                        
                        // if requested, evaluate matched points
                        if (percentageMatched != nullptr || percentageMultiple != nullptr) {
                            shared_ptr<vector<int>> match = matched.find(shapes.at(i)->getId())->second;
                            match->at(points[i]) = match->at(points[i]) + 1;
                        }
                    }
                }
            }
        }
    }
    
    // color all shapes
    for (auto entry : shapes_) {
        if (entry.second != reference_) {
            entry.second->setColoring(pointAttributes_.find(entry.second->getId())->second, Shape::Coloring::Type::PointRgb);
            
            // if requested, evaluate matched points
            if (percentageMatched != 0 || percentageMultiple != 0) {
                shared_ptr<vector<int>> match = matched.find(entry.second->getId())->second;
                int numMatched = 0;
                int numMultiple = 0;
                
                // count matched / multiple matched points
                for (int i = 0; i < match->size(); i++) {
                    if (match->at(i) > 0)
                        numMatched++;
                    if (match->at(i) > 1)
                        numMultiple++;
                }
                
                // add percentage matched points
                if (percentageMatched != nullptr) {
                    double per = ((double) numMatched) * 100 / match->size();
                    percentageMatched->push_back(make_pair(entry.second->getId(), per));
                }
                
                // add percentage mulitple matched points
                if (percentageMultiple != nullptr) {
                    double per = ((double) numMultiple) * 100 / match->size();
                    percentageMultiple->push_back(make_pair(entry.second->getId(), per));
                }
            }
            
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void custom::tabs::CorrespondenceColoring::showFaceCorrespondences(
                                    shared_ptr<vector<pair<vtkIdType, double>>> percentageMatched,
                                    shared_ptr<vector<pair<vtkIdType, double>>> percentageMultiple)
{
    faceAttributes_.clear();
    
    // completely color reference shape
    reference_->colorFacesCoordinates();
    
    vtkIdType referenceId = reference_->getId();
    vtkSmartPointer<vtkUnsignedCharArray> referenceColors = vtkUnsignedCharArray::SafeDownCast(reference_->getColoring()->values);
    
    faceAttributes_.insert(make_pair(referenceId, referenceColors));
    
    // for evaluating matched points
    unordered_map<vtkIdType, shared_ptr<vector<int>>> matched;
    
    // initialize color arrays for all shapes
    for (auto entry : shapes_) {
        if(entry.second != reference_) {
            vtkSmartPointer<vtkUnsignedCharArray> array = vtkSmartPointer<vtkUnsignedCharArray>::New();
            array->SetNumberOfComponents(3);
            array->SetNumberOfTuples(entry.second->getPolyData()->GetNumberOfCells());
            array->SetName("Colors");
            faceAttributes_.insert(make_pair(entry.second->getId(), array));
            
            double white[3] = {255, 255, 255};
            for(vtkIdType i = 0; i < entry.second->getPolyData()->GetNumberOfCells(); i++) {
                array->SetTuple(i, white);
            }
            
            // if requested, evaluate matched points
            if (percentageMatched != nullptr || percentageMultiple != nullptr) {
                shared_ptr<vector<int>> mArray = make_shared<vector<int>>(entry.second->getPolyData()->GetNumberOfCells(), 0);
                matched.insert(make_pair(entry.second->getId(), mArray));
            }
        }
    }
    
    // look up colors for non-reference shapes
    for(auto entry : faceCorrespondences_) {
        vector<shared_ptr<Shape>> shapes = entry.first->getShapes();
        vector<vtkIdType> points = entry.first->getCorrespondingIds();
        
        // id of correspondence on reference shape
        vtkIdType referenceCorrespondence = -1;
        
        // check if reference is in correspondence
        for(int i = 0; i < shapes.size(); i++) {
            if (shapes.at(i)->getId() == referenceId) {
                referenceCorrespondence = points[i];
                break;
            }
        }
        
        if(referenceCorrespondence != -1) {
            // look up color for each shape in correspondence
            for (int i = 0; i < shapes.size(); i++) {
                // dont color reference shape
                if (shapes.at(i)->getId() != referenceId) {
                    if(faceAttributes_.count(shapes.at(i)->getId()) != 0) {
                        vtkSmartPointer<vtkUnsignedCharArray> colors = faceAttributes_.find(shapes.at(i)->getId())->second;
                        // color of point is the same as the corresponding point on the reference shape has
                        double color[3];
                        referenceColors->GetTuple(referenceCorrespondence, color);
                        colors->SetTuple(points[i], color);
                        
                        // if requested, evaluate matched points
                        if (percentageMatched != nullptr || percentageMultiple != nullptr) {
                            shared_ptr<vector<int>> match = matched.find(shapes.at(i)->getId())->second;
                            match->at(points[i]) = match->at(points[i]) + 1;
                        }
                    }
                }
            }
        }
    }
    
    // color all shapes
    for (auto entry : shapes_) {
        if(entry.second != reference_) {
            entry.second->setColoring(faceAttributes_.find(entry.second->getId())->second, Shape::Coloring::Type::FaceRgb);
            
            // if requested, evaluate matched points
            if (percentageMatched != 0 || percentageMultiple != 0) {
                shared_ptr<vector<int>> match = matched.find(entry.second->getId())->second;
                int numMatched = 0;
                int numMultiple = 0;
                
                // count matched / multiple matched points
                for (int i = 0; i < match->size(); i++) {
                    if (match->at(i) > 0)
                        numMatched++;
                    if (match->at(i) > 1)
                        numMultiple++;
                }
                
                // add percentage matched points
                if (percentageMatched != nullptr) {
                    double per = ((double) numMatched) * 100 / match->size();
                    percentageMatched->push_back(make_pair(entry.second->getId(), per));
                }
                
                // add percentage mulitple matched points
                if (percentageMultiple != nullptr) {
                    double per = ((double) numMultiple) * 100 / match->size();
                    percentageMultiple->push_back(make_pair(entry.second->getId(), per));
                }
            }
        }
    }
}