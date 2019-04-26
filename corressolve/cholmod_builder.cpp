#include "cholmod_builder.h"
#include "SuiteSparse_config.h"
#include "cholmod_builder.h"
#include "umfpack.h"
#include "surfaceInvMatrix.h"
#include "3dtree.h"
cholmod_common common, *cm;
std::map<int, int> vertexVarMap;
struct elementaryTerm
{
    float elementaryMatrix[9][4] = {{0}};
    float elementaryVector[9] = {0};
};

static void addCholmodEntry(cholmod_triplet *T, int i, int j, double x);

static void appendElementaryTermToLinearSystem(Triangle t, std::vector<Correspondence> corresList, cholmod_triplet *M, cholmod_dense *C, elementaryTerm et, int row, float weight, int num_free);
static int appendSmoothness2LinearSystem(Mesh *source, AdjacentMatrix *adj, std::vector<elementaryTerm> etl, cholmod_triplet **M, cholmod_dense **C, float weight_smooth, int i_row, int num_free, std::vector<Correspondence> corresList);
static int appendIdentity2LinearSystem(Mesh *source, std::vector<elementaryTerm> etl, cholmod_triplet **M, cholmod_dense **C, float weight_identity, int i_row, std::vector<Correspondence> corresList, int num_free);
static int appendMinDist2LinearSystem(Mesh *source, Mesh *target, std::vector<Correspondence> corresList, ThreeDTree *tdt, cholmod_triplet **M, cholmod_dense **C, int num_free, int i_row, float weight);
static void calculateU1Coefficients(glm::mat3x3 surfaceInvMat, float *coef);
static elementaryTerm calculateElementaryTerm(Mesh *source, Mesh *target, std::vector<Correspondence> corresList, glm::mat3x3 surfaceInvMatrix, int index);
static std::vector<elementaryTerm> createElementaryTermList(Mesh *source, Mesh *target, std::vector<Correspondence> corresList, std::vector<glm::mat3x3> surfaceInvList);
static std::map<int, int> buildVertexVarMap(Mesh *h, std::vector<Correspondence> corresList);
/*sehr stark vom github projekt abgeschaut, ist aber auch ähnlich zu beispielcodes von UMFPACK und CHOLMOD*/
cholmod_dense *solveLeastSquares(cholmod_sparse *A, cholmod_dense *c)
{
    void *symbolic_obj, *numeric_obj;
    cholmod_sparse *A_trans, *AtA;
    cholmod_dense *x, *b = cholmod_zeros(A->ncol, 1, CHOLMOD_REAL, cm);
    A_trans = cholmod_transpose(A, 2, cm);
    //calculate b = A'*c
    double alpha[2] = {1, 0};
    double beta[2] = {0, 0};
    cholmod_sdmult(A_trans, 0, alpha, beta, c, b, cm);
    AtA = cholmod_aat(A_trans, NULL, 0, 1, cm);
    cholmod_sort(AtA, cm);

    //Do not use cholmod as it is not stable, umfpack returns better results, it is also used by the github project and other implementations

    //cholesky factorization
    //cholmod_factor *L = cholmod_analyze(AtA, cm);
    //cholmod_factorize(AtA, L, cm);
    //x = cholmod_solve(CHOLMOD_A, L, b, cm);

    cholmod_free_sparse(&A_trans, cm);

    umfpack_di_symbolic((int)AtA->nrow, (int)AtA->ncol,
                        (const int *)AtA->p,
                        (const int *)AtA->i,
                        (const double *)AtA->x,
                        &symbolic_obj,
                        NULL, NULL);
    umfpack_di_numeric((const int *)AtA->p,
                       (const int *)AtA->i,
                       (const double *)AtA->x,
                       symbolic_obj,
                       &numeric_obj,
                       NULL, NULL);
    umfpack_di_free_symbolic(&symbolic_obj);
    x = cholmod_allocate_dense(AtA->ncol, 1, AtA->ncol, CHOLMOD_REAL, cm);
    umfpack_di_solve(UMFPACK_A,
                     (const int *)AtA->p,
                     (const int *)AtA->i,
                     (const double *)AtA->x,
                     (double *)x->x,
                     (const double *)b->x,
                     numeric_obj,
                     NULL, NULL);
    umfpack_di_free_numeric(&numeric_obj);
    cholmod_free_sparse(&AtA, cm);
    cholmod_free_dense(&b, cm);
    return x;
}
void build_linearProblem_Phase1(Mesh *source,
                                Mesh *target,
                                AdjacentMatrix *adj,
                                std::vector<Correspondence> &CorrespondenceList,
                                cholmod_triplet **M, cholmod_dense **C, //outputParams
                                double weight_smooth,
                                double weight_identity)
{
    int n_rows, n_cols;
    n_rows = 9 * (source->getTriangles().size()) + 9 * adj->getNumAdjacences();
    //n_rows = 9 * source->getTriangles().size();
    int n_free_Vertices = source->getVertices().size() - CorrespondenceList.size();
    n_cols = 3 * (n_free_Vertices + source->getTriangles().size());
    std::cout << "Problemsize: " << n_rows << " rows and " << n_cols << " cols" << std::endl;
    *M = cholmod_allocate_triplet(n_rows, n_cols, 0, 0, CHOLMOD_REAL, cm);
    *C = cholmod_zeros(n_rows, 1, CHOLMOD_REAL, cm);
    vertexVarMap = buildVertexVarMap(source, CorrespondenceList);
    std::cout << "calculate inverted surface matrices" << std::endl;
    std::vector<glm::mat3x3> surfaceInvList = calcSurfaceInvList(source);
    std::cout << "calculate elementary terms, this step may take a while...(could be improved by a better way of determining if a vertex is free or not)" << std::endl;
    std::vector<elementaryTerm> etl = createElementaryTermList(source, target, CorrespondenceList, surfaceInvList);
    std::cout << "elementary terms calculated. append Smoothness terms to linear system" << std::endl;
    int i_row = 0;
    i_row = appendSmoothness2LinearSystem(source, adj, etl, M, C, weight_smooth, i_row, n_free_Vertices, CorrespondenceList);
    std::cout << "appended" << i_row << " lines for smoothnes terms to linear system. appending now identity terms to linear system" << std::endl;
    i_row = appendIdentity2LinearSystem(source, etl, M, C, weight_identity, i_row, CorrespondenceList, n_free_Vertices);
    std::cout << "all terms appended. total rows appended: " << i_row << std::endl;
}
void build_linearProblem_Phase2(Mesh *source,
                                Mesh *target,
                                AdjacentMatrix *adj,
                                std::vector<Correspondence> &CorrespondenceList,
                                cholmod_triplet **M, cholmod_dense **C, //outputParams
                                double weight_smooth,
                                double weight_identity,
                                double weight_closest)
{
    int n_rows, n_cols;
    int n_free_Vertices = source->getVertices().size() - CorrespondenceList.size();
    n_rows = 9 * (source->getTriangles().size()) + 9 * adj->getNumAdjacences() + 3 * n_free_Vertices;
    n_cols = 3 * (n_free_Vertices + source->getTriangles().size());
    std::cout << "Problem size: " << n_rows << " rows and " << n_cols << "columns" << std::endl;
    *M = cholmod_allocate_triplet(n_rows, n_cols, 0, 0, CHOLMOD_REAL, cm),
    *C = cholmod_zeros(n_rows, 1, CHOLMOD_REAL, cm);
    std::cout << "calculate inverted surface matrices" << std::endl;
    std::vector<glm::mat3x3> surfaceInvList = calcSurfaceInvList(source);
    std::cout << "calculate all elementary terms, this may take a while" << std::endl;
    std::vector<elementaryTerm> etl = createElementaryTermList(source, target, CorrespondenceList, surfaceInvList);
    std::cout << "building kd-tree with k = 3 for faster nearest neighbour search in target model" << std::endl;
    ThreeDTree *tdt = new ThreeDTree(target);
    int i_row = 0;
    std::cout << "appending smoothness terms to linear system" << std::endl;
    i_row = appendSmoothness2LinearSystem(source, adj, etl, M, C, weight_smooth, i_row, n_free_Vertices, CorrespondenceList);
    std::cout << "appended " << i_row << " lines. appending identity terms now ..." << std::endl;
    i_row = appendIdentity2LinearSystem(source, etl, M, C, weight_identity, i_row, CorrespondenceList, n_free_Vertices);
    std::cout << "appended " << i_row << "lines. appending minDist terms now ..." << std::endl;
    i_row = appendMinDist2LinearSystem(source, target, CorrespondenceList, tdt, M, C, n_free_Vertices, i_row, weight_closest);
    std::cout << "all terms appended. total lines appended: " << i_row << std::endl;
}
static std::vector<elementaryTerm> createElementaryTermList(Mesh *source, Mesh *target, std::vector<Correspondence> corresList, std::vector<glm::mat3x3> surfaceInvList)
{
    int oldpercent = -1;
    std::vector<elementaryTerm> elementaryTermList;
    std::vector<Triangle> sourceTriangles = source->getTriangles();
    for (int i = 0; i < sourceTriangles.size(); i++)
    {
        Triangle t = sourceTriangles.at(i);
        glm::mat3x3 sufaceInv = surfaceInvList.at(i);
        elementaryTerm et = calculateElementaryTerm(source, target, corresList, sufaceInv, i);
        elementaryTermList.push_back(et);
        int newpercent = (100 * i) / source->getTriangles().size();
        if (newpercent != oldpercent)
        {
            oldpercent = newpercent;
            std::cout << newpercent << " % abgeschlossen\r" << std::flush;
        }
    }
    std::cout << std::endl;
    return elementaryTermList;
}
static elementaryTerm calculateElementaryTerm(Mesh *source, Mesh *target, std::vector<Correspondence> corresList, glm::mat3x3 surfaceInvMat, int index)
{
    elementaryTerm et;
    float coefu1[3];
    float coef;
    std::vector<glm::vec3> targetVertices = target->getVertices();
    calculateU1Coefficients(surfaceInvMat, coefu1);
    std::vector<int> vertexIndices = source->getTriangles().at(index).getGlobalVertexIndices();
    int corresIndexV0 = getCorresIndex(corresList, vertexIndices[0]);
    int corresIndexV1 = getCorresIndex(corresList, vertexIndices[1]);
    int corresIndexV2 = getCorresIndex(corresList, vertexIndices[2]);

    for (int i = 0, row = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++, row++)
        {
            coef = coefu1[j];
            //if free,  edit M of M*X=C to calculate X, C is 0 (minimization), if it is a constrained Vertex, edit C so there is no effect to the corresponding Var in X but the Constrained takes effect on the other Vertices
            if (corresIndexV0 == -1)
            {
                et.elementaryMatrix[row][0] = coef;
            }
            else
            {
                glm::vec3 targetVertex = targetVertices.at(corresIndexV0);
                et.elementaryVector[row] -= coef * targetVertex[i];
            }
            coef = surfaceInvMat[j][0];

            if (corresIndexV1 == -1)
            {
                et.elementaryMatrix[row][1] = coef;
            }
            else
            {
                glm::vec3 targetVertex = targetVertices.at(corresIndexV1);
                et.elementaryVector[row] -= coef * targetVertex[i];
            }
            coef = surfaceInvMat[j][1];
            if (corresIndexV2 == -1)
            {
                et.elementaryMatrix[row][2] = coef;
            }
            else
            {
                glm::vec3 targetVertex = targetVertices.at(corresIndexV2);
                et.elementaryVector[row] -= coef * targetVertex[i];
            }
            et.elementaryMatrix[row][3] = surfaceInvMat[j][2];
        }
    }
    return et;
}
static void calculateU1Coefficients(glm::mat3x3 surfaceInvMat, float *coefu1)
{
    for (int i = 0; i < 3; i++)
    {
        //glm::mat is columnMajor, so surfaceInvMat[i][0] is V_1i, [i][1] is V_2i and so on
        coefu1[i] = -((surfaceInvMat[i][0] + surfaceInvMat[i][1] + surfaceInvMat[i][2]));
    }
}
//Can be improved by a hashmap instead of a Vector of Correspondences
int getCorresIndex(std::vector<Correspondence> corresList, int vertexIndex)
{
    for (auto c : corresList)
    {
        if (c.leftVertexIndex == vertexIndex)
        {
            return c.rightVertexIndex;
        }
    }
    return -1;
}
static int appendSmoothness2LinearSystem(Mesh *source, AdjacentMatrix *adj, std::vector<elementaryTerm> etl, cholmod_triplet **M, cholmod_dense **C, float weight_smooth, int i_row, int num_free, std::vector<Correspondence> corresList)
{
    std::vector<Triangle> sourceTriangles = source->getTriangles();

    for (int i = 0; i < sourceTriangles.size(); i++)
    {
        elementaryTerm et = etl.at(i);
        std::vector<int> adjaTriangleIdx = adj->GetAdjacentTriangleIndicesForTriangleIndex(i);
        for (int j : adjaTriangleIdx)
        {

            appendElementaryTermToLinearSystem(sourceTriangles.at(i), corresList, *M, *C, et, i_row, weight_smooth, num_free);
            appendElementaryTermToLinearSystem(sourceTriangles.at(j), corresList, *M, *C, etl.at(j), i_row, -weight_smooth, num_free);
            i_row += 9;
        }
    }
    return i_row;
}
static void assignElementaryTerm(elementaryTerm *target, float matrix[][4], float vector[])
{
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            target->elementaryMatrix[row][col] = matrix[row][col];
        }
        target->elementaryVector[row] = vector[row];
    }
}
static int appendIdentity2LinearSystem(Mesh *source, std::vector<elementaryTerm> etl, cholmod_triplet **M, cholmod_dense **C, float weight_identity, int i_row, std::vector<Correspondence> corresList, int num_free)
{
    std::vector<Triangle> triangles = source->getTriangles();
    for (int triangleIndex = 0; triangleIndex < triangles.size(); triangleIndex++)
    {

        elementaryTerm et = etl.at(triangleIndex);
        float identity[] = {1.0, 0.0, 0.0,
                            0.0, 1.0, 0.0,
                            0.0, 0.0, 1.0};
        for (int i = 0; i < 9; i++)
        {
            identity[i] += et.elementaryVector[i];
        }
        elementaryTerm *etTemp = new elementaryTerm();

        assignElementaryTerm(etTemp, et.elementaryMatrix, identity);
        appendElementaryTermToLinearSystem(triangles.at(triangleIndex), corresList, *M, *C, *etTemp, i_row, weight_identity, num_free);
        i_row += 9;
    }
    return i_row;
}
static void appendElementaryTermToLinearSystem(Triangle t, std::vector<Correspondence> corresList, cholmod_triplet *M, cholmod_dense *C, elementaryTerm et, int row, float weight, int num_free)
{
    std::vector<int> vertices = t.getGlobalVertexIndices();
    int j_row = 0;
    for (int dim = 0; dim < 3; dim++)
    {
        for (int equation = 0; equation < 3; equation++, row++, j_row++)
        {
            for (int i_v = 0; i_v < 4; i_v++)
            {
                //real vertices use vertexIndex, for phantum use triangleIndex instead
                int vertexId = i_v < 3 ? vertices.at(i_v) : t.getIndex();
                //free vertex?
                if (i_v >= 3 || getCorresIndex(corresList, vertexId) < 0)
                {
                    int vertexVarIdx = getVertexVarIndex(vertexId, dim, i_v == 3, num_free);
                    addCholmodEntry(M, row, vertexVarIdx, weight * et.elementaryMatrix[j_row][i_v]);
                }
            }
            auto val = (((C)->dtype == 0) ? ((double *)((C)->x))[row] : ((float *)((C)->x))[row]);
            val = val + weight * et.elementaryVector[j_row];
            (((C)->dtype == 0) ? (((double *)((C)->x))[row] = ((double)(val))) : (((float *)((C)->x))[row] = (float)(val)));
        }
    }
}
static int appendMinDist2LinearSystem(Mesh *source, Mesh *target, std::vector<Correspondence> corresList, ThreeDTree *tdt, cholmod_triplet **M, cholmod_dense **C, int num_free, int i_row, float weight)
{
    std::vector<glm::vec3> sourceVertices = source->getVertices();
    std::vector<glm::vec3> sourceNormals = source->getNormals();
    for (int vertexIndex = 0; vertexIndex < sourceVertices.size(); vertexIndex++)
    {
        glm::vec3 sourceVertex = sourceVertices.at(vertexIndex);
        glm::vec3 sourceNormal = sourceNormals.at(vertexIndex);
        //if free vertex
        if (getCorresIndex(corresList, vertexIndex) < 0)
        {
            int targetVertexIndex = tdt->findNearestNeighbourIndex(sourceVertex, sourceNormal);
            glm::vec3 targetVertex = target->getVertices().at(targetVertexIndex);
            int varIndex_x, varIndex_y, varIndex_z;
            varIndex_x = getVertexVarIndex(vertexIndex, 0, false, num_free);
            varIndex_y = getVertexVarIndex(vertexIndex, 1, false, num_free);
            varIndex_z = getVertexVarIndex(vertexIndex, 2, false, num_free);

            addCholmodEntry(*M, i_row, varIndex_x, weight);
            ((*C)->dtype == 0) ? (((double *)((*C)->x))[i_row] = ((double)(weight * targetVertex.x))) : (((float *)((*C)->x))[i_row] = ((weight * targetVertex.x)));
            i_row++;
            addCholmodEntry(*M, i_row, varIndex_y, weight);
            ((*C)->dtype == 0) ? (((double *)((*C)->x))[i_row] = ((double)(weight * targetVertex.y))) : (((float *)((*C)->x))[i_row] = ((weight * targetVertex.y)));
            i_row++;
            addCholmodEntry(*M, i_row, varIndex_z, weight);
            ((*C)->dtype == 0) ? (((double *)((*C)->x))[i_row] = ((double)(weight * targetVertex.z))) : (((float *)((*C)->x))[i_row] = ((weight * targetVertex.z)));
            i_row++;
        }
    }
    return i_row;
}
int getVertexVarIndex(int vertexIndex, int dim, bool isPhantomVertex, int num_free)
{
    return isPhantomVertex ? (num_free + vertexIndex) * 3 + dim : vertexVarMap[vertexIndex] + dim;
}
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
static std::map<int, int> buildVertexVarMap(Mesh *h, std::vector<Correspondence> corresList)
{
    std::map<int, int> newMap;
    int varRootIndex = 0;
    for (int i = 0; i < h->getVertices().size(); i++)
    {
        //if is constrained
        if (getCorresIndex(corresList, i) > -1)
        {
            newMap[i] = -1;
        }
        else
        {
            newMap[i] = varRootIndex;
            varRootIndex += 3;
        }
    }
    return newMap;
}