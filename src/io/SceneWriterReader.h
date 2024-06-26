#ifndef __ShapeAnalyzer__SceneWriterReader__
#define __ShapeAnalyzer__SceneWriterReader__

#include "../domain/correspondences/Correspondence.h"
#include "../domain/correspondences/FaceCorrespondence.h"
#include "../domain/correspondences/PointCorrespondence.h"
#include "../domain/correspondences/VisualCorrespondence.h"
#include "../util/HashMap.h"

#include "../view/CustomListWidgetItem.h"

#include <QInputDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QObject>
#include <QString>

#include <vtkCamera.h>
#include <vtkHomogeneousTransform.h>
#include <vtkMatrix4x4.h>

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "IOError.h"

using namespace std;
using namespace correspondences;
using namespace util;

/// \brief Namespace including classes for saving and loading shapes, correspondences and scenes in different formats.
namespace io {

///
/// \brief Class for reading and writing whole scenes or lists of correspondences in Binary as well as in ASCII.
/// \details Provides functionality for both reading/writing of whole scenes including point and face correspondences and reading/writing of face or point correspondences separately.
/// \author Emanuel Laude and Zorah Lähner
///
class SceneWriterReader {
    
public:
    /// \brief Empty constructor.
    SceneWriterReader() {};
    
    /// \brief Empty destructor.
    ~SceneWriterReader() {};
    
    /// \brief Reads a scene including both shapes and correspondences from a binary file. It returns a vector of shapes two maps containing point and face correspondences respectively and the last used IDs for shapes and correspondences.
    /// @param string The name of the file that should be read.
    /// @param vtkSmartPointer<vtkRenderer> The VTK renderer object.
    /// @param vector<shared_ptr<Shape>>& Return parameter. A reference to an empty vector that will be filled with the shapes from the file.
    /// @param vector<pair<shared_ptr<PointCorrespondence>, shared_ptr<VisualCorrespondence<PointCorrespondence>>>>& Return parameter. A reference to an empty vector that will be filled with pairs of PointCorrespondence and VisualCorrespondence objects. If the correspondence is visible then the second member of the pair is not nullptr.
    /// @param vector<pair<shared_ptr<FaceCorrespondence>, shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>& Return parameter. A reference to an empty vector that will be filled with pairs of FaceCorrespondence and VisualCorrespondence objects. If the correspondence is visible then the second member of the pair is not nullptr.
    /// @return pair<int, int> Last insert IDs. First is the lastInsertShapeId. Second is the lastInsertCorrespondenceId.
    static pair<int, int> importSceneBinary(
                                            string filename,
                                            vtkSmartPointer<vtkRenderer> renderer,
                                            vector<shared_ptr<Shape>>& shapes,
                                            vector<pair<shared_ptr<PointCorrespondence>, shared_ptr<VisualCorrespondence<PointCorrespondence>>>>& pointCorrespondences,
                                            vector<pair<shared_ptr<FaceCorrespondence>, shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>& faceCorrespondences);
    /// \brief Exports a scene in binary including both shapes and correspondences. It takes an ordered vector of the shapes, two maps containing point face correspondences respectively and the last used IDs for shapes and correspondences as arguments.
    /// @param string The name of the file where the data should be written to.
    /// @param vtkSmartPointer<vtkRenderer> The VTK renderer object.
    /// @param vector<shared_ptr<Shape>>& A reference to a vector containing the shapes. It should be ordered.
    /// @param vector<pair<shared_ptr<PointCorrespondence>, shared_ptr<VisualCorrespondence<PointCorrespondence>>>>& A reference to a vector that that contains pairs of PointCorrespondence and VisualCorrespondence objects. If the correspondence is visible then the second member of the pair is not nullptr.
    /// @param vector<pair<shared_ptr<FaceCorrespondence>, shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>& A reference to a vector that that contains pairs of FaceCorrespondence and VisualCorrespondence objects. If the correspondence is visible then the second member of the pair is not nullptr.
    static void exportSceneBinary(string filename,
                                  vtkSmartPointer<vtkRenderer> renderer,
                                  vector<shared_ptr<Shape>>& shapes,
                                  int lastInsertShapeId,
                                  vector<pair<shared_ptr<PointCorrespondence>, shared_ptr<VisualCorrespondence<PointCorrespondence>>>>& pointCorrespondences,
                                  vector<pair<shared_ptr<FaceCorrespondence>, shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>& faceCorrespondences,
                                  int lastInsertCorrespondenceId);

