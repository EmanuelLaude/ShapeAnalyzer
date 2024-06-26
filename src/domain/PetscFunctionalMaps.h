#ifndef __ShapeAnalyzer__PetscFunctionalMaps__
#define __ShapeAnalyzer__PetscFunctionalMaps__

#include <iostream>

#include "FunctionalMaps.h"

#include "laplaceBeltrami/PetscLaplaceBeltramiOperator.h"

#include "PetscHelper.h"

#include <slepceps.h>

using namespace std;
using namespace laplaceBeltrami;

/// \brief Concrete implementation of the FunctionalMaps class using the PETSc framework.
/// \details Functional maps correspondence is computed using a sparsity prior optimized via a forward-backward splitting algorithm as proposed by Pokrass et al. in "Sparse Modeling of Intrinsic Correspondences" which appeared in 2013.
class PetscFunctionalMaps : public FunctionalMaps {
public:
    /// \brief Constructor
    /// @param shared_ptr<Shape> Source shape.
    /// @param shared_ptr<Shape> Target shape.
    /// @param PetscLaplaceBeltramiOperator* Laplacian of source shape.
    /// @param PetscLaplaceBeltramiOperator* Laplacian of target shape.
    /// @param vector<vtkSmartPointer<vtkDoubleArray>>& Vector of constraints (Vertex-to-Double maps) defined on the source shape.
    /// @param vector<vtkSmartPointer<vtkDoubleArray>>& Vector of constraints (Vertex-to-Double maps) defined on the target shape.
    /// @param int Number of eigenfunctions that are used for the representation of the functions.
    /// @param double optional argument for the step size of the forward backward splitting algorithm
    /// @param double optional argument controlling the weight of the sparsity penalizer
    /// @param int optional number of iterations
    /// @param bool optional argument controlling whether outlier constraints should be absorbed
    /// @param double mu optional argument controlling the weight of the outlier absorption
    /// @param function<void(int, double)> optional argument. Lambda expression that is executed on every iteration. The current iteration and the current residual are handed over as parameters.
    PetscFunctionalMaps(shared_ptr<Shape> shape1,
                        shared_ptr<Shape> shape2,
                        shared_ptr<PetscLaplaceBeltramiOperator> laplacian1,
                        shared_ptr<PetscLaplaceBeltramiOperator> laplacian2,
                        vector<vtkSmartPointer<vtkDoubleArray>>& c1,
                        vector<vtkSmartPointer<vtkDoubleArray>>& c2,
                        int numberOfEigenfunctions,
                        double alpha = 1e2,
                        double lambda = 0.019,
                        int iterations = 300,
                        bool outliers = false,
                        double mu = 0.07,
                        function<void(int, double)> iterationCallback = [](int, double)->void {}
                        );
    
    virtual vtkSmartPointer<vtkDoubleArray> transferFunction(vtkSmartPointer<vtkDataArray> f);
    
    virtual void computeCorrespondence();
    
    /// \brief Returns the outlier score of constraint i
    /// @param int index of the constraint that is queried.
    double getOutlierScore(int i);
    
    /// \brief Virtual destructor.
    ~PetscFunctionalMaps();
private:
    
    /// \brief weight of the sparsity penalizer
    PetscScalar lambda_;
    
    /// \brief forward-backward splitting step size
    PetscScalar alpha_;
    
    /// \brief number of iterations
    int iterations_;
    
    /// \brief outlier flag
    bool outliers_;
    
    /// \brief weight of outlier absorbance
    PetscScalar mu_;
    
    
    /// \brief Proximity operator of matrix C_k
    /// \details Stores the projected matrix C_k+1 in class member C_
    /// @param Temporary matrix C_k
    void proxOperator1(Mat* C);
    
    /// \brief Proximity operator of matrix O_k
    /// \details Stores the projected matrix O_k+1 in class member O_
    /// @param Temporary matrix O_k
    void proxOperator2(Mat* O);
    
    /// \brief Computes the matrix Phi^T * M that is used for further computations where Phi is the matrix containing the eigenfunctions as columns and M is the Mass matrix of the respective shape.
    void setupPhiTM(Shape* shape, PetscLaplaceBeltramiOperator* laplacian, Mat* Phi, Mat *PhiTM);
    
    
    /// \brief Correspondence in Functional Maps representation
    Mat C_;
    
    /// \brief Outlier matrix
    Mat O_;
    
    /// \brief Constraints of shape M projected onto eigenfunction basis as rows
    Mat A_;

    /// \brief Constraints of shape N projected onto eigenfunction basis as rows
    Mat B_;
    
    Mat W_;
    
    /// \brief Eigenfunctions of source shape stacked as columns into a matrix
    Mat Phi1_;
    
    /// \brief Eigenfunctions of target shape stacked as columns into a matrix
    Mat Phi2_;
    
    /// \brief the matrix {Phi_1}^T * M_M
    Mat PhiTM1_;
    
    /// \brief the matrix {Phi_2}^T * M_N
    Mat PhiTM2_;
    
    /// \brief Laplacian of source shape.
    shared_ptr<PetscLaplaceBeltramiOperator> laplacian1_;
    
    /// \brief Laplacian of source shape.
    shared_ptr<PetscLaplaceBeltramiOperator> laplacian2_;
    
    /// \brief Lambda expression that is executed on every iteration. The current iteration and the current residual are handed over as parameters.
    function<void(int, double)> iterationCallback_;
};


#endif /* defined(__ShapeAnalyzer__FunctionalMaps__) */

