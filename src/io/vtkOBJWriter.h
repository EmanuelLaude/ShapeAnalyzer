#ifndef __vtkOBJWriter_h
#define __vtkOBJWriter_h

#include <vtkObjectFactory.h>
#include <vtkWriter.h>
#include <vtkErrorCode.h>
#include <vtkPolyData.h>
#include <vtkCell.h>
#include <vtkInformation.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>

#include <cstdio>
#include <cstdlib>

namespace io {

/// \brief Vtk class to save shapes into .obj format.
class vtkOBJWriter : public vtkWriter {
public:
    static vtkOBJWriter *New();
    vtkTypeMacro(vtkOBJWriter,vtkWriter)
    void PrintSelf(ostream& os, vtkIndent indent);
    
    vtkGetMacro(DecimalPrecision, int);
    vtkSetMacro(DecimalPrecision, int);
    
    vtkSetStringMacro(FileName);
    vtkGetStringMacro(FileName);
    
    vtkPolyData* GetInput();
    vtkPolyData* GetInput(int port);
protected:
    vtkOBJWriter();
    ~vtkOBJWriter();
    
    void WriteData();
    int FillInputPortInformation(int port, vtkInformation *info);
    
    
    int DecimalPrecision;
    char *FileName;
private:
    vtkOBJWriter(const vtkOBJWriter&);  // Not implemented.
    void operator=(const vtkOBJWriter&);  // Not implemented.
};

}

#endif
