
#ifndef __ShapeAnalyzer__ExtractSegmentContextMenuItem__
#define __ShapeAnalyzer__ExtractSegmentContextMenuItem__

#include <string>

#include "../../util/HashMap.h"

#include "CustomContextMenuItem.h"

#include "../../domain/laplaceBeltrami/LaplaceBeltramiOperator.h"

#include "../../view/ShapeAnalyzerInterface.h"

#include "../Factory.h"

#include <QMessageBox>

using namespace std;
using namespace util;

namespace custom {
namespace contextMenuItems {

class ExtractSegmentContextMenuItem : public CustomContextMenuItem {
public:
    ExtractSegmentContextMenuItem(shared_ptr<Shape> shape, ShapeAnalyzerInterface* shapeAnalyzer) : CustomContextMenuItem(shape, shapeAnalyzer) {}
    
    virtual void onClick(vtkIdType pointId, vtkIdType faceId, QWidget* parent);
};
    
}
}

#endif /* defined(__ShapeAnalyzer__ExtractSegmentContextMenuItem__) */