    /// \brief Reads a scene including both shapes and correspondences from an ASCII file. It returns a vector of shapes two maps containing point and face correspondences respectively and the last used IDs for shapes and correspondences.
    /// @param string The name of the file that should be read.
    /// @param vtkSmartPointer<vtkRenderer> The VTK renderer object.
    /// @param vector<shared_ptr<Shape>>& Return parameter. A reference to an empty vector that will be filled with the shapes from the file.
    /// @param vector<pair<shared_ptr<PointCorrespondence>, shared_ptr<VisualCorrespondence<PointCorrespondence>>>>& Return parameter. A reference to an empty vector that will be filled with pairs of PointCorrespondence and VisualCorrespondence objects. If the correspondence is visible then the second member of the pair is not nullptr.
    /// @param vector<pair<shared_ptr<FaceCorrespondence>, shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>& Return parameter. A reference to an empty vector that will be filled with pairs of FaceCorrespondence and VisualCorrespondence objects. If the correspondence is visible then the second member of the pair is not nullptr.
    /// @return pair<int, int> Last insert IDs. First is the lastInsertShapeId. Second is the lastInsertCorrespondenceId.
    static pair<int, int> importSceneASCII(string filename,
                                           vtkSmartPointer<vtkRenderer> renderer,
                                           vector<shared_ptr<Shape>>& shapes,
                                           vector<pair<shared_ptr<PointCorrespondence>, shared_ptr<VisualCorrespondence<PointCorrespondence>>>>& pointCorrespondences,
                                           vector<pair<shared_ptr<FaceCorrespondence>, shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>& faceCorrespondences);
    
    /// Exports a scene in ASCII including both shapes and correspondences. It takes an ordered vector of the shapes, two maps containing point face correspondences respectively and the last used IDs for shapes and correspondences as arguments.
    /// @param string The name of the file where the data should be written to.
    /// @param vtkSmartPointer<vtkRenderer> The VTK renderer object.    
    /// @param vector<shared_ptr<Shape>>& A reference to a vector containing the shapes. It should be ordered.
    /// @param vector<pair<shared_ptr<PointCorrespondence>, shared_ptr<VisualCorrespondence<PointCorrespondence>>>>& A reference to a vector that that contains pairs of PointCorrespondence and VisualCorrespondence objects. If the correspondence is visible then the second member of the pair is not nullptr.
    /// @param vector<pair<shared_ptr<FaceCorrespondence>, shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>& A reference to a vector that that contains pairs of FaceCorrespondence and VisualCorrespondence objects. If the correspondence is visible then the second member of the pair is not nullptr.
    static void exportSceneASCII(string filename,
                                 vtkSmartPointer<vtkRenderer> renderer,
                                 vector<shared_ptr<Shape>>& shapes,
                                 int lastInsertShapeId,
                                 vector<pair<shared_ptr<PointCorrespondence>, shared_ptr<VisualCorrespondence<PointCorrespondence>>>>& pointCorrespondences,
                                 vector<pair<shared_ptr<FaceCorrespondence>, shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>& faceCorrespondences,
                                 int lastInsertCorrespondenceId);

    
    /// Exports a shape in ASCII file.
    /// @param string The name of the file where the data should be written to.
    /// @param Shape* The shape that should be exported.
    static void exportShapeASCII(string filename, Shape* shape);
    
    /// Exports a shape in ASCII stream.
    /// @param ostream& A reference to the output stream object.
    /// @param Shape* The shape that should be exported
    static void exportShapeASCII(ostream& os, Shape* shape);
    
    /// Exports a shape in Binary file.
    /// @param string The name of the file where the data should be written to.
    /// @param Shape* The shape that should be exported.
    static void exportShapeBinary(string filename, Shape* shape);
    
    /// Exports a shape in Binary stream.
    /// @param ostream& A reference to the output stream object.
    /// @param Shape* The shape that should be exported
    static void exportShapeBinary(ostream& os, Shape* shape);
    
    /// Imports a shape from ASCII file.
    /// @param string The filename of the shape that is imported.
    /// @param vtkSmartPointer<vtkRenderer> The renderer object that renders the shape.
    /// @return shared_ptr<Shape> The shape that has been read.
    static shared_ptr<Shape> importShapeASCII(string filename, vtkSmartPointer<vtkRenderer> renderer);
    
