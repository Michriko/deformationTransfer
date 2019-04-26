#include "cholmod_builder.h"
#include "surfaceInvMatrix.h"
#include "triangle.h"
#include <glm/glm.hpp>
#include <iostream>
//Ist sehr ähnlich zum github repo, hab es als vorlage genutzt, aber code ist nicht kopiert und etwas anders strukturiert.

cholmod_common common, *cm;

struct elementaryMatrix
{
    float x[9][4] = {{0}};
};
static void addCholmodEntry(cholmod_triplet *T, int i, int j, double x)
{

    if (T->nnz >= T->nzmax && !cholmod_reallocate_triplet(2 * (T->nzmax), T, cm))
    {
        return; //failed
    }
    if (T->itype == CHOLMOD_LONG)
    {
        ((SuiteSparse_long *)(T->i))[T->nnz] = i;
        ((SuiteSparse_long *)(T->j))[T->nnz] = j;
    }
    else
    {
        ((int *)(T->i))[T->nnz] = i;
        ((int *)(T->j))[T->nnz] = j;
    }
    T->dtype == 0 ? (((double *)(T->x))[T->nnz] = x)
                  : (((float *)(T->x))[T->nnz] = x);
    T->nnz += 1;
}

static elementaryMatrix calculateElementaryMatrix(glm::mat3x3 inverseSurfaceM)
{
    elementaryMatrix em;
    for (int row = 0, i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++, row++)
        {
            em.x[row][0] = -(inverseSurfaceM[j][0] + inverseSurfaceM[j][1] + inverseSurfaceM[j][2]);
            em.x[row][1] = inverseSurfaceM[j][0];
            em.x[row][2] = inverseSurfaceM[j][1];
            em.x[row][3] = inverseSurfaceM[j][2];
        }
    }
    return em;
}
static int getVariableIndex(Mesh *model, int triangleIndex, int vertexIndex, int dimension)
{
    Triangle t = model->getTriangles().at(triangleIndex);
    if (vertexIndex < 3)
    {
        int globalVertexIndex = t.getGlobalVertexIndex(vertexIndex);
        return (3 * globalVertexIndex) + dimension;
    }
    else
    {
        return 3 * (model->getVertices().size() + triangleIndex) + dimension;
    }
}
static int appendElementaryMatrixToLinearSystem(Mesh *model, cholmod_triplet *M, int triangleIndex, elementaryMatrix em, int i_row)
{
    int j_row = 0;
    for (int dimension = 0; dimension < 3; dimension++)
    {
        for (int equationNo = 0; equationNo < 3; equationNo++, i_row++, j_row++)
        {
            for (int localVertexIndex = 0; localVertexIndex < 4; localVertexIndex++)
            {
                int varIndex = getVariableIndex(model, triangleIndex, localVertexIndex, dimension);
                //append to sparseMatrix
                addCholmodEntry(M, i_row, varIndex, (double)em.x[j_row][localVertexIndex]);
            }
        }
    }
    return i_row;
}
static int appendRhsVectorToLinearSystem(glm::mat3x3 T, cholmod_dense *C, int i_row)
{
    //glm::mat3x3 indexing is columnmajor
    cholmod_modifyvec(C, i_row, T[0][0]);
    i_row++;
    cholmod_modifyvec(C, i_row, T[1][0]);
    i_row++;
    cholmod_modifyvec(C, i_row, T[2][0]);
    i_row++;
    cholmod_modifyvec(C, i_row, T[0][1]);
    i_row++;
    cholmod_modifyvec(C, i_row, T[1][1]);
    i_row++;
    cholmod_modifyvec(C, i_row, T[2][1]);
    i_row++;
    cholmod_modifyvec(C, i_row, T[0][2]);
    i_row++;
    cholmod_modifyvec(C, i_row, T[1][2]);
    i_row++;
    cholmod_modifyvec(C, i_row, T[2][2]);
    i_row++;
    return i_row;
}
void startCholmod()
{
    cholmod_start(&common);
    cm = &common;
}
void allocateMemory(Mesh *target, corresDict corresmap, cholmod_triplet **A, cholmod_dense **C)
{
    int n_row = 9 * target->getTriangles().size();
    int n_col = 3 * (target->getVertices().size() + target->getTriangles().size());
    std::cout << "Problem size: " << n_row << " rows and " << n_col << " columns" << std::endl;
    *A = cholmod_allocate_triplet(n_row, n_col, 0, CHOLMOD_INT, CHOLMOD_REAL, cm);
    *C = cholmod_zeros(n_row, 1, CHOLMOD_REAL, cm);
}
void buildCoefficientMatrix(Mesh *target, corresDict corresMap, cholmod_triplet *A)
{
    int i_row = 0;
    int oldPercent = -1, newPercent = -1;

    std::vector<glm::mat3x3> surfaceInvList = calcSurfaceInvList(target);
    int numTriangles = target->getTriangles().size();
    for (int triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        elementaryMatrix em = calculateElementaryMatrix(surfaceInvList.at(triangleIndex));

        i_row = appendElementaryMatrixToLinearSystem(target, A, triangleIndex, em, i_row);
        newPercent = (100 * triangleIndex) / numTriangles;
        if (newPercent != oldPercent)
        {
            oldPercent = newPercent;
            std::cout << newPercent << " % abgeschlossen\r" << std::flush;
        }
    }
    std::cout << "appended " << i_row << " entries" << std::endl;
}

void buildRhsVector(Mesh *sourceDeformed, Mesh *targetRef, std::vector<glm::mat3x3> &invertedSurfaceMatrix_ref, corresDict corresmap, cholmod_dense *C)
{

    int i_row = 0;
    std::vector<Triangle> targetTriangles = targetRef->getTriangles();
    std::vector<Triangle> sourceDeformedTriangles = sourceDeformed->getTriangles();
    for (int triangleIndex = 0; triangleIndex < targetTriangles.size(); triangleIndex++)
    {

        int indexSourceTriangle = corresmap[triangleIndex];
        Triangle t = sourceDeformedTriangles.at(indexSourceTriangle);
        glm::mat3x3 V = calculateSurfaceMatrix(sourceDeformed, t);
        glm::mat3x3 T = V * invertedSurfaceMatrix_ref.at(indexSourceTriangle);
        //add Transformationterms to Linear system
        i_row = appendRhsVectorToLinearSystem(T, C, i_row);
    }
    std::cout << "appended " << i_row << " entries" << std::endl;
}
