
#ifndef __ShapeAnalyzer__ShapeInfoContextMenuItem__
#define __ShapeAnalyzer__ShapeInfoContextMenuItem__

#include <string>

#include <QMessageBox>

#include "CustomContextMenuItem.h"

#include "ui_ShapeInfoDialog.h"


using namespace std;

namespace custom {
namespace contextMenuItems {

class ShapeInfoContextMenuItem : public CustomContextMenuItem {
public:
    ShapeInfoContextMenuItem(shared_ptr<Shape> shape, ShapeAnalyzerInterface* shapeAnalyzer) : CustomContextMenuItem(shape, shapeAnalyzer) {}
    
    virtual void onClick(vtkIdType pointId, vtkIdType faceId, QWidget* parent);
};
    
}
}

#endif /* defined(__ShapeAnalyzer__ShapeInfoContextMenuItem__) */