    /// Imports a shape from ASCII stream.
    /// @param istream& A reference to the input stream object.
    /// @param vtkSmartPointer<vtkRenderer> The renderer object that renders the shape.
    /// @return shared_ptr<Shape> The shape that has been read.
    static shared_ptr<Shape> importShapeASCII(istream& is, vtkSmartPointer<vtkRenderer> renderer);
    
    /// Imports a shape from Binary file.
    /// @param string The filename of the shape that is imported.
    /// @param vtkSmartPointer<vtkRenderer> The renderer object that renders the shape.
    /// @return shared_ptr<Shape> The shape that has been read.
    static shared_ptr<Shape> importShapeBinary(string filename, vtkSmartPointer<vtkRenderer> renderer);
    
    /// Imports a shape from Binary stream.
    /// @param istream& A reference to the input stream object.
    /// @param vtkSmartPointer<vtkRenderer> The renderer object that renders the shape.
    /// @return shared_ptr<Shape> The shape that has been read.
    static shared_ptr<Shape> importShapeBinary(istream& is, vtkSmartPointer<vtkRenderer> renderer);

    
    /// \brief Exports a list of point correspondences in ASCII. It takes a map containing PointCorrespondence objects a vector of the corresponding shapes ordered by ID and the filename as arguments.
    static void exportPointCorrespondencesASCII(
                                            const HashMap<shared_ptr<PointCorrespondence>, bool>&   pointCorrespondences,
                                            const vector<shared_ptr<Shape>>&                        shapesOrderedById,
                                            const string                                            filename
                                                );
    
    /// \brief Exports a list of face correspondences in ASCII. It takes a map containing PointCorrespondenceData objects a vector containing the corresponding shapes ordered by ID and the filename as arguments.
    static void exportFaceCorrespondencesASCII(
                                            const HashMap<shared_ptr<FaceCorrespondence>, bool>&    faceCorrespondences,
                                            const vector<shared_ptr<Shape>>&                        shapesOrderedById,
                                            const string                                            filename
                                               );
    
    
    /// \brief Reads two vectors of point and face correspondences respectively from an ASCII file.
    static void importCorrespondencesASCII(
                                        const string                                    filename,
                                        function<shared_ptr<PointCorrespondence>()>     pointCorrespondenceFactory,
                                        function<shared_ptr<FaceCorrespondence>()>      faceCorrespondenceFactory,
                                        vector<shared_ptr<PointCorrespondence>>&        pointCorrespondences,
                                        vector<shared_ptr<FaceCorrespondence>>&         faceCorrespondences,
                                        vector<shared_ptr<Shape>>&                      shapesOrderedById,
                                        QWidget*                                        parentWidget
                                           );

    
    /// \brief Exports a list of point correspondences in binary. It takes a map containing PointCorrespondenceData objects a vector containing the corresponding shapes ordered by ID and the filename as arguments.
    static void exportPointCorrespondencesBinary(
                                            const HashMap<shared_ptr<PointCorrespondence>, bool>&   pointCorrespondences,
                                            const vector<shared_ptr<Shape>>&                        shapesOrderedById,
                                            const string                                            filename
                                                );
    /// \brief Exports a list of face correspondences in binary. It takes a map containing FaceCorrespondenceData objects a vector the corresponding shapes ordered by ID and the filename as arguments.
    static void exportFaceCorrespondencesBinary(
                                            const HashMap<shared_ptr<FaceCorrespondence>, bool>&    faceCorrespondences,
                                            const vector<shared_ptr<Shape>>&                        shapesOrderedById,
                                            const string                                            filename
                                                );
    
    /// \brief Reads two vectors of point and face correspondences respectively from a binary file.
    static void importCorrespondencesBinary(
                                        const string                                    filename,
                                        function<shared_ptr<PointCorrespondence>()>     pointCorrespondenceFactory,
                                        function<shared_ptr<FaceCorrespondence>()>      faceCorrespondenceFactory,
                                        vector<shared_ptr<PointCorrespondence>>&        pointCorrespondences,
                                        vector<shared_ptr<FaceCorrespondence>>&         faceCorrespondences,
                                        vector<shared_ptr<Shape>>&                      shapesOrderedById,
                                        QWidget*                                        parentWidget
                                            );
};

}

#endif /* defined(__ShapeAnalyzer__SceneWriterReader__) */
