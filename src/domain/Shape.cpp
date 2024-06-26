#include "Shape.h"


///////////////////////////////////////////////////////////////////////////////
// Constructors and Destructor (and related functions)
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
Shape::Shape(
             vtkIdType id,
             string name,
             vtkSmartPointer<vtkPolyData> polyData,
             vtkSmartPointer<vtkRenderer> renderer
             )
: id_(id), name_(name), polyData_(polyData), renderer_(renderer) {

    
    //Visualize with normals. Looks smoother ;)
    polyDataNormals_ = vtkSmartPointer<vtkPolyDataNormals>::New();
    polyDataNormals_->SetInputData(polyData_);
    polyDataNormals_->ComputeCellNormalsOn();
    polyDataNormals_->Update();
    
    actor_ = vtkSmartPointer<vtkActor>::New();
    mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper_->SetInputData(polyData_);
    
    vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
    lookupTable->SetTableRange(1.0, 1.0);
    lookupTable->SetHueRange(0.667, 0.0);
    lookupTable->Build();
    mapper_->SetLookupTable(lookupTable);
    
    actor_->SetMapper(mapper_);
    renderer_->AddActor(actor_);
    
    
    // set up box widget
    boxWidget_ = vtkSmartPointer<vtkBoxWidget2>::New();
    vtkSmartPointer<vtkBoxRepresentation> boxRepresentation = vtkSmartPointer<vtkBoxRepresentation>::New();
    boxRepresentation->HandlesOff();
    boxRepresentation->SetPlaceFactor(1.25);
    boxRepresentation->PlaceWidget(actor_->GetBounds());
    boxWidget_->SetRepresentation(boxRepresentation);
    boxWidget_->SetInteractor(renderer_->GetRenderWindow()->GetInteractor());
}


///////////////////////////////////////////////////////////////////////////////
Shape::Shape(vtkIdType id, string name, vtkSmartPointer<vtkPolyData> polyData, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkMatrix4x4> matrix) : Shape(id, name, polyData, renderer)
{
    actor_->SetUserMatrix(matrix);
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->SetMatrix(matrix);
    static_cast<vtkBoxRepresentation*>(boxWidget_->GetRepresentation())->SetTransform(transform);
}


///////////////////////////////////////////////////////////////////////////////
// Public Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void Shape::removeFromRenderer() {
    renderer_->RemoveActor(actor_);
    boxWidget_->SetInteractor(nullptr);
}


///////////////////////////////////////////////////////////////////////////////
void Shape::clearColoring() {
    coloring_.reset();
    mapper_->SetScalarModeToUsePointData();
    mapper_->SetColorModeToDefault();
    mapper_->ScalarVisibilityOff();
    mapper_->Modified();
    
    
    renderer_->GetRenderWindow()->Render();
}


///////////////////////////////////////////////////////////////////////////////
void Shape::modified() {
    polyData_->Modified();
    actor_->Modified();
    
    static_cast<vtkBoxRepresentation*>(boxWidget_->GetRepresentation())->PlaceWidget(polyData_->GetBounds());
    if(actor_->GetUserTransform() != nullptr) {
        static_cast<vtkBoxRepresentation*>(boxWidget_->GetRepresentation())->SetTransform((vtkTransform*) actor_->GetUserTransform());
    }
}


