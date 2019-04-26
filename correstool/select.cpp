#include "renderer.h"
#include "mesh.h"
#include "triangle.h"
#include <GL/freeglut.h>

int processHits(int hits, const GLuint *selectBuf);
double PICK_TOL = 10;
using namespace std;
int Renderer::findTriangle(Mesh m, int x, int y)
{
  extern int WINDOWWIDTH, WINDOWHEIGHT;
  extern float ZOOMLEVEL;
  extern float ROTATEX, ROTATEY, TRANSLATEX, TRANSLATEY;
  GLuint selectBuffer[17000];
  GLint hits;
  GLint viewport[4];
  glSelectBuffer(17000, selectBuffer);

  glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(0);
  //glViewport(0, 0, WINDOWWIDTH / 2, WINDOWHEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glGetIntegerv(GL_VIEWPORT, viewport);
  gluPickMatrix(x, y, 1, 1, viewport);
  gluPerspective(/* field of view in degree */ 40.0,
                 /* aspect ratio */ (float)WINDOWWIDTH / (float)WINDOWHEIGHT,
                 /* Z near */ 0.01, /* Z far */ 100.0);
  gluLookAt(0.0, 0.5, 3.0, /* eye is at (0,0,5) */
            0.0, 0.0, 0.0, /* center is at (0,0,0) */
            0.0, 1.0, 0.); /* up is in positive Y direction */
  /* Adjust cube position to be asthetic angle. */

  glTranslatef(TRANSLATEX, TRANSLATEY, ZOOMLEVEL);
  glRotatef(ROTATEX, 1.0, 0.0, 0.0);
  glRotatef(ROTATEY, 0.0, 1.0, 0.0);

  int triangle_index;
  vector<Triangle> triangles = m.getTriangles();
  for (triangle_index = 0; triangle_index < triangles.size(); triangle_index++)
  {
    Triangle t = triangles.at(triangle_index);
    vector<glm::vec3> points = t.getPoints();
    vector<glm::vec3> normals = t.getNormals();
    glLoadName((GLuint)triangle_index);
    glBegin(GL_TRIANGLES);
    //glNormal3f(normals.at(0).x, normals.at(0).y, normals.at(0).z);
    glVertex3f(points.at(0).x, points.at(0).y, points.at(0).z);
    //glNormal3f(normals.at(1).x, normals.at(1).y, normals.at(1).z);
    glVertex3f(points.at(1).x, points.at(1).y, points.at(1).z);
    //glNormal3f(normals.at(2).x, normals.at(2).y, normals.at(2).z);
    glVertex3f(points.at(2).x, points.at(2).y, points.at(2).z);
    glEnd();
  }
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glFlush();
  hits = glRenderMode(GL_RENDER);

  return processHits(hits, selectBuffer);
}
/*Vom GithubProjekt übernommen
* 
*/
int processHits(int hits, const GLuint *selectBuf)
{
  GLuint i_obj;        /* name(id, index) of the selected object */
  GLuint depth, min_z; /* distance of the object from cam */
  int i;

  if (hits)
  {
    i_obj = selectBuf[3]; /* name of the first hitten object */
    min_z = selectBuf[1]; /* initialize min_z with the depth value of the
                                  first hitten object */

    for (i = 1; i < hits; i++)
    {
      depth = selectBuf[i * 4 + 1];
      if (depth < min_z) /* if this is an upper layer object? */
      {
        i_obj = selectBuf[i * 4 + 3]; /* name of this object */
        min_z = depth;
      }
    }
    return (int)i_obj;
  }
  else
  {
    return -1; /* no object is hitten (picked up) */
  }
}
/*Ende Github Code*/
