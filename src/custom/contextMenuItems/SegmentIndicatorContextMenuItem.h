//
//  SegmentIndicatorContextMenuItem.h
//  ShapeAnalyzer
//
//  Created by Emanuel Laude on 24.12.14.
//
//

#ifndef __ShapeAnalyzer__SegmentIndicatorContextMenuItem__
#define __ShapeAnalyzer__SegmentIndicatorContextMenuItem__

#include <string>

#include <QMessageBox>

#include "CustomContextMenuItem.h"

#include "ui_ShapeInfoDialog.h"


using namespace std;

namespace custom {
    namespace contextMenuItems {
        
        class SegmentIndicatorContextMenuItem : public CustomContextMenuItem {
        public:
            SegmentIndicatorContextMenuItem(shared_ptr<Shape> shape, ShapeAnalyzerInterface* shapeAnalyzer) : CustomContextMenuItem(shape, shapeAnalyzer) {}
            
            virtual void onClick(vtkIdType pointId, vtkIdType faceId, QWidget* parent);
        };
        
    }
}

#endif /* defined(__ShapeAnalyzer__SegmentIndicatorContextMenuItem__) */
