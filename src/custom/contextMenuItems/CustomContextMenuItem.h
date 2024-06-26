#ifndef ShapeAnalyzer_CustomContextMenuItem_h
#define ShapeAnalyzer_CustomContextMenuItem_h

#include "../../domain/Shape.h"
#include "../../view/ShapeAnalyzerInterface.h"

#include <QWidget>

namespace custom {
/// \brief Namespace for all custom context menu items.
namespace contextMenuItems {

/// \brief Abstract class for displaying a menu item in the right-click context menu of each shape.
/// \details Concrete menu items have to be registered in RegisterCustomClasses to be connected with the GUI.
class CustomContextMenuItem {
    
public:
    /// \brief Empty constructor.
    CustomContextMenuItem(shared_ptr<Shape> shape, ShapeAnalyzerInterface* shapeAnalyzer) : shape_(shape), shapeAnalyzer_(shapeAnalyzer) {}
    
    /// \brief This function is called when the menu item is clicked.
    /// \details Concrete classes implement this method and perform a specific task in it.
    /// @param vtkIdType The vertex ID at the position of the mouse if there is any. Needed e.g. for ExtractSegmentCustomContextMenuItem.
    /// @param vtkIdType The face ID at the position of the mouse.
    /// @param QWidget* The QWidget parent.
    virtual void onClick(vtkIdType pointId, vtkIdType faceId, QWidget* parent) = 0;
    
protected:
    /// \brief Shape* The shape for which the context menu item has been clicked.
    shared_ptr<Shape> shape_;
    /// \brief Reference to the ShapeAnalyzerInterface.
    ShapeAnalyzerInterface* shapeAnalyzer_;
};
    
}
}

#endif
