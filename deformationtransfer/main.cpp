#include "mesh.h"
#include "transformer.h"
#include "correspondences.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
int main(int argc, char *argv[])
{
    string current_exec_name = argv[0];
    string first_arg;
    vector<string> all_args;
    if (argc < 5)
    {
        cout << "Usage: defTrans sourceObjFile targetRefObj sourceTransformed correspondences" << endl;
        exit(1);
    }
    first_arg = argv[1];
    all_args.assign(argv + 1, argv + argc);
    cout << "Reading obj files" << endl;
    Mesh *sourceRef = new Mesh(all_args[0]);
    Mesh *targetRef = new Mesh(all_args[1]);
    Mesh *deformedSource = new Mesh(all_args[2]);
    cout << "Reading corresFile" << endl;
    std::vector<Correspondence> corresList = readCorresFile(all_args[3]);
    //build linear system
    cout << "Build transformer (matrix)" << endl;
    Transformer *t = new Transformer(sourceRef, targetRef, corresList);
    //Apply transformation
    cout << "applying transformation to target" << endl;
    t->applyToTargetMesh(deformedSource);

    targetRef->writeObjFile("deformed.obj");
}