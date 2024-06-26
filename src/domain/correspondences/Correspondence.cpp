#include "Correspondence.h"


///////////////////////////////////////////////////////////////////////////////
void correspondences::Correspondence::addShape(shared_ptr<Shape> shape, vtkIdType correspondingId) {
    shapes_.push_back(shape);
    correspondingIds_.push_back(correspondingId);
}


///////////////////////////////////////////////////////////////////////////////
int correspondences::Correspondence::removeShape(Shape* shape) {
    if(shapes_.size() <= 2) {
        throw invalid_argument(string("Cannot remove a shape from a correspondence containing only 2 shapes in ").append(__PRETTY_FUNCTION__));
    }
    int i = 0;
    for(auto it = shapes_.begin(); it != shapes_.end(); i++) {
        if(it->get() == shape) {
            shapes_.erase(it);
            correspondingIds_.erase(correspondingIds_.begin() + i);
            return i;
        }
        it++;
    }
    return -1;
}


///////////////////////////////////////////////////////////////////////////////
void correspondences::Correspondence::clear() {
    correspondingIds_.clear();
    shapes_.clear();
}
