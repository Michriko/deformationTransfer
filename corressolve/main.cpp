
#include "mesh.h"
#include <iostream>
#include "cholmod.h"
#include "correspondences.h"
#include "adjacent.h"
#include "CorrespondenceProblem.h"
#include "triangleMapping.h"
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
  string current_exec_name = argv[0];
  string first_arg;
  vector<string> all_args;
  if (argc < 5)
  {
    cout << "Usage: CorresSolve sourceObjFile targetObjFile SourceAdjFile markerPointFile" << endl;
    exit(1);
  }
  first_arg = argv[1];
  all_args.assign(argv + 1, argv + argc);
  cout << "loading all files..." << endl;
  Mesh *source = new Mesh(all_args[0]);
  Mesh *target = new Mesh(all_args[1]);
  AdjacentMatrix *adjSource = new AdjacentMatrix();
  adjSource->load(all_args[2]);
  vector<Correspondence> corres = readCorresFile(all_args[3]);
  cout << "all files loaded" << endl;
  cout << "building corresProblem" << endl;
  CorrespondenceProblem *cpp = new CorrespondenceProblem(source, target, corres, adjSource);
  cout << "CorresProblem Built" << endl;
  cout << "Solving CorresProblem" << endl;
  cpp->Solve();
  cout << "corresProblem Solved" << endl;
  source->writeObjFile("correspondence_output.obj");
  cout << "Transformed obj wirtten to correspondence_output.obj" << endl;
  cout << "Map Triangle centroids..." << std::endl;
  vector<Correspondence> correslist = createCorrespondenceList(source, target);
  saveCorresFile(correslist, "resolved_correspondences.csv");
}