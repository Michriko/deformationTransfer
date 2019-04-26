#include "CorrespondenceProblem.h"
#include "cholmod_builder.h"
#include <iostream>
#include <map>
CorrespondenceProblem::CorrespondenceProblem(Mesh *source,
                                             Mesh *target,
                                             std::vector<Correspondence> userMapping, AdjacentMatrix *adj) : source(source), target(target), userMapping(userMapping)
{
    this->adj = adj;
}

static void cm_errorhandler(int status, const char *file, int line, const char *message)
{
    fprintf(stderr, "cholmod error: file : %s line: %d status: %d: %s\n", file, line, status, message);
}
void CorrespondenceProblem::start_cholmod()
{
    cholmod_start(&common);
    cm = &common;
    //cm->error_handler = cm_errorhandler;
}
void CorrespondenceProblem::solvePhase1()
{
    cholmod_triplet **M = (cholmod_triplet **)malloc(sizeof(*M));
    cholmod_dense **C = (cholmod_dense **)malloc(sizeof(*C));
    cholmod_dense *
        x;
    cholmod_sparse *A;
    std::cout << "build linear problem phase 1" << std::endl;
    build_linearProblem_Phase1(source, target, adj, userMapping, M, C, weight_smooth, weight_identity);
    std::cout << "Phase 1 build, solving Phase 1 now..." << std::endl;
    cholmod_check_triplet(*M, cm);
    cholmod_check_dense(*C, cm);
    cholmod_print_triplet(*M, "M", cm);
    cholmod_print_dense(*C, "C", cm);
    A = cholmod_triplet_to_sparse(*M, (*M)->nnz, cm);
    x = solveLeastSquares(A, *C);
    cholmod_free_triplet(M, cm);
    cholmod_free_sparse(&A, cm);
    cholmod_free_dense(C, cm);
    std::cout << "solved least squares problem, applying deformation to source model" << std::endl;
    applyDeformation(x);
    std::cout << "deformation applied to source model" << std::endl;
}
void CorrespondenceProblem::solvePhase2()
{
    cholmod_triplet **M = (cholmod_triplet **)malloc(sizeof(*M));
    cholmod_dense **C = (cholmod_dense **)malloc(sizeof(*C));
    cholmod_dense *x;
    cholmod_sparse *A;
    std::cout << "begin iterative closest point loop" << std::endl;
    //ClosestPointIteration
    for (double weight_closest = weight_closest_start;
         weight_closest < weight_closest_end;
         weight_closest *= 18)
    {
        //buildLinearSystem
        std::cout << "building linear problem with current closest weight: " << weight_closest << std::endl;
        build_linearProblem_Phase2(source, target, adj, userMapping, M, C, weight_smooth, weight_identity, weight_closest);
        std::cout << "solving linear problem..." << std::endl;
        A = cholmod_triplet_to_sparse(*M, (*M)->nnz, cm);
        cholmod_free_triplet(M, cm);
        x = solveLeastSquares(A, *C);
        cholmod_free_sparse(&A, cm);
        cholmod_free_dense(C, cm);
        std::cout << "applying deformation to source model" << std::endl;
        applyDeformation(x);
        std::cout << "deformation applied to source model" << std::endl;
        source->writeObjFile("afterPhase2_step_" + std::to_string(weight_closest) + ".obj");
    }
    std::cout << "finished phase 2" << std::endl;
}
void CorrespondenceProblem::solveFinalize()
{
    cholmod_finish(cm);
}
void CorrespondenceProblem::Solve()
{
    std::cout << "starting cholmod" << std::endl;
    start_cholmod();
    std::cout << "solving phase 1" << std::endl;
    solvePhase1();
    source->writeObjFile("preliminary.obj");
    std::cout << "solving phase 2" << std::endl;
    solvePhase2();
    source->writeObjFile("afterPhase2.obj");
    std::cout << "stopping cholmod" << std::endl;
    solveFinalize();
}
/*Nächste Zeile vom Github repo übernommen*/
#define cholmod_getValue(vec, i) \
    (((vec)->dtype == 0) ? ((double *)((vec)->x))[i] : ((float *)((vec)->x))[i])
/*Ende Github Code*/

void CorrespondenceProblem::applyDeformation(cholmod_dense *x)
{
    //iterate over all sourceVertices
    for (int vertexIndex = 0; vertexIndex < source->getVertices().size(); vertexIndex++)
    {
        //if free vertex, take calculation, else take usermapping
        int corresIndex = getCorresIndex(userMapping, vertexIndex);
        if (corresIndex < 0)
        {
            glm::vec3 newValue;
            newValue.x = cholmod_getValue(x, getVertexVarIndex(vertexIndex, 0, false, 0));
            newValue.y = cholmod_getValue(x, getVertexVarIndex(vertexIndex, 1, false, 0));
            newValue.z = cholmod_getValue(x, getVertexVarIndex(vertexIndex, 2, false, 0));

            source->setVertex(vertexIndex, newValue);
        }
        else
        {

            glm::vec3 targetVertex = target->getVertices().at(corresIndex);
            source->setVertex(vertexIndex, targetVertex);
        }
    }
    source->makeTrianglesConsistentWithVertices();
}