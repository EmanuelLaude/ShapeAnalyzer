#ifndef vtkToscaReader_h
#define vtkToscaReader_h

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
#include <vtkTriangle.h>


#include <regex>
#include <sstream>
#include <fstream>

namespace io {

/// \brief Vtk class to load shapes from tosca .vert and .tri format.
class vtkToscaReader : public vtkPolyDataAlgorithm {
public:
    vtkTypeMacro(vtkToscaReader, vtkPolyDataAlgorithm);
    
    void PrintSelf(ostream& os, vtkIndent indent);
    
    static vtkToscaReader *New();
    
    vtkSetStringMacro(FileName);
    
    vtkGetStringMacro(FileName);
protected:
    vtkToscaReader();
    ~vtkToscaReader();
    int FillOutputPortInformation(int port, vtkInformation* info);
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
private:
    vtkToscaReader(const vtkToscaReader&);  // Not implemented.
    void operator=(const vtkToscaReader&);  // Not implemented.
    
    char* FileName;
};
    
}


#endif

