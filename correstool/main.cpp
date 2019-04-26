#include "mesh.h"
#include "renderer.h"
#include "controls.h"
#include <GL/freeglut.h>
#include <string>
#include <iostream>

using namespace std;

void render(void);
static void onMousePressed(int key, int state, int x, int y);
static void onMouseMoved(int x, int y);
static void onKeyPressed(unsigned char key, int x, int y);
static void onReshape(int width, int height);
Renderer *r;
Controls *c;
Mesh *m1, *m2;
int width = 1200,
    height = 800;

int main(int argc, char **argv)
{
  string current_exec_name = argv[0];
  string first_arg;
  vector<string> all_args;
  if (argc < 3)
  {
    cout << "Usage: Corres sourceObjFile targetObjFile " << endl;
    exit(1);
  }
  first_arg = argv[1];
  all_args.assign(argv + 1, argv + argc);

  m1 = new Mesh(all_args[1]);
  m2 = new Mesh(all_args[0]);
  r = new Renderer(*m1, *m2, width, height);
  c = new Controls(r);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Correspondence selector");
  glEnable(GL_DEPTH_TEST);
  glutDisplayFunc(render);
  glutMouseFunc(onMousePressed);
  glutMotionFunc(onMouseMoved);
  glutKeyboardFunc(onKeyPressed);
  glutReshapeFunc(onReshape);
  glutMainLoop();
}
void render(void)
{
  r->displayFunc();
}
static void onMousePressed(int key, int state, int x, int y)
{
  c->onMousePressed(key, state, x, y);
}
static void onMouseMoved(int x, int y)
{
  c->onMouseMoved(x, y);
}
static void onKeyPressed(unsigned char key, int x, int y)
{
  c->onKeyPressed(key, x, y);
}
static void onReshape(int width, int height)
{
  r->onReshape(width, height);
}
