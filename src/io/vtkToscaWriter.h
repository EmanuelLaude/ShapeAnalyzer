#ifndef __ShapeAnalyzer__vtkToscaWriter__
#define __ShapeAnalyzer__vtkToscaWriter__

#include <vtkObjectFactory.h>
#include <vtkWriter.h>
#include <vtkErrorCode.h>
#include <vtkPolyData.h>
#include <vtkTriangle.h>
#include <vtkInformation.h>

#include <cstdio>
#include <cstdlib>
#include <regex>
#include <sstream>
#include <fstream>

namespace io {

/// \brief Vtk class to write shapes into tosca .vert and .tri format.
class vtkToscaWriter : public vtkWriter {
public:
    static vtkToscaWriter *New();
    vtkTypeMacro(vtkToscaWriter, vtkWriter)
    void PrintSelf(ostream& os, vtkIndent indent);
    
    vtkGetMacro(DecimalPrecision, int)
    vtkSetMacro(DecimalPrecision, int)
    
    // Description:
    // Specify file name of data file to write.
    vtkSetStringMacro(FileName)
    vtkGetStringMacro(FileName)
    
    // Description:
    // Get the input to this writer.
    vtkPolyData* GetInput();
    vtkPolyData* GetInput(int port);
    
protected:
    vtkToscaWriter();
    ~vtkToscaWriter();
    
    void WriteData();
    int FillInputPortInformation(int port, vtkInformation *info);
    
    int DecimalPrecision;
    char *FileName;
    
private:
    vtkToscaWriter(const vtkToscaWriter&);  // Not implemented.
    void operator=(const vtkToscaWriter&);  // Not implemented.
};
    
}

#endif /* defined(__ShapeAnalyzer__vtkToscaWriter__) */
