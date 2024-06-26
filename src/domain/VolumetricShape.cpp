//
//  VolumetricShape.cpp
//  ShapeAnalyzer
//
//  Created by Zorah Lähner on 17.02.15.
//
//

#include "VolumetricShape.h"

///////////////////////////////////////////////////////////////////////////////
VolumetricShape::VolumetricShape(Shape* shape) : shape_(shape) {
    vtkSmartPointer<vtkPolyData> polyData = shape_->getPolyData();
    
    // copy all points
    in_.firstnumber = 1;
    in_.numberofpoints = polyData->GetNumberOfPoints();
    in_.pointlist = new REAL[in_.numberofpoints * 3];
    double point[3];
    for (int i = 0; i < polyData->GetNumberOfPoints(); i++) {
        polyData->GetPoints()->GetPoint(i, point);
        in_.pointlist[i * 3]     = point[0];
        in_.pointlist[i * 3 + 1] = point[1];
        in_.pointlist[i * 3 + 2] = point[2];
    }
    
    // copy all faces
    in_.numberoffacets = polyData->GetNumberOfCells();
    in_.facetlist = new tetgenio::facet[in_.numberoffacets];
    in_.facetmarkerlist = new int[in_.numberoffacets];
    
    tetgenio::facet *f;
    tetgenio::polygon *p;
    
    for (int i = 0; i < polyData->GetPolys()->GetNumberOfCells(); i++) {
        f = &in_.facetlist[i];
        f->numberofpolygons = 1;
        f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
        f->numberofholes = 0;
        f->holelist = NULL;
        p = &f->polygonlist[0];
        p->numberofvertices = 3;
        p->vertexlist = new int[p->numberofvertices];
        
        vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
        polyData->GetCellPoints(i, idList);
        
        p->vertexlist[0] = idList->GetId(0);
        p->vertexlist[1] = idList->GetId(1);
        p->vertexlist[2] = idList->GetId(2);
    }
    
    tetgenbehavior b = tetgenbehavior();
    tetrahedralize(&b, &in_, &out_);
    
    
}