///////////////////////////////////////////////////////////////////////////////
void Shape::setVisualRepresentation(VisualRepresentation representation) {
    switch (representation) {
        case VisualRepresentation::MeshSurface:
            mapper_->SetInputData(polyData_);
            actor_->GetProperty()->SetRepresentationToSurface();
            actor_->GetProperty()->SetColor(1, 1, 1);
            actor_->Modified();
            
            break;
        case VisualRepresentation::InterpolatedNormals:
            mapper_->SetInputData(polyDataNormals_->GetOutput());
            actor_->GetProperty()->SetRepresentationToSurface();
            actor_->GetProperty()->SetColor(1, 1, 1);
            actor_->Modified();
            
            break;
            
        case VisualRepresentation::PointCloud:
            actor_->GetProperty()->SetPointSize(3);
            actor_->GetProperty()->SetRepresentationToPoints();
            actor_->GetProperty()->SetColor(0, 0, 1);
            actor_->Modified();
            
            break;
            
        case VisualRepresentation::Mesh:
            mapper_->SetInputData(polyData_);
            actor_->GetProperty()->SetRepresentationToWireframe();
            actor_->GetProperty()->SetColor(1, 1, 0);
            actor_->Modified();
            
            break;
        default:
            break;
    }
}


///////////////////////////////////////////////////////////////////////////////
void Shape::colorPointsCoordinates() {
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    vtkSmartPointer<vtkPoints> points = polyData_->GetPoints();
    
    // color values
    colors->SetNumberOfComponents(3);
    colors->SetNumberOfTuples(points->GetNumberOfPoints());
    colors->SetName("Colors");
    
    // calculate range
    double xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
    
    for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++) {
        double point[3];
        points->GetPoint(i, point);
        
        // determine range of points
        xmin = std::min(xmin, point[0]);
        ymin = std::min(ymin, point[1]);
        zmin = std::min(zmin, point[2]);
        xmax = std::max(xmax, point[0]);
        ymax = std::max(ymax, point[1]);
        zmax = std::max(zmax, point[2]);
    }
    
    double xOffset = 0, yOffset = 0, zOffset = 0;
    
    if (xmin < 0) {
        xOffset = xmin * (-1);
        xmin = xmin + xOffset;
        xmax = xmax + xOffset;
    }
    if (ymin < 0) {
        yOffset = ymin * (-1);
        ymin = ymin + yOffset;
        ymax = ymax + yOffset;
    }
    if (zmin < 0) {
        zOffset = zmin * (-1);
        zmin = zmin + zOffset;
        zmax = zmax + zOffset;
    }
    
    // calculate colors
    for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++) {
        double point[3];
        points->GetPoint(i, point);
        
        point[0] = (point[0] + xOffset) / xmax * 255;
        point[1] = (point[1] + yOffset) / ymax * 255;
        point[2] = (point[2] + zOffset) / zmax * 255;
        
        colors->SetTuple(i, point);
    }

    setColoring(colors, Coloring::Type::PointRgb);
}


///////////////////////////////////////////////////////////////////////////////
void Shape::colorFacesCoordinates() {
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    vtkSmartPointer<vtkPoints> points = polyData_->GetPoints();
    
    // color values
    colors->SetNumberOfComponents(3);
    colors->SetNumberOfTuples(polyData_->GetNumberOfCells());
    colors->SetName("Colors");
    
    // calculate range
    double xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
    
    for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++) {
        double point[3];
        points->GetPoint(i, point);
        
        // determine range of points
        xmin = std::min(xmin, point[0]);
        ymin = std::min(ymin, point[1]);
        zmin = std::min(zmin, point[2]);
        xmax = std::max(xmax, point[0]);
        ymax = std::max(ymax, point[1]);
        zmax = std::max(zmax, point[2]);
    }
    
    double xOffset = 0, yOffset = 0, zOffset = 0;
    
    if (xmin < 0) {
        xOffset = xmin * (-1);
        xmin = xmin + xOffset;
        xmax = xmax + xOffset;
    }
    if (ymin < 0) {
        yOffset = ymin * (-1);
        ymin = ymin + yOffset;
        ymax = ymax + yOffset;
    }
    if (zmin < 0) {
        zOffset = zmin * (-1);
        zmin = zmin + zOffset;
        zmax = zmax + zOffset;
    }
    
    // calculate colors
    for (vtkIdType i = 0; i < polyData_->GetNumberOfCells(); i++) {
        vtkSmartPointer<vtkIdList> pointIds = polyData_->GetCell(i)->GetPointIds();
        
        double point1[3], point2[3], point3[3], color[3];
        
        points->GetPoint(pointIds->GetId(0), point1);
        points->GetPoint(pointIds->GetId(1), point2);
        points->GetPoint(pointIds->GetId(2), point3);
        
        // interpolate points to get color
        color[0] = (point1[0] + point2[0] + point3[0]) / 3;
        color[1] = (point1[1] + point2[1] + point3[1]) / 3;
        color[2] = (point1[2] + point2[2] + point3[2]) / 3;
        
        // range colors from 0 to 255
        color[0] = (color[0] + xOffset) / xmax * 255;
        color[1] = (color[1] + yOffset) / ymax * 255;
        color[2] = (color[2] + zOffset) / zmax * 255;
        
        colors->SetTuple(i, color);
    }
    
    setColoring(colors, Coloring::Type::FaceRgb);
}


