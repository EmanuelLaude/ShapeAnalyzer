#include "PetscFEMLaplaceBeltramiOperator.h"

///////////////////////////////////////////////////////////////////////////////
laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::PetscFEMLaplaceBeltramiOperator(shared_ptr<Shape> shape, int numberOfEigenfunctions) : PetscLaplaceBeltramiOperator(shape, numberOfEigenfunctions) {
    PetscErrorCode ierr;
    
    setupMatrices();
    
    // Create eigensolver context
    ierr = EPSCreate(PETSC_COMM_WORLD, &eps_);
    
    // Set operators. In this case, it is a generalized eigenvalue problem
    ierr = EPSSetOperators(eps_, L_, M_);
    
    
    // Set solver parameters at runtime
    // type: generalized hermitian
    EPSSetProblemType(eps_, EPS_GHEP);
    EPSSetTarget(eps_, -1e-5);
    EPSSetWhichEigenpairs(eps_, EPS_TARGET_MAGNITUDE);
    //EPSSetType(eps_, EPSARNOLDI);
    //EPSSetType(eps_, EPSARPACK);
    vtkIdType numberOfPoints = shape_->getPolyData()->GetNumberOfPoints();
    
    EPSSetDimensions(eps_, min((vtkIdType) numberOfEigenfunctions_, numberOfPoints), PETSC_DECIDE, PETSC_DECIDE);

    ierr = EPSGetST(eps_, &st_);
    STSetType(st_, STSINVERT);
    //BV bv;
    //EPSGetBV(eps_, &bv);
    //BVSetOrthogonalization(bv, BV_ORTHOG_CGS, BV_ORTHOG_REFINE_IFNEEDED, 0.7071);
    
    EPSSetFromOptions(eps_);
    // Solve the eigensystem
    ierr = EPSSolve(eps_);
    if(ierr != 0) {
        throw LaplaceBeltramiError(string("Error occured in computation of Laplace-Beltrami eigenfunction (Probably due to corrupted shape)").append(" in ").append(__PRETTY_FUNCTION__));
    }
    
    ierr = EPSView(eps_, PETSC_VIEWER_STDOUT_SELF);
    ierr = EPSPrintSolution(eps_, NULL);
}


///////////////////////////////////////////////////////////////////////////////
laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::~PetscFEMLaplaceBeltramiOperator() {
    PetscErrorCode ierr;
    
    ierr = EPSDestroy(&eps_);
    ierr = MatDestroy(&L_);
    ierr = MatDestroy(&M_);
}


///////////////////////////////////////////////////////////////////////////////
Mat* laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::getMassMatrix() {
    return &M_;
}


///////////////////////////////////////////////////////////////////////////////
Mat* laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::getStiffnessMatrix() {
    return &L_;
}


///////////////////////////////////////////////////////////////////////////////
void laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::getNnz(PetscInt *nnz, vtkIdType numberOfPoints, vtkIdType numberOfFaces) {

    set<vtkIdType>* adjacencyList = new set<vtkIdType>[numberOfPoints];
    for(vtkIdType i = 0; i < numberOfFaces; i++) {
        vtkTriangle* face = reinterpret_cast<vtkTriangle*>(shape_->getPolyData()->GetCell(i));
        
        
        for(int i = 0; i < 3; i++) {
            int j = (i + 1) % 3;
            int k = (i + 2) % 3;
            
            adjacencyList[face->GetPointId(i)].insert(face->GetPointId(j));
            adjacencyList[face->GetPointId(i)].insert(face->GetPointId(k));
        }
        
    }
    
    for(vtkIdType i = 0; i < numberOfPoints; i++) {
        nnz[i] = 1 + adjacencyList[i].size();
    }
    delete [] adjacencyList;
}


///////////////////////////////////////////////////////////////////////////////
PetscScalar laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::getMass(double *a, double *b, double *c) {
    return vtkTriangle::TriangleArea(a, b, c) / 12.0;
}


///////////////////////////////////////////////////////////////////////////////
PetscScalar laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::getStiffness(double *a, double *b, double *c) {
    double ca[3];
    ca[0] = a[0] - c[0];
    ca[1] = a[1] - c[1];
    ca[2] = a[2] - c[2];
    
    double norm = sqrt(ca[0]*ca[0] + ca[1]*ca[1] + ca[2]*ca[2]);
    ca[0] /= norm;
    ca[1] /= norm;
    ca[2] /= norm;
    
    double cb[3];
    cb[0] = b[0] - c[0];
    cb[1] = b[1] - c[1];
    cb[2] = b[2] - c[2];
    
    double adjacent = cb[0]*ca[0] + cb[1]*ca[1] + cb[2]*ca[2];
    
    double opposite = sqrt(pow(c[0]+adjacent*ca[0]-b[0], 2) + pow(c[1]+adjacent*ca[1]-b[1], 2) + pow(c[2]+adjacent*ca[2]-b[2], 2));
    
    double cotan = adjacent / opposite;
    return 0.5 * cotan;
}


