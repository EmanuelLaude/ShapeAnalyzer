#include "FaceCorrespondence.h"


///////////////////////////////////////////////////////////////////////////////
string correspondences::FaceCorrespondence::toString() {
    string str;
    
    str.append("Type: ");
    str.append("FaceCorrespondence");
    str.append("\n");
    
    str.append("ID: ");
    str.append(to_string(id_));
    str.append("\n");
    
    
    for(int i = 0; i < shapes_.size(); i++) {
        str.append("S");
        str.append(to_string(shapes_.at(i)->getId()));
        str.append(":");
        str.append(to_string(correspondingIds_.at(i)));
        if(i < shapes_.size()-1) {
            str.append("->");
        }
    }
    
    return str;
}