///////////////////////////////////////////////////////////////////////////////
double Shape::getArea() {
    double area = 0.0;
    // add up the areas of all triangles
    for (int i = 0; i < polyData_->GetNumberOfCells(); i++) {
        // store point coordinates in a, b, c
        vtkSmartPointer<vtkPoints> ids = polyData_->GetCell(i)->GetPoints();
        double a[3], b[3], c[3];
        ids->GetPoint(0, a);
        ids->GetPoint(1, b);
        ids->GetPoint(2, c);
        
        // side lengths
        double ab = sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2) + pow(a[2] - b[2], 2));
        double bc = sqrt(pow(c[0] - b[0], 2) + pow(c[1] - b[1], 2) + pow(c[2] - b[2], 2));
        double ca = sqrt(pow(a[0] - c[0], 2) + pow(a[1] - c[1], 2) + pow(a[2] - c[2], 2));
        
        double s = (ab + bc + ca) / 2;
        
        area = area + sqrt(s * (s - ab) * (s - bc) + (s - ca));
    }
    
    return area;
}


///////////////////////////////////////////////////////////////////////////////
vtkIdType Shape::getRandomPoint() {
    return std::rand() % polyData_->GetPoints()->GetNumberOfPoints();
}


///////////////////////////////////////////////////////////////////////////////
void Shape::setColoring(vtkSmartPointer<vtkDataArray> values, Coloring::Type type) {
    // release old shared_ptr. Decrements ref count and if there is no other object
    // referencing the coloring it will be destroyed.
    coloring_.reset();
    coloring_ = unique_ptr<Coloring>(new Coloring);
    coloring_->values = values;
    coloring_->type = type;
    
    // check if input array matches coloring type
    if(coloring_->type == Coloring::Type::PointScalar || coloring_->type == Coloring::Type::FaceScalar) {
        if(vtkDoubleArray::SafeDownCast (coloring_->values) == nullptr) {
            throw invalid_argument(string("The Coloring of type PointScalar or FaceScalar was not a vtkDoubleArray in ").append(__PRETTY_FUNCTION__));
        }
    } else if(coloring_->type == Coloring::Type::FaceSegmentation || coloring_->type == Coloring::Type::PointSegmentation) {
        if(vtkIntArray::SafeDownCast (coloring_->values) == nullptr) {
            throw invalid_argument(string("The Coloring of type PointSegmentation or FaceSegmentation was not a vtkIntArray in ").append(__PRETTY_FUNCTION__));
        }
    } else if(coloring_->type == Coloring::Type::FaceRgb || coloring_->type == Coloring::Type::PointRgb) {
        if(vtkUnsignedCharArray::SafeDownCast (coloring_->values) == nullptr) {
            throw invalid_argument(string("The Coloring of type PointRgb or FaceRgb was not a vtkUnsignedCharArray in ").append(__PRETTY_FUNCTION__).append(". Be aware that for RGB colorings only values between 0 and 255 are valid."));
        }
    }
    
    
    
    mapper_->ScalarVisibilityOn();
    if(coloring_->type == Coloring::Type::PointScalar || coloring_->type == Coloring::Type::PointSegmentation) {
        // argument check
        
        // wrong number of components
        if(coloring_->values->GetNumberOfComponents() != 1) {
            throw invalid_argument(string("The Coloring is of type PointScalar or PointSegmentation but does not have 1 component in").append(__PRETTY_FUNCTION__));
        }
        
        // wrong number of values
        if(coloring_->values->GetNumberOfTuples() != polyData_->GetPoints()->GetNumberOfPoints()) {
            throw invalid_argument(string("The number of coloring values does not match the number of vertices in").append(__PRETTY_FUNCTION__));
        }
        
        // set coloring
        double range[2];
        coloring_->values->GetRange(range);
        
        polyData_->GetPointData()->SetScalars(coloring_->values);
        polyData_->Modified();
        
        mapper_->SetScalarModeToUsePointData();
        mapper_->SetColorModeToMapScalars();
        mapper_->SetScalarRange(range[0], range[1]);
    } else if(coloring_->type == Coloring::Type::FaceScalar || coloring_->type == Coloring::Type::FaceSegmentation) {
        // argument check
        
        // wrong number of components
        if(coloring_->values->GetNumberOfComponents() != 1) {
            throw invalid_argument(string("The Coloring is of type FaceScalar or FaceSegmentation but does not have 1 component in").append(__PRETTY_FUNCTION__));
        }
        
        // wrong number of values
        if(coloring_->values->GetNumberOfTuples() != polyData_->GetNumberOfCells()) {
            throw invalid_argument(string("The number of coloring values does not match the number of faces in").append(__PRETTY_FUNCTION__));
        }
        
        // set coloring
        double range[2];
        coloring_->values->GetRange(range);
        
        polyData_->GetCellData()->SetScalars(coloring_->values);
        polyData_->Modified();
        
        mapper_->SetScalarModeToUseCellData();
        mapper_->SetColorModeToMapScalars();
        mapper_->SetScalarRange(range[0], range[1]);
    } else if(coloring_->type == Coloring::Type::PointRgb) {
        // argument check
        
        if(coloring_->values->GetNumberOfComponents() != 3) {
            throw invalid_argument(string("The Coloring is of type PointRgb but does not have 3 components in").append(__PRETTY_FUNCTION__));
        }
        if(coloring_->values->GetNumberOfTuples() != polyData_->GetPoints()->GetNumberOfPoints()) {
            throw invalid_argument(string("The number of coloring values does not match the number of vertices in").append(__PRETTY_FUNCTION__));
        }
        
        // set coloring
        polyData_->GetPointData()->SetScalars(coloring_->values);
        
        mapper_->SetScalarModeToUsePointData();
        mapper_->SetColorModeToDefault();
        mapper_->ScalarVisibilityOn();
    } else if(coloring_->type == Coloring::Type::FaceRgb) {
        // argument check

        if(coloring_->values->GetNumberOfComponents() != 3) {
            throw invalid_argument(string("The Coloring is of type FaceRgb but does not have 3 components in").append(__PRETTY_FUNCTION__));
        }
        if(coloring_->values->GetNumberOfTuples() != polyData_->GetNumberOfCells()) {
            throw invalid_argument(string("The number of coloring values does not match the number of faces in").append(__PRETTY_FUNCTION__));
        }
        
        // set coloring
        polyData_->GetCellData()->SetScalars(coloring_->values);
        
        mapper_->SetScalarModeToUseCellData();
        mapper_->ScalarVisibilityOn();
    } else {}
    
    mapper_->ScalarVisibilityOn();
    mapper_->Modified();
    
    polyDataNormals_->Update();
    polyDataNormals_->Modified();
    
    renderer_->GetRenderWindow()->Render();
}

