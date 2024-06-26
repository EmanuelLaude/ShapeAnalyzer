#ifndef __ShapeAnalyzer__MeshChecker__
#define __ShapeAnalyzer__MeshChecker__

#include <vtkType.h>
#include <vtkCell.h>
#include <vtkPolyData.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkIdList.h>

#include "Shape.h"

using namespace std;

///
/// \brief The MeshChecker class has various functions to check attributes of a given shape.
/// \details The class uses the half edge structure for certain properties. It is implemented
/// as large 2d-array and needs a lot of time and memory.
/// \note Might change the array for something more optimal.
/// \author Emanuel Laude and Zorah Lähner
///
class MeshChecker {
public:
    /// \brief Empty Constructor
    MeshChecker(shared_ptr<Shape> shape);
    /// \brief Empty Destructor
    ~MeshChecker() {}
    
    /// \brief Checks the shape for borders.
    /// \details Borders are found by checking the symmetry of the halfedge structure.
    /// If the half edge structure does not exist, it will be created which is quite
    /// time consuming.
    /// The halfedge structure counts the directed edges between all vertices. A border
    /// is recognized if the total number of edges between two vertices is 1.
    /// @param borders Optional. If a pointer is given, the border segments will be returned as pairs of vertices.
    /// @return true if the shape has borders, false otherwise
    bool checkForBorders(vector<pair<vtkIdType, vtkIdType> >* borders = nullptr);
    /// \brief Checks the shape for inconsistent half-edge structure.
    /// \details Orientation mistakes are found by checking the symmetry of the halfedge structure.
    /// If the half edge structure does not exist, it will be created which is quite
    /// time consuming.
    /// The halfedge structure counts the directed edges between all vertices. An edge is considered
    /// inconsistent if the total sum of directed edges between two vertices greater than 2.
    /// @param inconsistent Optional. If a pointer is given, the inconsistent segments will be returned as pairs of vertices.
    /// @return true if the shape has wrongly oriented edges, false otherwise
    bool checkInconsistency(vector<pair<vtkIdType, vtkIdType> >* inconsistent = nullptr);
    /// \brief Checks the shape for wrongly orientied edges.
    /// \details Orientation mistakes are found by checking the symmetry of the halfedge structure.
    /// If the half edge structure does not exist, it will be created which is quite
    /// time consuming.
    /// The halfedge structure counts the directed edges between all vertices. An edge is considered
    /// wrongly oriented if the total sum of directed edges between two vertices is 2, but they are
    /// both directed in the same direction. This is not a 100% correct indicatior for wrong orientation.
    /// @param unoriented Optional. If a pointer is given, the wrongly oriented segments will be returned as pairs of vertices.
    /// @return true if the shape has wrongly oriented edges, false otherwise
    bool checkOrientation(vector<pair<vtkIdType, vtkIdType> >* unoriented = nullptr);
    /// \brief Checks if all cells are triangles.
    /// \details Checks if the number of vertices is 3 for all cells. Degenerated triangles are
    /// recognized as triangles.
    /// @param nontriangles Optional. Returns tupels of the cell id and the number of vertices of the non-triangles.
    /// @return true if all cells are triangles, false otherwise
    bool checkTriangulation(vector<pair<vtkIdType, vtkIdType> >* nontriangles = nullptr);
    /// \brief Returns the number of connected regions in the shape.
    int checkNumberOfRegions();
    
private:
    void createHalfEdgeStructure();
    
    /// Reference to the checked shape
    shared_ptr<Shape>                  shape_;
    /// \brief Representation of the half-edge data structure.
    /// \details 1 in the position [i,j] there is a half edge from i to j. 0 there is no half edge.
    /// \note Might change the vectors for something more optimal.
    vector<shared_ptr<vector<int>>>   halfEdges_;
};

#endif /* defined(__ShapeAnalyzer__MeshChecker__) */
