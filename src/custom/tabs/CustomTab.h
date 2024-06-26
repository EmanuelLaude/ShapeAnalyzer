#ifndef ShapeAnalyzer_CustomTab_h
#define ShapeAnalyzer_CustomTab_h

#include <vtkActor.h>

#include "../../view/ShapeAnalyzerInterface.h"

#include "../../domain/Shape.h"

#include "../../domain/correspondences/PointCorrespondence.h"
#include "../../domain/correspondences/FaceCorrespondence.h"

#include "../../util/HashMap.h"

using namespace util;

namespace custom {
/// \brief Namespace for all tabs.
namespace tabs {

///
///  \brief Abstract class for Tabs.
///  \details Concrete CustomTab classes have to inherit from this class. Moreover they have to define the event functions onShapeAdd(), onShapeEdit(), onShapeDelete() and onClear() that are called from ShapeAnalyzer if one of these events occurs. Usually these event functions serve the purpose to keep the widgets in the CustomTab classes up-to-data. E.g. a comboBox containing a list of shapes should contain all available shapes. So it has to be updated if a shape is added, deleted or renamed.
///
///  \author Emanuel Laude and Zorah Lähner
///
class CustomTab {
public:
    CustomTab(const HashMap<vtkActor*, shared_ptr<Shape>>& shapes,
              const HashMap<shared_ptr<PointCorrespondence>, bool>& pointCorrespondences,
              const HashMap<shared_ptr<FaceCorrespondence>, bool>& faceCorrespondences,
              ShapeAnalyzerInterface* shapeAnalyzer)
    : shapes_(shapes), pointCorrespondences_(pointCorrespondences),
    faceCorrespondences_(faceCorrespondences), shapeAnalyzer_(shapeAnalyzer) {}
    
    /// \brief Virtual Destructor
    virtual ~CustomTab() {}
    /// \brief Abstract event function called if a new shape is added.
    /// \details The function is triggered before the shape is added to the GUI
    /// or included in any data structures in the main application.
    /// \param shape Pointer to added shape.
    virtual void onShapeAdd(Shape* shape) = 0;
    /// \brief Abstract event function called if an existent shape is edited.
    /// \param shape Pointer to edited shape.
    virtual void onShapeEdit(Shape* shape) = 0;
    /// \brief Abstract event function called if an existent shape is deleted.
    /// \param shape Pointer to the shape that is deleted.
    virtual void onShapeDelete(Shape* shape) = 0;
    /// \brief Abstract event function called if all correspondences and shapes are cleared.
    virtual void onClear() = 0;
    
protected:
    /// \brief Creates an string identifier of the shape consisting of the id and the name.
    /// \details Should be used when identifing the shapes with a string because the
    /// pure name may not be unique.
    /// \return <id>:<name>
    QString getShapeIdentifier(Shape* shape) {
        QString label = QString::number(shape->getId());
        return label.append(QString::fromStdString(":"+shape->getName()));
    }
    
    /// \brief Extracts the id from a string produced by getShapeIdentifier.
    /// \details There is no further checking if the given string is valid.
    /// \return Id of the shape described by getShapeIdentfier
    vtkIdType getIdFromIdentifier(QString identifier) {
        return identifier.split(':')[0].toInt();
    }
    
    /// \brief Returns the shape with given id if it exists.
    /// \return Pointer to the shape with id i, nullptr if it does not exist
    shared_ptr<Shape> getShapeFromId(vtkIdType i) {
        shared_ptr<Shape> shape = nullptr;
        
        for(auto entry : shapes_) {
            if(i == entry.second->getId()) {
                shape = entry.second;
            }
        }
        
        return shape;
    }
    
    /// \brief Returns a list of all shape identifiers in shapes_.
    /// \return QStringList of all shape identfiers in shapes_.
    QStringList getShapeIdentifierList() {
        QStringList labels;
        for(auto entry : shapes_) {
            labels << getShapeIdentifier(entry.second.get());
        }
        return labels;
    }
    
    /// \brief Returns the shape corresponding to the identifier if it exists.
    /// \return Pointer to the shape with the identifier, nullptr if it does not exist
    shared_ptr<Shape> getShapeFromIdentifier(QString identifier) {
        return getShapeFromId(getIdFromIdentifier(identifier));
    }
    
    /// \brief Read-only reference to HashMap containing all Shape objects
    const HashMap<vtkActor*, shared_ptr<Shape>>& shapes_;
    /// \brief Read-only reference to HashMap containing all PointCorrespondence objects
    const HashMap<shared_ptr<PointCorrespondence>, bool>& pointCorrespondences_;
    /// \brief Read-only reference to HashMap containing all FaceCorrespondence objects
    const HashMap<shared_ptr<FaceCorrespondence>, bool>& faceCorrespondences_;

    /// \brief Pointer to the ShapeAnalyzer widget parent.
    ShapeAnalyzerInterface* shapeAnalyzer_;
};

}
}

#endif
