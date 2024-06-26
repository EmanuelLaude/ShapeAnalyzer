#ifndef ShapeAnalyzer_ShapeAnalyzerInteractrorStyle_h
#define ShapeAnalyzer_ShapeAnalyzerInteractrorStyle_h

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>

/// \brief Surpresses certain short cuts of the vtkWidget.
class ShapeAnalyzerInteractorStyle : public vtkInteractorStyleTrackballCamera {
public:
    static ShapeAnalyzerInteractorStyle* New();
    vtkTypeMacro(ShapeAnalyzerInteractorStyle, vtkInteractorStyleTrackballCamera);
    
    // Description:
    // OnChar is triggered when an ASCII key is pressed. Some basic key presses
    // are handled here ('q' for Quit, 'p' for Pick, etc)
    virtual void OnChar() {}
    
    // OnKeyDown is triggered by pressing any key (identical to OnKeyPress()).
    // An empty implementation is provided. The behavior of this function should
    // be specified in the subclass.
    virtual void OnKeyDown() {}
    
    // OnKeyUp is triggered by releaseing any key (identical to OnKeyRelease()).
    // An empty implementation is provided. The behavior of this function should
    // be specified in the subclass.
    virtual void OnKeyUp() {}
    
    // OnKeyPress is triggered by pressing any key (identical to OnKeyDown()).
    // An empty implementation is provided. The behavior of this function should
    // be specified in the subclass.
    virtual void OnKeyPress() {}
    
    // OnKeyRelease is triggered by pressing any key (identical to OnKeyUp()).
    // An empty implementation is provided. The behavior of this function should
    // be specified in the subclass.
    virtual void OnKeyRelease() {}
};

#endif