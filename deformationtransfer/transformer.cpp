#include "transformer.h"
#include "surfaceInvMatrix.h"
#include <glm/glm.hpp>
#include "cholmod_builder.h"
#include "umfpack.h"
#include <iostream>

//Ist sehr ähnlich zu dem Transformer struct im github, aber nichts kopiert.
Transformer::~Transformer()
{
    try
    {
        cholmod_free_dense(&C, cm);
        cholmod_free_dense(&c, cm);
        cholmod_free_dense(&x, cm);
        cholmod_free_sparse(&At, cm);
        cholmod_free_sparse(&AtA, cm);
        umfpack_di_free_numeric(&umfpackNumericObj);
        corresMap.~map();
        sourceInvSurfaces.~vector();
    }
    catch (std::exception e)
    {
    }
}

Transformer::Transformer(Mesh *sourceRef,
                         Mesh *targetRef,
                         std::vector<Correspondence> CorresList) : source(sourceRef),
                                                                   target(targetRef),
                                                                   CorresList(CorresList)
{
    startCholmod();
    cholmod_triplet **A_triplet = (cholmod_triplet **)malloc(sizeof(*A_triplet));
    cholmod_sparse *A;
    void *symbolicObj;
    std::cout << "building dictionary from correslist for faster access" << std::endl;
    corresMap = CorresListToDict(CorresList);
    //calculate inverse surface matrices of sourceRef
    std::cout << "calculate inverse surface matrices" << std::endl;
    this->sourceInvSurfaces = calcSurfaceInvList(sourceRef);
    //allocate Memory
    std::cout << "allocate memory for linear equations" << std::endl;
    allocateMemory(targetRef, corresMap, A_triplet, &(this->C));
    this->x = cholmod_zeros((*A_triplet)->ncol, 1, CHOLMOD_REAL, cm);
    this->c = cholmod_zeros((*A_triplet)->ncol, 1, CHOLMOD_REAL, cm);

    //Build equations
    std::cout << "build coefficient matrix" << std::endl;
    buildCoefficientMatrix(targetRef, corresMap, *A_triplet);
    A = cholmod_triplet_to_sparse(*A_triplet, (*A_triplet)->nnz, cm);
    //cholmod_free_triplet(A_triplet, cm);
    this->At = cholmod_transpose(A, 2, cm);
    //cholmod_free_sparse(&A, cm);
    this->AtA = cholmod_aat(this->At, NULL, 0, 1, cm);
    cholmod_sort(this->AtA, cm);
    std::cout << "factorize coefficient matrix" << std::endl;
    umfpack_di_symbolic(
        (int)(this->AtA->nrow),
        (int)(this->AtA->ncol),
        (const int *)(this->AtA->p),
        (const int *)(this->AtA->i),
        (const double *)(this->AtA->x),
        &symbolicObj, NULL, NULL);
    umfpack_di_numeric(
        (const int *)(this->AtA->p),
        (const int *)(this->AtA->i),
        (const double *)(this->AtA->x),
        symbolicObj, &(this->umfpackNumericObj),
        NULL, NULL);
    umfpack_di_free_symbolic(&symbolicObj);
}
void Transformer::applyDeformation()
{
    //iterate over all targetVertices

    std::vector<glm::vec3> targetVertices = target->getVertices();
    for (int i = 0, row = 0; i < targetVertices.size(); i++)
    {
        glm::vec3 tempNewCoords;
        tempNewCoords.x = cholmod_getVarAt(this->x, row);
        row++;
        tempNewCoords.y = cholmod_getVarAt(this->x, row);
        row++;
        tempNewCoords.z = cholmod_getVarAt(this->x, row);
        row++;

        target->setVertex(i, tempNewCoords);
    }
    target->makeTrianglesConsistentWithVertices();
}

void Transformer::applyToTargetMesh(Mesh *sourceDeformed)
{
    std::cout << "build constraint vector" << std::endl;
    buildRhsVector(sourceDeformed, this->target, this->sourceInvSurfaces, this->corresMap, this->C);
    //calculate b = A'*c
    double alpha[2] = {1, 0};
    double beta[2] = {0, 0};
    std::cout << "calculate right side of equation: A' * c" << std::endl;
    cholmod_sdmult(At, 0, alpha, beta, C, c, cm);
    std::cout << "solve equation" << std::endl;
    umfpack_di_solve(UMFPACK_A,
                     (const int *)(this->AtA->p),
                     (const int *)(this->AtA->i),
                     (const double *)(this->AtA->x),
                     (double *)(this->x->x),
                     (const double *)(this->c->x),
                     this->umfpackNumericObj,
                     NULL, NULL);
    std::cout << "apply calculated deformation" << std::endl;
    applyDeformation();
}
