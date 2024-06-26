#ifndef vtkOFFReader_h
#define vtkOFFReader_h

#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkGenericCell.h>
#include <vtkCell.h>

#include <sstream>
#include <fstream>

namespace io {

/// \brief Vtk class to load shapes from .off format.
class vtkOFFReader : public vtkPolyDataAlgorithm {
public:
    vtkTypeMacro(vtkOFFReader, vtkPolyDataAlgorithm);
    
    void PrintSelf(ostream& os, vtkIndent indent);
    
    static vtkOFFReader *New();
    
    vtkSetStringMacro(FileName);
    
    vtkGetStringMacro(FileName);
protected:
    vtkOFFReader();
    ~vtkOFFReader();
    int FillOutputPortInformation(int port, vtkInformation* info);
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
private:
    vtkOFFReader(const vtkOFFReader&);  // Not implemented.
    void operator=(const vtkOFFReader&);  // Not implemented.
    
    char* FileName;
};
   
}

#endif
