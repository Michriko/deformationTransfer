
#include "mesh.h"
#include <iostream>
#include <string>
#include "adjacent.h"
using namespace std;
//Adjazenz berechnung,
//momentan noch bruteforce, kann aber deutlich verbessert werden!
//da dies nur ein teilproblem ist, das "offline" berechnet werden
//kann, hab ich lieber die größeren probleme versucht effizienter zu programmieren
int main(int argc, char *argv[])
{
  string current_exec_name = argv[0];
  string first_arg;
  vector<string> all_args;
  if (argc < 2)
  {
    cout << "Usage: adjCalc pathToObjFile" << endl;
    exit(1);
  }
  first_arg = argv[1];
  all_args.assign(argv + 1, argv + argc);

  Mesh *m = new Mesh(all_args[0]);
  AdjacentMatrix adj1(*m);
  adj1.save("adjacence.adj");
}