///////////////////////////////////////////////////////////////////////////////
//setup cotan and mass matrix for further computation (e.g. eigenfunctions)
void laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::setupMatrices() {
    PetscErrorCode ierr;

    vtkIdType numberOfPoints = shape_->getPolyData()->GetNumberOfPoints();
    vtkIdType numberOfFaces = shape_->getPolyData()->GetNumberOfCells();
    
    //compute number of non-zero elements per row (nnz is indexed by row index)
    PetscInt* nnz = new PetscInt[numberOfPoints];
    getNnz(nnz, numberOfPoints, numberOfFaces);
    
    //allocate spares matrices
    ierr = MatCreateSeqAIJ(PETSC_COMM_SELF, numberOfPoints, numberOfPoints, 0, nnz, &M_);
    ierr = MatSetOption(M_, MAT_SPD, PETSC_TRUE);
    
    ierr = MatCreateSeqAIJ(PETSC_COMM_SELF, numberOfPoints, numberOfPoints, 0, nnz, &L_);
    ierr = MatSetOption(L_, MAT_SYMMETRIC, PETSC_TRUE);
    
    //fill matrices with their values
    //iterate over all faces
    for(vtkIdType x = 0; x < numberOfFaces; x++) {
        vtkTriangle* face = reinterpret_cast<vtkTriangle*>(shape_->getPolyData()->GetCell(x));
        
        //iterate over all edges ij of triangle ijk. k always lies on the opposite side of edge ij
        for(int i = 0; i < 3; i++) {
            int j = (i + 1) % 3;
            int k = (i + 2) % 3;

            double a[3];
            face->GetPoints()->GetPoint(i, a);
            double b[3];
            face->GetPoints()->GetPoint(j, b);
            double c[3];
            face->GetPoints()->GetPoint(k, c);
            
            
            PetscScalar mass = getMass(a, b, c);
            ierr = MatSetValue(M_, face->GetPointId(i), face->GetPointId(j), mass, ADD_VALUES);
            ierr = MatSetValue(M_, face->GetPointId(j), face->GetPointId(i), mass, ADD_VALUES);
            ierr = MatSetValue(M_, face->GetPointId(i), face->GetPointId(i), mass, ADD_VALUES);
            ierr = MatSetValue(M_, face->GetPointId(j), face->GetPointId(j), mass, ADD_VALUES);
            
            PetscScalar stiffness = getStiffness(a, b, c);
            ierr = MatSetValue(L_, face->GetPointId(i), face->GetPointId(j), stiffness, ADD_VALUES);
            ierr = MatSetValue(L_, face->GetPointId(j), face->GetPointId(i), stiffness, ADD_VALUES);
            ierr = MatSetValue(L_, face->GetPointId(i), face->GetPointId(i), -stiffness, ADD_VALUES);
            ierr = MatSetValue(L_, face->GetPointId(j), face->GetPointId(j), -stiffness, ADD_VALUES);
        }
    }
    
    //assemble matrices
    ierr = MatAssemblyBegin(M_, MAT_FINAL_ASSEMBLY);
    ierr = MatAssemblyEnd(M_, MAT_FINAL_ASSEMBLY);
    

    
    ierr = MatAssemblyBegin(L_, MAT_FINAL_ASSEMBLY);
    ierr = MatAssemblyEnd(L_, MAT_FINAL_ASSEMBLY);
    
    delete [] nnz;
}


///////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkDoubleArray> laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::getEigenfunction(int i) {
    Vec vec;
    getEigenfunction(i, &vec);
    
    vtkSmartPointer<vtkDoubleArray> eigenfunction = PetscHelper::petscVecToVtkDoubleArray(vec);
    VecDestroy(&vec);
    
    return eigenfunction;
}


///////////////////////////////////////////////////////////////////////////////
void laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::getEigenfunction(PetscInt i, Vec* phi) {
    PetscErrorCode ierr;
    MatGetVecs(L_, NULL, phi);
    ierr = EPSGetEigenvector(eps_, i, *phi, NULL);
}


///////////////////////////////////////////////////////////////////////////////
double laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::getEigenvalue(int i) {
    PetscErrorCode ierr;
    PetscScalar lambda;
    ierr = EPSGetEigenvalue(eps_, i, &lambda, NULL);
    
    return lambda;
}


///////////////////////////////////////////////////////////////////////////////
void laplaceBeltrami::PetscFEMLaplaceBeltramiOperator::getEigenpair(PetscInt i, Vec* phi, PetscScalar* lambda) {
    PetscErrorCode ierr;
    MatGetVecs(L_, NULL, phi);
    ierr = EPSGetEigenpair(eps_, i, lambda, NULL, *phi, NULL);
}

