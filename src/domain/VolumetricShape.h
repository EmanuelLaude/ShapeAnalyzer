#ifndef __ShapeAnalyzer__VolumetricShape__
#define __ShapeAnalyzer__VolumetricShape__

#include <vtkIdList.h>
#include <vtkPolyData.h>

#include "Shape.h"
#include "3rdparty/tetgen/tetgen.h"

/// \brief Tetrahedralized representation of a volumetric shape.
class VolumetricShape {
public:
    VolumetricShape(Shape* shape);
    ~VolumetricShape() {};
    
protected:
    Shape* shape_;
    tetgenio in_, out_;
};

#endif /* defined(__ShapeAnalyzer__VolumetricShape__) */
